/*
 * tft.h
 *
 *  Created on: Dec 15, 2025
 *      Author: Marta
 */

#ifndef INC_TFT_H_
#define INC_TFT_H_

#include <stdint.h>

// ===================================================================
// 1. Definiciones de Colores (Formato RGB565)
// ===================================================================

// Estos son valores hexadecimales estándar que representan colores comunes en el formato de 16 bits (5 bits para Rojo, 6 para Verde, 5 para Azul)
#define TFT_COLOR_BLACK   0x0000
#define TFT_COLOR_WHITE   0xFFFF
#define TFT_COLOR_RED     0xF800
#define TFT_COLOR_GREEN   0x07E0
#define TFT_COLOR_BLUE    0x001F
#define TFT_COLOR_YELLOW  0xFFE0
#define TFT_COLOR_CYAN    0x07FF
#define TFT_COLOR_MAGENTA 0xF81F

// ===================================================================
// 2. Prototipos de Funciones de Hardware
// ===================================================================

/**
 * @brief Inicializa el hardware de la pantalla TFT (SPI/paralelo y el controlador).
 */
void TFT_Init(void);

/**
 * @brief Dibuja un píxel en una coordenada específica.
 * @param x Coordenada X.
 * @param y Coordenada Y.
 * @param color Color en formato RGB565.
 */
void TFT_DrawPixel(uint16_t x, uint16_t y, uint16_t color);

/**
 * @brief Borra toda la pantalla con un color uniforme.
 * @param color Color para rellenar la pantalla.
 */
void TFT_ClearScreen(uint16_t color);

/**
 * @brief Dibuja un carácter en una posición y con un color específico.
 * Esta es una función de dibujo básica usada por la capa UI.
 * @param x Coordenada X de inicio.
 * @param y Coordenada Y de inicio.
 * @param c El carácter a dibujar.
 * @param fg_color Color del primer plano (el carácter).
 * @param bg_color Color del fondo.
 */
void TFT_DrawChar(uint16_t x, uint16_t y, char c, uint16_t fg_color, uint16_t bg_color);


// Funciones para escribir cadenas completas (a menudo implementadas en la capa UI, pero útil tenerlas aquí si el hardware lo soporta mejor)
void TFT_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t fg_color, uint16_t bg_color);


#endif /* INC_TFT_H_ */
