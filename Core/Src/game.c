/*
 * game.c
 *
 * Created on: Dec 15, 2025
 * Author: Marta
 */

#include "game.h"
#include "input_joy.h"
#include "trivia_db.h"
#include "ui.h"

static uint8_t  sel = 0;
static uint32_t q   = 0;
static uint32_t score = 0;

void GAME_Init(void){
    sel = 0;
    q = 0;
    score = 0;
    UI_Init(); // Inicializa TFT (dentro de UI_Init)

    // Dibuja la primera pregunta y la puntuación inicial
    UI_DrawQuestion(DB_Get(q), sel);
    UI_DrawScore(score);
}

void GAME_Tick(void){
    JoyEvent ev = JOY_GetEvent();
    if(ev == JOY_NONE) return;

    // Guardamos la selección anterior para optimizar el redibujo
    uint8_t sel_prev = sel;

    // Lógica de Movimiento
    if(ev == JOY_UP && sel > 0) sel--;
    if(ev == JOY_DOWN && sel < 3) sel++;

    if(ev == JOY_CLICK){

        // Comprobar respuesta y actualizar puntuación
        if(sel == DB_Get(q)->correct) {
            score++;
            // Opcional: Llamar a una función de feedback (LED/sonido)
        }

        // Avanzar a la siguiente pregunta
        q = (q + 1) % DB_Count();
        sel = 0; // Resetear selección

        // Redibujar la nueva pregunta y actualizar la puntuación
        UI_DrawQuestion(DB_Get(q), sel);
        UI_DrawScore(score);
    }
    // Redibujar SOLO si hubo movimiento y la selección realmente cambió
    else if ((ev == JOY_UP || ev == JOY_DOWN) && (sel != sel_prev)) {
        // La pregunta sigue siendo la misma, pero se resalta la nueva 'sel'
        UI_DrawQuestion(DB_Get(q), sel);
    }
}
