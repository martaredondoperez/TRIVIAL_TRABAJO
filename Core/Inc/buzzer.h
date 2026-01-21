/*
 * buzzer.h
 *
 *  Created on: Jan 16, 2026
 *      Author: Marta
 */

#pragma once
#include "stm32f4xx_hal.h"
#include <stdint.h>

typedef struct {
  GPIO_TypeDef *port;
  uint16_t pin;

  uint8_t active_high;   // 1: pin=1 enciende, 0: pin=0 enciende

  uint8_t is_on;
  uint32_t off_ms;       // cuando apagar el pitido (HAL_GetTick)
} Buzzer;

void Buzzer_Init(Buzzer *b, GPIO_TypeDef *port, uint16_t pin, uint8_t active_high);
void Buzzer_Beep(Buzzer *b, uint32_t on_ms);   // pitido no bloqueante
void Buzzer_Update(Buzzer *b);                 // llamar en el loop
void Buzzer_Off(Buzzer *b);
