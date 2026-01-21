/*
 * trivial_game.h
 *
 *  Created on: Jan 16, 2026
 *      Author: Marta
 */

#pragma once
#include "lcd_pcf8574.h"
#include "joystick_simple.h"
#include "trivial_questions.h"
#include "buzzer.h"
#include <stdint.h>

typedef struct {
  LCD_PCF8574 *lcd;
  JoystickSimple *joy;
  Buzzer *bz;

  Difficulty diff;
  uint32_t score;
  uint32_t total;
  uint8_t  max_q;            // limite de preguntas por partida

  int32_t q_index;
  uint8_t sel;               // 0..3
  uint8_t last_sel;          // para actualizar solo flecha
  uint32_t q_start_ms;

  uint8_t needs_full_redraw;

  // Cuenta atrás sonora (ventanas de 5s)
  uint8_t  win;              // 0..3
  uint8_t  beep_idx;         // pitido actual dentro de la ventana
  uint32_t next_beep_ms;      // (no imprescindible, pero lo dejamos)

  // Secuencias de pitidos (win/lose) no bloqueantes
  uint8_t  seq_remaining;
  uint32_t seq_next_ms;
  uint16_t seq_on_ms;
  uint16_t seq_gap_ms;

  enum { ST_MENU=0, ST_Q, ST_RES, ST_END } st;
} TrivialGame;

void TrivialGame_Init(TrivialGame *g, LCD_PCF8574 *lcd, JoystickSimple *joy, Buzzer *bz);
void TrivialGame_Update(TrivialGame *g);
