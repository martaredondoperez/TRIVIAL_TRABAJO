/**
 * @file    tft.c
 * @brief   Implementación de la capa de bajo nivel para el hardware TFT (LCD).
 * ADAPTADO A SPI1 y PINES DE CONTROL PB6/PB7/PB8.
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "main.h"      // Necesario para las funciones HAL y definiciones GPIO
#include "tft.h"       // Prototipos del TFT
#include "spi.h"       // Necesario para el handle hspi1
// Si usas una fuente bitmap, incluye aquí el archivo de la fuente (ej. "font_8x10.h")

/* Externs -------------------------------------------------------------------*/

// Handle de la estructura SPI definido en spi.c
extern SPI_HandleTypeDef hspi1;

/* Private defines -----------------------------------------------------------*/

// PINES DE CONTROL DE LA PANTALLA TFT (Deducidos de gpio.c)
#define TFT_RST_PORT    GPIOB
#define TFT_RST_PIN     GPIO_PIN_6    // Reset (Asumido PB6)
#define TFT_DC_PORT     GPIOB
#define TFT_DC_PIN      GPIO_PIN_7    // Data/Command (Asumido PB7)
#define TFT_CS_PORT     GPIOB
#define TFT_CS_PIN      GPIO_PIN_8    // Chip Select (Asumido PB8)

#define TFT_WIDTH       240           // Ancho de tu pantalla en píxeles (Adaptar si es diferente)
#define TFT_HEIGHT      320           // Alto de tu pantalla en píxeles (Adaptar si es diferente)

// DEFINICIONES DE FUENTE (Ajustar a la fuente real si usas DrawChar/DrawString)
#define FONT_WIDTH      8
#define FONT_HEIGHT     10

/* Private function prototypes -----------------------------------------------*/
static void TFT_WriteCommand(uint8_t cmd);
static void TFT_WriteData(uint8_t data);
static void TFT_SetAddressWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
static void TFT_DrawChar(uint16_t x, uint16_t y, char c, uint16_t fg_color, uint16_t bg_color);


/* Function implementations --------------------------------------------------*/

/**
 * @brief Inicializa el hardware de la pantalla TFT.
 */
void TFT_Init(void)
{
	/**
	 * @brief Inicializa el hardware de la pantalla TFT ST7735S.
	 * ESTA ES LA SECUENCIA REQUERIDA PARA EL CONTROLADOR ST7735S
	 */
	void TFT_Init(void)
	{
	    // --- 1. HARDWARE RESET ---
	    HAL_GPIO_WritePin(TFT_RST_PORT, TFT_RST_PIN, GPIO_PIN_RESET);
	    HAL_Delay(150); // El ST7735 necesita un pulso de reset más largo.
	    HAL_GPIO_WritePin(TFT_RST_PORT, TFT_RST_PIN, GPIO_PIN_SET);
	    HAL_Delay(150);

	    // --- 2. COMANDOS DE INICIALIZACIÓN (Standard Sequence) ---

	    // Salir del modo de sueño (Sleep Out)
	    TFT_WriteCommand(0x11);
	    HAL_Delay(120);

	    // Frame Rate Control (Normal Mode)
	    TFT_WriteCommand(0xB1);
	    TFT_WriteData(0x01);
	    TFT_WriteData(0x2C);
	    TFT_WriteData(0x2D);

	    // Frame Rate Control (Idle Mode)
	    TFT_WriteCommand(0xB2);
	    TFT_WriteData(0x01);
	    TFT_WriteData(0x2C);
	    TFT_WriteData(0x2D);

	    // Frame Rate Control (Partial Mode)
	    TFT_WriteCommand(0xB3);
	    TFT_WriteData(0x01);
	    TFT_WriteData(0x2C);
	    TFT_WriteData(0x2D);
	    TFT_WriteData(0x01);
	    TFT_WriteData(0x2C);
	    TFT_WriteData(0x2D);

	    // Display Inversion Control (Si ves colores negativos, cambia 0x07 por 0x00 o 0x01)
	    TFT_WriteCommand(0xB4);
	    TFT_WriteData(0x07);

	    // Power Control 1
	    TFT_WriteCommand(0xC0);
	    TFT_WriteData(0xA2);
	    TFT_WriteData(0x02);
	    TFT_WriteData(0x84);

	    // Power Control 2
	    TFT_WriteCommand(0xC1);
	    TFT_WriteData(0xC5);

	    // Power Control 3 (Normal Mode)
	    TFT_WriteCommand(0xC2);
	    TFT_WriteData(0x0A);
	    TFT_WriteData(0x00);

	    // Power Control 4 (Idle Mode)
	    TFT_WriteCommand(0xC3);
	    TFT_WriteData(0x8A);
	    TFT_WriteData(0x2A);

	    // Power Control 5 (Partial Mode)
	    TFT_WriteCommand(0xC4);
	    TFT_WriteData(0x8A);
	    TFT_WriteData(0xEE);

	    // VCOM Control 1
	    TFT_WriteCommand(0xC5);
	    TFT_WriteData(0x0E); // VCOMH
	    TFT_WriteData(0x0A); // VCOM-L

	    // Memory Access Control (MADCTL) - Define la orientación y orden de color
	    // 0x08: Orden BGR (necesario para muchos módulos) y orientación Portrait (vertical)
	    TFT_WriteCommand(0x36);
	    TFT_WriteData(0x08);

	    // Interface Pixel Format (RGB565 - 16 bits/píxel)
	    TFT_WriteCommand(0x3A);
	    TFT_WriteData(0x05);

	    // Gamma Sequence (Opcional, mejora la calidad de color)
	    // ... (comandos de Gamma PGC 0xE0 y NGC 0xE1 omitidos por brevedad, pero puedes añadirlos)

	    // Activar la pantalla (Display ON)
	    TFT_WriteCommand(0x29);
	    HAL_Delay(100);

	    // Limpieza final
	    TFT_ClearScreen(TFT_COLOR_BLACK);
	}
}


/**
 * @brief Dibuja un píxel en una coordenada específica.
 */
void TFT_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if((x >= TFT_WIDTH) || (y >= TFT_HEIGHT)) return;

    TFT_SetAddressWindow(x, y, x, y);

    // El color se divide en dos bytes (High Byte y Low Byte)
    uint8_t data[2] = {(uint8_t)(color >> 8), (uint8_t)color};

    HAL_GPIO_WritePin(TFT_DC_PORT, TFT_DC_PIN, GPIO_PIN_SET);  // Data mode
    HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_RESET); // CS low

    // Transmitir 2 bytes de color a través del SPI
    HAL_SPI_Transmit(&hspi1, data, 2, 100); // <-- ¡Sustitución hecha con hspi1!

    HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_SET);  // CS high
}

/**
 * @brief Borra toda la pantalla con un color uniforme.
 */
void TFT_ClearScreen(uint16_t color)
{
    TFT_SetAddressWindow(0, 0, TFT_WIDTH - 1, TFT_HEIGHT - 1);

    HAL_GPIO_WritePin(TFT_DC_PORT, TFT_DC_PIN, GPIO_PIN_SET); // Data mode
    HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_RESET); // CS low

    uint8_t color_data[2] = {(uint8_t)(color >> 8), (uint8_t)color};

    // Este bucle DEBE ser optimizado con DMA o Fast SPI, pero funciona para testeo.
    for(uint32_t i = 0; i < (uint32_t)TFT_WIDTH * TFT_HEIGHT; i++)
    {
        // Transmitir dos bytes de color para cada píxel
        HAL_SPI_Transmit(&hspi1, color_data, 2, 10);
    }

    HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_SET); // CS high
}

/**
 * @brief Dibuja una cadena de texto.
 */
void TFT_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t fg_color, uint16_t bg_color)
{
    // Implementación simple de DrawString llamando a DrawChar
    while(*str)
    {
        TFT_DrawChar(x, y, *str, fg_color, bg_color);
        x += FONT_WIDTH;
        str++;
    }
}


// ===================================================================
// Implementaciones Auxiliares y de Bajo Nivel
// ===================================================================

/**
 * @brief Función de bajo nivel para enviar un comando al controlador de pantalla.
 */
static void TFT_WriteCommand(uint8_t cmd)
{
    HAL_GPIO_WritePin(TFT_DC_PORT, TFT_DC_PIN, GPIO_PIN_RESET); // Command mode
    HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_RESET); // CS low

    // Transmitir 1 byte (el comando)
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 100); // <-- ¡Sustitución hecha con hspi1!

    HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_SET); // CS high
}

/**
 * @brief Función de bajo nivel para enviar un dato al controlador de pantalla.
 */
static void TFT_WriteData(uint8_t data)
{
    HAL_GPIO_WritePin(TFT_DC_PORT, TFT_DC_PIN, GPIO_PIN_SET); // Data mode
    HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_RESET); // CS low

    // Transmitir 1 byte (el dato)
    HAL_SPI_Transmit(&hspi1, &data, 1, 100); // <-- ¡Sustitución hecha con hspi1!

    HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_SET); // CS high
}


/**
 * @brief Establece la región de memoria de la pantalla a la que se escribirá.
 */
static void TFT_SetAddressWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    // Comando Column Address Set (0x2A)
    TFT_WriteCommand(0x2A);
    TFT_WriteData(x1 >> 8);
    TFT_WriteData(x1 & 0xFF);
    TFT_WriteData(x2 >> 8);
    TFT_WriteData(x2 & 0xFF);

    // Comando Page Address Set (0x2B)
    TFT_WriteCommand(0x2B);
    TFT_WriteData(y1 >> 8);
    TFT_WriteData(y1 & 0xFF);
    TFT_WriteData(y2 >> 8);
    TFT_WriteData(y2 & 0xFF);

    // Comando Memory Write (0x2C)
    TFT_WriteCommand(0x2C);
}

// NOTA: TFT_DrawChar debe tener su implementación completa con la matriz de la fuente.
static void TFT_DrawChar(uint16_t x, uint16_t y, char c, uint16_t fg_color, uint16_t bg_color)
{
    // Implementación Pendiente: Aquí se leerían los bits del carácter 'c'
    // desde la matriz de la fuente y se llamarían a TFT_DrawPixel() para cada bit.
}
