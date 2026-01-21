/*
 * trivial_questions.h
 *
 *  Created on: Jan 16, 2026
 *      Author: Marta
 */

#pragma once
#include <stdint.h>

typedef enum { DIFF_EASY=0, DIFF_MED=1, DIFF_HARD=2 } Difficulty;

typedef struct {
  const char *q;
  const char *opt[4];
  uint8_t correct;   // 0..3
  Difficulty diff;
} Question;

const Question* Questions_GetAll(uint32_t *count);
