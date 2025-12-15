/*
 * input_joy.h
 *
 *  Created on: Dec 15, 2025
 *      Author: Marta
 */

#ifndef INC_INPUT_JOY_H_
#define INC_INPUT_JOY_H_

#include <stdint.h>

typedef enum {
    JOY_NONE = 0,
    JOY_UP,
    JOY_DOWN,
    JOY_CLICK
} JoyEvent;

void JOY_Init(void);
JoyEvent JOY_GetEvent(void);

#endif /* INC_INPUT_JOY_H_ */
