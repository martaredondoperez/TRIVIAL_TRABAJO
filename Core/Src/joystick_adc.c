/*
 * joystick_adc.c
 *
 *  Created on: Jan 16, 2026
 *      Author: Marta
 */

#include "joystick_adc.h"

#define JOY_INVERT_Y            0   // 1 si arriba/abajo va al revés

// Suavizado (0 = sin filtro). 3..6 va bien.
#define JOY_FILTER_SHIFT        2   // 2 => promedio exponencial 1/4

static inline uint8_t sw_pressed(GPIO_TypeDef *port, uint16_t pin) {
  // Pull-up -> pulsado = 0
  return (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET) ? 1 : 0;
}

void JoystickADC_Init(JoystickADC *j,
                      ADC_HandleTypeDef *hadc,
                      volatile uint16_t *dma_buf,
                      GPIO_TypeDef *sw_port, uint16_t sw_pin)
{
  j->hadc = hadc;
  j->dma_buf = dma_buf;
  j->sw_port = sw_port;
  j->sw_pin = sw_pin;

  // Ajustes
  j->deadzone = 250;     // sube si hay ruido
  j->threshold = 700;    // 600-900 típico
  j->repeat_ms = 180;

  j->center_y = 2048;
  j->center_x = 2048; // no se usa, pero lo dejamos

  uint32_t now = HAL_GetTick();
  j->last_dir_ms = now;
  j->last_sw_ms = now;
  j->last_sw_state = 0;

  // Estado interno de filtro usando center_x como “y_filt” sin cambiar tu struct
  j->center_x = 2048;
}

HAL_StatusTypeDef JoystickADC_Start(JoystickADC *j)
{
  // Por robustez: si el ADC estaba arrancado por DMA, lo paramos
  HAL_ADC_Stop_DMA(j->hadc);
  HAL_ADC_Stop(j->hadc);

  // Arranca ADC + DMA circular (1 elemento: VRy)
  HAL_StatusTypeDef st = HAL_ADC_Start_DMA(j->hadc, (uint32_t*)j->dma_buf, 1);
  if (st != HAL_OK) return st;

  // Calibración simple del centro: promedia unas cuantas muestras
  HAL_Delay(20);

  uint32_t sum = 0;
  for (int i = 0; i < 16; i++) {
    sum += (uint32_t)j->dma_buf[0];
    HAL_Delay(2);
  }
  uint16_t mid = (uint16_t)(sum / 16U);

  // Si mid es absurdo (0 fijo / 4095 fijo), NO recalibres (indica pin mal)
  if (mid > 50 && mid < 4045) {
    j->center_y = mid;
    j->center_x = mid; // usamos center_x como y_filt
  }

  return HAL_OK;
}

JoyDir JoystickADC_GetDir(JoystickADC *j)
{
  // Lee VRy (DMA)
  uint16_t raw = j->dma_buf[0];

  // Filtro exponencial simple (suaviza ruido)
#if JOY_FILTER_SHIFT > 0
  uint16_t y_filt = (uint16_t)j->center_x;
  y_filt = (uint16_t)(y_filt + ((int32_t)raw - (int32_t)y_filt) / (1 << JOY_FILTER_SHIFT));
  j->center_x = y_filt; // guardamos filtro aquí (sin tocar el .h)
  int32_t y = (int32_t)y_filt - (int32_t)j->center_y;
#else
  int32_t y = (int32_t)raw - (int32_t)j->center_y;
#endif

  // Deadzone
  if (y < (int32_t)j->deadzone && y > -(int32_t)j->deadzone) {
    return JOY_NONE;
  }

#if JOY_INVERT_Y
  if (y > (int32_t)j->threshold)  return JOY_UP;
  if (y < -(int32_t)j->threshold) return JOY_DOWN;
#else
  if (y > (int32_t)j->threshold)  return JOY_DOWN;
  if (y < -(int32_t)j->threshold) return JOY_UP;
#endif

  return JOY_NONE;
}

JoyEvents JoystickADC_Poll(JoystickADC *j)
{
  JoyEvents e = {0};
  uint32_t now = HAL_GetTick();

  // Dirección (con repetición)
  JoyDir d = JoystickADC_GetDir(j);
  if (d != JOY_NONE && (now - j->last_dir_ms) >= j->repeat_ms) {
    j->last_dir_ms = now;
    if (d == JOY_UP) e.up = 1;
    else if (d == JOY_DOWN) e.down = 1;
  }

  // Botón SW con debounce
  uint8_t sw = sw_pressed(j->sw_port, j->sw_pin);
  if (sw != j->last_sw_state) {
    if (now - j->last_sw_ms >= 120) {
      j->last_sw_ms = now;
      j->last_sw_state = sw;
      if (sw) e.press = 1;
    }
  }

  return e;
}
