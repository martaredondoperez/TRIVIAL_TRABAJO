/*
 * joystick_simple.c
 *
 *  Created on: Jan 16, 2026
 *      Author: Marta
 */

#include "joystick_simple.h"

static inline uint8_t sw_pressed(GPIO_TypeDef *port, uint16_t pin) {
  // Pull-up -> pulsado = 0
  return (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET) ? 1 : 0;
}

void JoystickSimple_Init(JoystickSimple *j, AdcReader *adc, GPIO_TypeDef *sw_port, uint16_t sw_pin)
{
  j->adc = adc;
  j->sw_port = sw_port;
  j->sw_pin = sw_pin;

  j->deadzone = 250;
  j->threshold = 700;
  j->repeat_ms = 180;

  j->center = 2048;

  uint32_t now = HAL_GetTick();
  j->last_dir_ms = now;
  j->last_sw_ms = now;
  j->last_sw_state = 0;
}

void JoystickSimple_Calibrate(JoystickSimple *j)
{
  uint32_t sum = 0;
  for (int i = 0; i < 16; i++) {
    uint16_t v = AdcReader_ReadBlocking(j->adc, 10);
    sum += v;
    HAL_Delay(2);
  }
  uint16_t mid = (uint16_t)(sum / 16U);

  // evita calibrar a basura si el pin estuviera mal
  if (mid > 50 && mid < 4045) j->center = mid;
}

uint16_t JoystickSimple_ReadRaw(JoystickSimple *j)
{
  return AdcReader_ReadBlocking(j->adc, 10);
}

JoyEvents JoystickSimple_Poll(JoystickSimple *j)
{
  JoyEvents e = {0};
  uint32_t now = HAL_GetTick();

  // UP/DOWN con repetición
  uint16_t v = AdcReader_ReadBlocking(j->adc, 10);
  int32_t y = (int32_t)v - (int32_t)j->center;

  if ((now - j->last_dir_ms) >= j->repeat_ms) {
    if (!(y < (int32_t)j->deadzone && y > -(int32_t)j->deadzone)) {
      if (y > (int32_t)j->threshold) {
        e.down = 1;
        j->last_dir_ms = now;
      } else if (y < -(int32_t)j->threshold) {
        e.up = 1;
        j->last_dir_ms = now;
      }
    }
  }

  // SW con debounce
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
