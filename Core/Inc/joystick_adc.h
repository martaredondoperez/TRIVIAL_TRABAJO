/*
 * joystick_adc.h
 *
 *  Created on: Jan 16, 2026
 *      Author: Marta
 */

#pragma once
#include "stm32f4xx_hal.h"
#include <stdint.h>

typedef enum {
  JOY_NONE = 0,
  JOY_UP,
  JOY_DOWN,
  JOY_LEFT,
  JOY_RIGHT
} JoyDir;

typedef struct {
  uint8_t up, down, left, right;
  uint8_t press;
} JoyEvents;

typedef struct {
  ADC_HandleTypeDef *hadc;
  volatile uint16_t *dma_buf; // [0]=X, [1]=Y

  GPIO_TypeDef *sw_port;
  uint16_t sw_pin;

  uint16_t center_x;
  uint16_t center_y;
  uint16_t deadzone;
  uint16_t threshold;
  uint32_t repeat_ms;

  uint32_t last_dir_ms;
  uint32_t last_sw_ms;
  uint8_t last_sw_state;
} JoystickADC;

void JoystickADC_Init(JoystickADC *j,
                      ADC_HandleTypeDef *hadc,
                      volatile uint16_t *dma_buf,
                      GPIO_TypeDef *sw_port, uint16_t sw_pin);

HAL_StatusTypeDef JoystickADC_Start(JoystickADC *j);
JoyDir JoystickADC_GetDir(JoystickADC *j);
JoyEvents JoystickADC_Poll(JoystickADC *j);
