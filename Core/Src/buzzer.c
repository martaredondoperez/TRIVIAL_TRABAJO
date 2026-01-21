/*
 * buzzer.c
 *
 *  Created on: Jan 16, 2026
 *      Author: Marta
 */


#include "buzzer.h"

static inline void buzzer_write(Buzzer *b, uint8_t on)
{
  GPIO_PinState st;
  if (b->active_high) st = on ? GPIO_PIN_SET : GPIO_PIN_RESET;
  else               st = on ? GPIO_PIN_RESET : GPIO_PIN_SET;

  HAL_GPIO_WritePin(b->port, b->pin, st);
}

void Buzzer_Init(Buzzer *b, GPIO_TypeDef *port, uint16_t pin, uint8_t active_high)
{
  b->port = port;
  b->pin = pin;
  b->active_high = active_high ? 1 : 0;
  b->is_on = 0;
  b->off_ms = 0;
  buzzer_write(b, 0);
}

void Buzzer_Beep(Buzzer *b, uint32_t on_ms)
{
  uint32_t now = HAL_GetTick();
  b->is_on = 1;
  b->off_ms = now + on_ms;
  buzzer_write(b, 1);
}

void Buzzer_Update(Buzzer *b)
{
  if (!b->is_on) return;
  uint32_t now = HAL_GetTick();
  if ((int32_t)(now - b->off_ms) >= 0) {
    b->is_on = 0;
    buzzer_write(b, 0);
  }
}

void Buzzer_Off(Buzzer *b)
{
  b->is_on = 0;
  buzzer_write(b, 0);
}
