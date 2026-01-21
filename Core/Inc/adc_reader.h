/*
 * adc_reader.h
 *
 *  Created on: Jan 16, 2026
 *      Author: Marta
 */

#pragma once
#include "stm32f4xx_hal.h"
#include <stdint.h>

typedef struct {
  ADC_HandleTypeDef *hadc;
  uint32_t channel;           // ADC_CHANNEL_10, etc.
  uint32_t sampling_time;     // ADC_SAMPLETIME_144CYCLES, etc.
} AdcReader;

void     AdcReader_Init(AdcReader *r, ADC_HandleTypeDef *hadc, uint32_t channel, uint32_t sampling_time);
uint16_t AdcReader_ReadBlocking(AdcReader *r, uint32_t timeout_ms);
