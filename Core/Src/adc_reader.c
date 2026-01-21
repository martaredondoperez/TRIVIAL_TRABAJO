/*
 * adc_reader.c
 *
 *  Created on: Jan 16, 2026
 *      Author: Marta
 */

#include "adc_reader.h"

void AdcReader_Init(AdcReader *r, ADC_HandleTypeDef *hadc, uint32_t channel, uint32_t sampling_time)
{
  r->hadc = hadc;
  r->channel = channel;
  r->sampling_time = sampling_time;
}

uint16_t AdcReader_ReadBlocking(AdcReader *r, uint32_t timeout_ms)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  // Fuerza canal por software (no dependes de CubeMX más allá de que ADC1 exista)
  sConfig.Channel = r->channel;
  sConfig.Rank = 1;
  sConfig.SamplingTime = r->sampling_time;

  if (HAL_ADC_ConfigChannel(r->hadc, &sConfig) != HAL_OK) {
    return 0xFFFF; // error config
  }

  if (HAL_ADC_Start(r->hadc) != HAL_OK) {
    return 0xFFFE; // error start
  }

  if (HAL_ADC_PollForConversion(r->hadc, timeout_ms) != HAL_OK) {
    HAL_ADC_Stop(r->hadc);
    return 0xFFFD; // timeout/error poll
  }

  uint16_t v = (uint16_t)HAL_ADC_GetValue(r->hadc);
  HAL_ADC_Stop(r->hadc);
  return v;
}
