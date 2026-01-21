/*
 * trivial_game.c
 *
 *  Created on: Jan 16, 2026
 *      Author: Marta
 */

#include "trivial_game.h"
#include <stdio.h>
#include <string.h>

static const uint32_t QUESTION_TIME_MS = 20000;
static const uint32_t WIN_MS = 5000;
static const uint8_t  BEEPS_PER_WIN[4] = {1, 2, 4, 7}; // 0-5:1, 5-10:2, 10-15:4, 15-20:7

/* ------------------- util preguntas ------------------- */

static int32_t next_q(Difficulty d, int32_t after) {
  uint32_t n; const Question *qs = Questions_GetAll(&n);
  for (uint32_t i=(uint32_t)(after+1); i<n; i++) if (qs[i].diff==d) return (int32_t)i;
  for (uint32_t i=0; i<n; i++) if (qs[i].diff==d) return (int32_t)i;
  return -1;
}

/* ------------------- render (sin parpadeo) ------------------- */

static void render_menu(TrivialGame *g) {
  LCD_Clear(g->lcd);
  LCD_PrintLine(g->lcd, 0, "== TRIVIAL STM32 ==");
  LCD_PrintLine(g->lcd, 1, (g->diff==DIFF_EASY) ? "> Facil"   : "  Facil");
  LCD_PrintLine(g->lcd, 2, (g->diff==DIFF_MED ) ? "> Media"   : "  Media");
  LCD_PrintLine(g->lcd, 3, (g->diff==DIFF_HARD) ? "> Dificil" : "  Dificil");
}

static void render_res(TrivialGame *g, uint8_t ok) {
  LCD_Clear(g->lcd);
  LCD_PrintLine(g->lcd, 1, ok ? "   CORRECTO! +1   " : "   INCORRECTO :(  ");
  char s[21];
  snprintf(s, sizeof(s), "Score %lu/%lu", (unsigned long)g->score, (unsigned long)g->total);
  LCD_PrintLine(g->lcd, 2, s);
  LCD_PrintLine(g->lcd, 3, "SW: siguiente");
}

static void render_end(TrivialGame *g) {
  LCD_Clear(g->lcd);
  LCD_PrintLine(g->lcd, 0, "   FIN DEL JUEGO   ");
  char s[21];
  snprintf(s, sizeof(s), "Puntuacion %lu/%u", (unsigned long)g->score, (unsigned)g->max_q);
  LCD_PrintLine(g->lcd, 2, s);
  LCD_PrintLine(g->lcd, 3, "SW: menu");
}

// limpia una línea sin LCD_Clear (evita flash)
static void clear_line(LCD_PCF8574 *lcd, uint8_t row) {
  LCD_PrintLine(lcd, row, "                    "); // 20 espacios
}

static void draw_option_line(TrivialGame *g, uint8_t opt_index, uint8_t with_arrow) {
  uint32_t n; const Question *qs = Questions_GetAll(&n);
  (void)n;
  const Question *q = &qs[g->q_index];

  char line[21];
  const char letter = (char)('A' + opt_index);
  snprintf(line, sizeof(line), "%c%c)%-.17s", with_arrow ? '>' : ' ', letter, q->opt[opt_index]);
  LCD_PrintLine(g->lcd, 1 + opt_index, line);
}

static void render_q_full(TrivialGame *g) {
  uint32_t n; const Question *qs = Questions_GetAll(&n);
  (void)n;
  const Question *q = &qs[g->q_index];

  // NO LCD_Clear -> evitamos parpadeo
  clear_line(g->lcd, 0);
  clear_line(g->lcd, 1);
  clear_line(g->lcd, 2);
  clear_line(g->lcd, 3);

  // pregunta recortada a 20 chars
  char qline[21];
  memset(qline, ' ', 20); qline[20] = 0;
  strncpy(qline, q->q, 20);
  LCD_PrintLine(g->lcd, 0, qline);

  // opciones A-D (líneas 1..4 -> aquí 1..3 y la 4 no existe, así que D va en la 3)
  // Tu LCD es 20x4: líneas válidas 0..3, así que:
  draw_option_line(g, 0, (g->sel==0)); // row 1
  draw_option_line(g, 1, (g->sel==1)); // row 2
  draw_option_line(g, 2, (g->sel==2)); // row 3? OJO: row 3 es la última
  // Para que quepan 4 opciones en 20x4: usamos filas 1,2,3 para A,B,C y D también en fila 3 no cabe.
  // En tu versión original D y tiempo iban en la fila 3 alternando C/D.
  // Aquí lo resolvemos mostrando SIEMPRE A,B,C en 1..3 y D en 3 sustituyendo C cuando selecciones 3.
  // Para mantener compatibilidad, hacemos render “mixto”:
  //  - Si sel<=2: fila 3 muestra C (con flecha si toca)
  //  - Si sel==3: fila 3 muestra D (con flecha)
  if (g->sel <= 2) {
    draw_option_line(g, 2, (g->sel==2));
  } else {
    draw_option_line(g, 3, 1); // muestra D con flecha en fila 3
  }

  g->last_sel = g->sel;
  g->needs_full_redraw = 0;
}

// actualiza SOLO lo mínimo cuando cambia sel
static void update_arrow_only(TrivialGame *g) {
  if (g->sel == g->last_sel) return;

  // Re-dibuja la fila 3 según el nuevo sel (porque alterna C/D)
  // Si antes estaba en 3 o ahora está en 3, hay que refrescar esa línea sí o sí.
  uint8_t need_row3_refresh = ((g->last_sel == 3) || (g->sel == 3));

  // Quita flecha en la anterior, si estaba en A o B (0,1)
  if (g->last_sel == 0 || g->last_sel == 1) {
    draw_option_line(g, g->last_sel, 0);
  }
  // Si estaba en C (2) y NO estamos en modo D, la línea 3 se refresca aparte
  if (g->last_sel == 2 && !need_row3_refresh) {
    draw_option_line(g, 2, 0);
  }

  // Pone flecha en la nueva, si es A o B
  if (g->sel == 0 || g->sel == 1) {
    draw_option_line(g, g->sel, 1);
  }

  // Refresca fila 3 completa si hace falta (C o D)
  if (need_row3_refresh) {
    if (g->sel == 3) {
      draw_option_line(g, 3, 1); // D con flecha
    } else {
      draw_option_line(g, 2, (g->sel==2)); // C (con flecha si sel=2)
    }
  } else {
    // si no hacía falta refresco especial y sel==2
    if (g->sel == 2) draw_option_line(g, 2, 1);
  }

  g->last_sel = g->sel;
}

/* ------------------- buzzer: secuencias no-bloqueantes ------------------- */

static void seq_start(TrivialGame *g, uint8_t n_beeps, uint16_t on_ms, uint16_t gap_ms) {
  g->seq_remaining = n_beeps;
  g->seq_on_ms = on_ms;
  g->seq_gap_ms = gap_ms;
  g->seq_next_ms = HAL_GetTick();
}

static void seq_update(TrivialGame *g) {
  if (!g->bz) return;
  if (g->seq_remaining == 0) return;

  uint32_t now = HAL_GetTick();
  if ((int32_t)(now - g->seq_next_ms) < 0) return;

  Buzzer_Beep(g->bz, g->seq_on_ms);
  g->seq_remaining--;
  g->seq_next_ms = now + g->seq_on_ms + g->seq_gap_ms;
}

static void countdown_reset(TrivialGame *g) {
  g->win = 0;
  g->beep_idx = 0;
  g->next_beep_ms = g->q_start_ms;
}

static void countdown_update(TrivialGame *g) {
  if (!g->bz) return;

  uint32_t now = HAL_GetTick();
  uint32_t elapsed = now - g->q_start_ms;

  if (elapsed >= QUESTION_TIME_MS) return;

  uint8_t win = (uint8_t)(elapsed / WIN_MS); // 0..3
  if (win > 3) win = 3;

  if (win != g->win) {
    g->win = win;
    g->beep_idx = 0;
  }

  uint8_t count = BEEPS_PER_WIN[g->win];
  if (count == 0) return;
  if (g->beep_idx >= count) return;

  uint32_t win_start = g->q_start_ms + (uint32_t)g->win * WIN_MS;
  uint32_t spacing  = WIN_MS / count; // ms
  uint32_t target   = win_start + (uint32_t)g->beep_idx * spacing;

  if ((int32_t)(now - target) >= 0) {
    Buzzer_Beep(g->bz, 40); // pitido corto
    g->beep_idx++;
  }
}

/* ------------------- flujo de juego ------------------- */

static void start_q(TrivialGame *g) {
  g->q_index = next_q(g->diff, g->q_index);
  g->sel = 0;
  g->last_sel = 0xFF;
  g->q_start_ms = HAL_GetTick();
  g->st = ST_Q;
  g->needs_full_redraw = 1;

  countdown_reset(g);

  // cancela sonidos anteriores
  g->seq_remaining = 0;
  if (g->bz) Buzzer_Off(g->bz);
}

void TrivialGame_Init(TrivialGame *g, LCD_PCF8574 *lcd, JoystickSimple *joy, Buzzer *bz) {
  memset(g, 0, sizeof(*g));
  g->lcd = lcd;
  g->joy = joy;
  g->bz  = bz;

  g->diff = DIFF_EASY;
  g->q_index = -1;
  g->st = ST_MENU;

  render_menu(g);
}

void TrivialGame_Update(TrivialGame *g) {
  // mantenimiento buzzer (no bloqueante)
  if (g->bz) Buzzer_Update(g->bz);
  seq_update(g);

  JoyEvents e = JoystickSimple_Poll(g->joy);
  uint32_t now = HAL_GetTick();

  /* ---- MENU ---- */
  if (g->st == ST_MENU) {
    if (e.up && g->diff > DIFF_EASY)   { g->diff--; render_menu(g); }
    if (e.down && g->diff < DIFF_HARD) { g->diff++; render_menu(g); }

    if (e.press) {
      g->score = 0;
      g->total = 0;
      g->q_index = -1;

      // limite preguntas por dificultad
      if (g->diff == DIFF_EASY)      g->max_q = 5;
      else if (g->diff == DIFF_MED)  g->max_q = 7;
      else                           g->max_q = 12;

      start_q(g);
      render_q_full(g);
    }
    return;
  }

  /* ---- QUESTION ---- */
  if (g->st == ST_Q) {
    if (g->needs_full_redraw) render_q_full(g);

    // cuenta atrás sonora (sin tocar LCD)
    countdown_update(g);

    if (e.up && g->sel > 0)   { g->sel--; update_arrow_only(g); }
    if (e.down && g->sel < 3) { g->sel++; update_arrow_only(g); }

    // Responder
    if (e.press) {
      uint32_t n; const Question *qs = Questions_GetAll(&n);
      (void)n;

      g->total++;
      uint8_t ok = (g->sel == qs[g->q_index].correct);
      if (ok) g->score++;

      // Sonido resultado
      if (ok) seq_start(g, 2, 40, 70);   // pi pi
      else    seq_start(g, 1, 350, 0);   // piiiii

      // ¿Fin de juego?
      if (g->total >= g->max_q) {
        g->st = ST_END;
        render_end(g);
        // opcional: sonido final según nota
        if (g->score * 2 >= g->max_q) seq_start(g, 3, 60, 80); // mini victoria
        else                         seq_start(g, 1, 600, 0);  // derrota
        return;
      }

      g->st = ST_RES;
      render_res(g, ok);
      return;
    }

    // Timeout
    if (now - g->q_start_ms >= QUESTION_TIME_MS) {
      g->total++;

      // sonido fallo/timeout
      seq_start(g, 1, 350, 0);

      // ¿Fin de juego?
      if (g->total >= g->max_q) {
        g->st = ST_END;
        render_end(g);
        if (g->score * 2 >= g->max_q) seq_start(g, 3, 60, 80);
        else                         seq_start(g, 1, 600, 0);
        return;
      }

      g->st = ST_RES;
      render_res(g, 0);
      return;
    }

    return;
  }

  /* ---- RESULT ---- */
  if (g->st == ST_RES) {
    if (e.press) {
      start_q(g);
      render_q_full(g);
    }
    return;
  }

  /* ---- END ---- */
  if (g->st == ST_END) {
    if (e.press) {
      g->st = ST_MENU;
      render_menu(g);
    }
    return;
  }
}
