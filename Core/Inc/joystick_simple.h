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
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "adc_reader.h"

typedef struct {
  uint8_t up;
  uint8_t down;
  uint8_t press;
} JoyEvents;

typedef struct {
  AdcReader *adc;              // lector ADC (VRy)
  GPIO_TypeDef *sw_port;
  uint16_t sw_pin;

  uint16_t center;
  uint16_t deadzone;
  uint16_t threshold;

  uint32_t repeat_ms;
  uint32_t last_dir_ms;

  uint32_t last_sw_ms;
  uint8_t  last_sw_state;
} JoystickSimple;

void JoystickSimple_Init(JoystickSimple *j, AdcReader *adc, GPIO_TypeDef *sw_port, uint16_t sw_pin);
void JoystickSimple_Calibrate(JoystickSimple *j);
JoyEvents JoystickSimple_Poll(JoystickSimple *j);
uint16_t JoystickSimple_ReadRaw(JoystickSimple *j);  // útil para debug
