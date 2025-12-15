/*
 * trivia_db.c
 *
 *  Created on: Dec 15, 2025
 *      Author: Marta
 */


#include "trivia_db.h"

static const Question db[] = {
    { "Capital de Espana?",
      {"Madrid","Sevilla","Valencia","Bilbao"}, 0 },

    { "Bus SPI es...",
      {"Serie","Paralelo","USB","CAN"}, 0 },

    { "2 + 2 = ?",
      {"3","4","5","22"}, 1 }
};

const Question* DB_Get(uint32_t i){
    return &db[i];
}

uint32_t DB_Count(void){
    return sizeof(db) / sizeof(db[0]);
}
