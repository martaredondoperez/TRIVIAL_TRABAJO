/*
 * game.c
 *
 *  Created on: Dec 15, 2025
 *      Author: Marta
 */


#include "game.h"
#include "input_joy.h"
#include "trivia_db.h"

static uint8_t  sel = 0;
static uint32_t q   = 0;
static uint32_t score = 0;

void GAME_Init(void){
    sel = 0;
    q = 0;
    score = 0;
}

void GAME_Tick(void){
    JoyEvent ev = JOY_GetEvent();
    if(ev == JOY_NONE) return;

    if(ev == JOY_UP && sel > 0) sel--;
    if(ev == JOY_DOWN && sel < 3) sel++;

    if(ev == JOY_CLICK){
        if(sel == DB_Get(q)->correct) score++;
        q = (q + 1) % DB_Count();
        sel = 0;
    }
    //  Aquí luego entra TFT / LEDs / sonido
}
