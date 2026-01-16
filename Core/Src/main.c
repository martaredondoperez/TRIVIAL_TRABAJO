/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// ================== AJUSTES ==================
#define LCD_ADDR7          0x27   // Direccion I2C 7-bit (Arduino suele mostrar 0x27)
#define LCD_I2C_TIMEOUT_MS 50
#define LCD_PCF_MAPPING    0      // 0 = Mapping A (tipico), 1 = Mapping B (alternativo)
#define LCD_BACKLIGHT_ON   1

// ================== DRIVER LCD (PCF8574 + HD44780) ==================

typedef struct {
  I2C_HandleTypeDef *hi2c;
  uint8_t addr7;
  uint8_t backlight;
} LCD_I2C_t;

static LCD_I2C_t lcd;

// --- PCF8574 pin mapping ---
#if (LCD_PCF_MAPPING == 0)
// Mapping A (muy común):
// P0=RS, P1=RW, P2=EN, P3=BL, P4=D4, P5=D5, P6=D6, P7=D7
  #define PIN_RS (1 << 0)
  #define PIN_RW (1 << 1)
  #define PIN_EN (1 << 2)
  #define PIN_BL (1 << 3)
  #define PIN_D4 (1 << 4)
  #define PIN_D5 (1 << 5)
  #define PIN_D6 (1 << 6)
  #define PIN_D7 (1 << 7)
#else
// Mapping B (también típico en otros backpacks):
// P0=D4, P1=D5, P2=D6, P3=D7, P4=BL, P5=EN, P6=RW, P7=RS
  #define PIN_D4 (1 << 0)
  #define PIN_D5 (1 << 1)
  #define PIN_D6 (1 << 2)
  #define PIN_D7 (1 << 3)
  #define PIN_BL (1 << 4)
  #define PIN_EN (1 << 5)
  #define PIN_RW (1 << 6)
  #define PIN_RS (1 << 7)
#endif

// LCD commands
#define LCD_CLEAR       0x01
#define LCD_ENTRY_MODE  0x06
#define LCD_DISPLAY_ON  0x0C
#define LCD_FUNCTION_4B 0x28   // 4-bit, 2-line mode (sirve para 20x4), 5x8
#define LCD_SET_DDRAM   0x80

static HAL_StatusTypeDef pcf_write(LCD_I2C_t *l, uint8_t data) {
  // HAL usa direccion 8-bit: (addr7 << 1)
  return HAL_I2C_Master_Transmit(l->hi2c, (uint16_t)(l->addr7 << 1), &data, 1, LCD_I2C_TIMEOUT_MS);
}

static void pulse_enable(LCD_I2C_t *l, uint8_t data) {
  pcf_write(l, data | PIN_EN);
  HAL_Delay(1);
  pcf_write(l, data & (uint8_t)~PIN_EN);
  HAL_Delay(1);
}

static void write4bits(LCD_I2C_t *l, uint8_t nibble, uint8_t rs) {
  uint8_t data = 0;

  // nibble -> D4..D7
  if (nibble & 0x01) data |= PIN_D4;
  if (nibble & 0x02) data |= PIN_D5;
  if (nibble & 0x04) data |= PIN_D6;
  if (nibble & 0x08) data |= PIN_D7;

  if (rs) data |= PIN_RS;
  // RW = 0 siempre (escritura)

  if (l->backlight) data |= PIN_BL;

  pcf_write(l, data);
  pulse_enable(l, data);
}

static void lcd_send(LCD_I2C_t *l, uint8_t value, uint8_t rs) {
  write4bits(l, (value >> 4) & 0x0F, rs);
  write4bits(l, value & 0x0F, rs);
  HAL_Delay(2);
}

static void lcd_cmd(LCD_I2C_t *l, uint8_t cmd) {
  lcd_send(l, cmd, 0);
}

static void lcd_data(LCD_I2C_t *l, uint8_t d) {
  lcd_send(l, d, 1);
}

static void lcd_backlight(LCD_I2C_t *l, uint8_t on) {
  l->backlight = on ? 1 : 0;
  uint8_t data = l->backlight ? PIN_BL : 0;
  pcf_write(l, data);
}

static void lcd_init(LCD_I2C_t *l) {
  l->backlight = LCD_BACKLIGHT_ON ? 1 : 0;

  HAL_Delay(50); // power-up

  // Init 4-bit
  write4bits(l, 0x03, 0); HAL_Delay(5);
  write4bits(l, 0x03, 0); HAL_Delay(5);
  write4bits(l, 0x03, 0); HAL_Delay(5);

  write4bits(l, 0x02, 0); HAL_Delay(5);

  lcd_cmd(l, LCD_FUNCTION_4B);
  lcd_cmd(l, LCD_DISPLAY_ON);
  lcd_cmd(l, LCD_CLEAR);
  HAL_Delay(5);
  lcd_cmd(l, LCD_ENTRY_MODE);
}

static void lcd_clear(LCD_I2C_t *l) {
  lcd_cmd(l, LCD_CLEAR);
  HAL_Delay(5);
}

static void lcd_set_cursor(LCD_I2C_t *l, uint8_t row, uint8_t col) {
  // offsets típicos 20x4
  static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
  if (row > 3) row = 3;
  lcd_cmd(l, LCD_SET_DDRAM | (row_offsets[row] + col));
}

static void lcd_print(LCD_I2C_t *l, const char *s) {
  while (*s) lcd_data(l, (uint8_t)*s++);
}

// ================== I2C scan (para debug) ==================
static uint8_t i2c_scan_find_first(I2C_HandleTypeDef *hi2c) {
  for (uint8_t a = 1; a < 128; a++) {
    if (HAL_I2C_IsDeviceReady(hi2c, (uint16_t)(a << 1), 2, 10) == HAL_OK) {
      return a; // primera direccion encontrada
    }
  }
  return 0; // nada
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  // 1) Scan I2C: pon breakpoint en dev_addr y mira qué sale
  uint8_t dev_addr = i2c_scan_find_first(&hi2c1);

  // Si NO encuentra nada, el LCD no va a funcionar. Se queda aquí para que lo veas en debug.
  if (dev_addr == 0) {
    // Parpadeo simple si tienes LED en la placa (opcional)
    while (1) {
      HAL_Delay(300);
    }
  }

  // 2) Init LCD (si tu modulo está en 0x27, dev_addr deberia ser 0x27)
  lcd.hi2c = &hi2c1;
  lcd.addr7 = LCD_ADDR7;

  lcd_init(&lcd);
  lcd_backlight(&lcd, 1);
  lcd_clear(&lcd);

  // 3) Prints
  lcd_set_cursor(&lcd, 0, 0);
  lcd_print(&lcd, "STM32F411 LCD 20x4");

  lcd_set_cursor(&lcd, 1, 0);
  lcd_print(&lcd, "I2C scan first:");

  // Imprime la direccion encontrada (formato hex simple)
  // (sin printf, lo hago a mano)
  char buf[21];
  memset(buf, ' ', sizeof(buf));
  buf[20] = 0;
  buf[0] = '0'; buf[1] = 'x';

  const char hex[] = "0123456789ABCDEF";
  buf[2] = hex[(dev_addr >> 4) & 0x0F];
  buf[3] = hex[dev_addr & 0x0F];

  lcd_set_cursor(&lcd, 2, 0);
  lcd_print(&lcd, buf);

#if (LCD_PCF_MAPPING == 0)
  lcd_set_cursor(&lcd, 3, 0);
  lcd_print(&lcd, "Mapping A (si no->B)");
#else
  lcd_set_cursor(&lcd, 3, 0);
  lcd_print(&lcd, "Mapping B");
#endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
