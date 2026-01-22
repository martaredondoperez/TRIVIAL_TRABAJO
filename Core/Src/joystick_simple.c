/*
 * joystick_simple.c
 *
 *  Created on: Jan 16, 2026
 *      Author: Marta
 */

#include "joystick_simple.h"

static volatile uint8_t g_sw_event = 0;

void JoystickSimple_OnSWInterrupt(void)
{
  g_sw_event = 1;
}

void JoystickSimple_Init(JoystickSimple *j,
                         ADC_HandleTypeDef *hadc,
                         GPIO_TypeDef *sw_port, uint16_t sw_pin)
{
  j->hadc = hadc;
  j->sw_port = sw_port;
  j->sw_pin = sw_pin;

  j->last_dir = 0;
  j->last_dir_ms = HAL_GetTick();
  j->repeat_ms = 180;

  j->up_th = 1200;
  j->down_th = 2800;
}

HAL_StatusTypeDef JoystickSimple_Start(JoystickSimple *j)
{
  return HAL_OK;
}

JoyEvents JoystickSimple_Poll(JoystickSimple *j)
{
  JoyEvents e = {0};
  uint32_t now = HAL_GetTick();

  HAL_ADC_Start(j->hadc);
  if (HAL_ADC_PollForConversion(j->hadc, 10) == HAL_OK) {
    uint16_t v = (uint16_t)HAL_ADC_GetValue(j->hadc);

    uint8_t dir = 0;
    if (v < j->up_th) dir = 1;
    else if (v > j->down_th) dir = 2;

    if (dir != 0 && dir != j->last_dir) {
      j->last_dir = dir;
      j->last_dir_ms = now;
      if (dir == 1) e.up = 1;
      else e.down = 1;
    } else if (dir != 0 && (now - j->last_dir_ms) >= j->repeat_ms) {
      j->last_dir_ms = now;
      if (dir == 1) e.up = 1;
      else e.down = 1;
    } else if (dir == 0) {
      j->last_dir = 0;
    }
  }
  HAL_ADC_Stop(j->hadc);

  if (g_sw_event) {
    g_sw_event = 0;
    e.press = 1;
  }

  return e;
}
