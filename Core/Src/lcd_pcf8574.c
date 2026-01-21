/*
 * lcd_pcf8574.c
 *
 *  Created on: Jan 16, 2026
 *      Author: Marta
 */


#include "lcd_pcf8574.h"
#include <string.h>

#define LCD_I2C_TIMEOUT_MS 50

// LCD commands
#define LCD_CLEAR       0x01
#define LCD_ENTRY_MODE  0x06
#define LCD_DISPLAY_ON  0x0C
#define LCD_FUNCTION_4B 0x28
#define LCD_SET_DDRAM   0x80

typedef struct {
  uint8_t RS, RW, EN, BL, D4, D5, D6, D7;
} MapPins;

static MapPins mapA = {1<<0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7};
// Mapping B: P0..P3 = D4..D7, P4=BL, P5=EN, P6=RW, P7=RS
static MapPins mapB = {1<<7, 1<<6, 1<<5, 1<<4, 1<<0, 1<<1, 1<<2, 1<<3};

static MapPins pins;

static HAL_StatusTypeDef pcf_write(LCD_PCF8574 *l, uint8_t data) {
  return HAL_I2C_Master_Transmit(l->hi2c, (uint16_t)(l->addr7 << 1), &data, 1, LCD_I2C_TIMEOUT_MS);
}

static void pulse_enable(LCD_PCF8574 *l, uint8_t data) {
  pcf_write(l, data | pins.EN);
  HAL_Delay(1);
  pcf_write(l, data & (uint8_t)~pins.EN);
  HAL_Delay(1);
}

static void write4bits(LCD_PCF8574 *l, uint8_t nibble, uint8_t rs) {
  uint8_t data = 0;

  if (nibble & 0x01) data |= pins.D4;
  if (nibble & 0x02) data |= pins.D5;
  if (nibble & 0x04) data |= pins.D6;
  if (nibble & 0x08) data |= pins.D7;

  if (rs) data |= pins.RS;
  // RW siempre 0 (escritura)

  if (l->backlight) data |= pins.BL;

  pcf_write(l, data);
  pulse_enable(l, data);
}

static void lcd_send(LCD_PCF8574 *l, uint8_t value, uint8_t rs) {
  write4bits(l, (value >> 4) & 0x0F, rs);
  write4bits(l, value & 0x0F, rs);
  HAL_Delay(2);
}

static void lcd_cmd(LCD_PCF8574 *l, uint8_t cmd) { lcd_send(l, cmd, 0); }
static void lcd_data(LCD_PCF8574 *l, uint8_t d)  { lcd_send(l, d, 1); }

void LCD_Backlight(LCD_PCF8574 *lcd, uint8_t on) {
  lcd->backlight = on ? 1 : 0;
  uint8_t data = lcd->backlight ? pins.BL : 0;
  pcf_write(lcd, data);
}

void LCD_Init(LCD_PCF8574 *lcd, I2C_HandleTypeDef *hi2c, uint8_t addr7, LCD_Map map) {
  memset(lcd, 0, sizeof(*lcd));
  lcd->hi2c = hi2c;
  lcd->addr7 = addr7;
  lcd->backlight = 1;
  lcd->map = map;

  pins = (map == LCD_MAP_A) ? mapA : mapB;

  HAL_Delay(50);

  // Init 4-bit sequence
  write4bits(lcd, 0x03, 0); HAL_Delay(5);
  write4bits(lcd, 0x03, 0); HAL_Delay(5);
  write4bits(lcd, 0x03, 0); HAL_Delay(5);
  write4bits(lcd, 0x02, 0); HAL_Delay(5);

  lcd_cmd(lcd, LCD_FUNCTION_4B);
  lcd_cmd(lcd, LCD_DISPLAY_ON);
  lcd_cmd(lcd, LCD_CLEAR);
  HAL_Delay(5);
  lcd_cmd(lcd, LCD_ENTRY_MODE);
}

void LCD_Clear(LCD_PCF8574 *lcd) {
  lcd_cmd(lcd, LCD_CLEAR);
  HAL_Delay(5);
}

void LCD_SetCursor(LCD_PCF8574 *lcd, uint8_t row, uint8_t col) {
  static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
  if (row > 3) row = 3;
  lcd_cmd(lcd, LCD_SET_DDRAM | (row_offsets[row] + col));
}

void LCD_Print(LCD_PCF8574 *lcd, const char *s) {
  while (*s) lcd_data(lcd, (uint8_t)*s++);
}

void LCD_PrintLine(LCD_PCF8574 *lcd, uint8_t row, const char *text) {
  LCD_SetCursor(lcd, row, 0);
  for (int i = 0; i < 20; i++) lcd_data(lcd, ' ');
  LCD_SetCursor(lcd, row, 0);
  for (int i = 0; text[i] && i < 20; i++) lcd_data(lcd, (uint8_t)text[i]);
}
