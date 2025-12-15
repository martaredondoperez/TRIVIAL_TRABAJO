/*
 * trivia_db.h
 *
 *  Created on: Dec 15, 2025
 *      Author: Marta
 */

#ifndef INC_TRIVIA_DB_H_
#define INC_TRIVIA_DB_H_

#include <stdint.h>

typedef struct {
    const char* q;
    const char* a[4];
    uint8_t correct;
} Question;

const Question* DB_Get(uint32_t i);
uint32_t DB_Count(void);


#endif /* INC_TRIVIA_DB_H_ */
