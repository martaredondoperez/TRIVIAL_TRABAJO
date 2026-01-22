/*
 * joystick_simple.h
 *
 *  Created on: Jan 16, 2026
 *      Author: Marta
 */

/*
 * joystick_simple.h
 *
 *  Created on: Jan 16, 2026
 *      Author: Marta
 */

#pragma once
#include "adc.h"
#include "gpio.h"
#include <stdint.h>

typedef struct {
  uint8_t up;
  uint8_t down;
  uint8_t press;
} JoyEvents;

typedef struct {
  ADC_HandleTypeDef *hadc;
  GPIO_TypeDef *sw_port;
  uint16_t sw_pin;

  uint8_t last_dir;
  uint32_t last_dir_ms;
  uint32_t repeat_ms;

  uint16_t up_th;
  uint16_t down_th;
} JoystickSimple;

void JoystickSimple_Init(JoystickSimple *j,
                         ADC_HandleTypeDef *hadc,
                         GPIO_TypeDef *sw_port, uint16_t sw_pin);

HAL_StatusTypeDef JoystickSimple_Start(JoystickSimple *j);

JoyEvents JoystickSimple_Poll(JoystickSimple *j);

void JoystickSimple_OnSWInterrupt(void);
