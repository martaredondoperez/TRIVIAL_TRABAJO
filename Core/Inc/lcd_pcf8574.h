/*
 * lcd_pcf8574.h
 *
 *  Created on: Jan 16, 2026
 *      Author: Marta
 */

#pragma once
#include "stm32f4xx_hal.h"
#include <stdint.h>

typedef enum {
  LCD_MAP_A = 0,
  LCD_MAP_B = 1
} LCD_Map;

typedef struct {
  I2C_HandleTypeDef *hi2c;
  uint8_t addr7;       // ejemplo 0x27
  uint8_t backlight;   // 0/1
  LCD_Map map;         // A o B
} LCD_PCF8574;

void LCD_Init(LCD_PCF8574 *lcd, I2C_HandleTypeDef *hi2c, uint8_t addr7, LCD_Map map);
void LCD_Backlight(LCD_PCF8574 *lcd, uint8_t on);
void LCD_Clear(LCD_PCF8574 *lcd);
void LCD_SetCursor(LCD_PCF8574 *lcd, uint8_t row, uint8_t col);
void LCD_Print(LCD_PCF8574 *lcd, const char *s);
void LCD_PrintLine(LCD_PCF8574 *lcd, uint8_t row, const char *text);
