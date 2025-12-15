/*
 * input_joy.c
 *
 *  Created on: Dec 15, 2025
 *      Author: Marta
 */

#include "input_joy.h"
#include "main.h"

extern ADC_HandleTypeDef hadc1;

#define DEADZONE 300
#define MID      2048
#define REPEAT_MS 180

static uint32_t t_last = 0;
static uint32_t t_rep  = 0;
static uint8_t  hold   = 0;

static uint8_t  btn_last = 1;
static uint32_t btn_t    = 0;

JoyEvent JOY_GetEvent(void){
    uint32_t now = HAL_GetTick();
    if(now - t_last < 10) return JOY_NONE;
    t_last = now;

    HAL_ADC_Start(&hadc1);

    HAL_ADC_PollForConversion(&hadc1, 10);
    uint16_t x = HAL_ADC_GetValue(&hadc1);

    HAL_ADC_PollForConversion(&hadc1, 10);
    uint16_t y = HAL_ADC_GetValue(&hadc1);

    HAL_ADC_Stop(&hadc1);

    JoyEvent ev = JOY_NONE;

    if(y > MID + DEADZONE){
        if(hold != 1 || now - t_rep > REPEAT_MS){
            ev = JOY_UP;
            hold = 1;
            t_rep = now;
        }
    }
    else if(y < MID - DEADZONE){
        if(hold != 2 || now - t_rep > REPEAT_MS){
            ev = JOY_DOWN;
            hold = 2;
            t_rep = now;
        }
    }
    else{
        hold = 0;
    }

    uint8_t btn = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
    if(btn != btn_last && now - btn_t > 30){
        btn_last = btn;
        btn_t = now;
        if(btn == GPIO_PIN_RESET) ev = JOY_CLICK;
    }

    return ev;
}

void JOY_Init(void){}
