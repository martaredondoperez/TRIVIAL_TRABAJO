/*
 * ui.c
 *
 *  Created on: Dec 15, 2025
 *      Author: Marta
 */


/**
 * @file    ui.c
 * @brief   Implementación de la lógica de la Interfaz de Usuario.
 * Utiliza las funciones de bajo nivel de tft.h para dibujar los elementos del juego.
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>    // Para usar snprintf
#include "ui.h"       // Prototipos de alto nivel (lo que estamos implementando)
#include "tft.h"      // Funciones de dibujo de bajo nivel (DrawPixel, DrawString, etc.)
#include "trivia_db.h" // Necesario para la estructura Question y const char*

/* Private defines -----------------------------------------------------------*/

#define FONT_HEIGHT 10    // Altura de los caracteres en píxeles (Ejemplo)
#define FONT_WIDTH  8     // Ancho de los caracteres en píxeles (Ejemplo)
#define MARGIN_X    10    // Margen izquierdo para el texto
#define OPTION_GAP  20    // Espacio vertical entre opciones

/* Private function prototypes -----------------------------------------------*/

// Función auxiliar para dibujar una sola línea de texto de forma limpia
static void UI_DrawLine(uint16_t x, uint16_t y, const char *str, uint16_t fg_color, uint16_t bg_color, bool selected);

/* Function implementations --------------------------------------------------*/

/**
 * @brief Inicializa el módulo de la Interfaz de Usuario.
 * Debe inicializar el hardware de la pantalla TFT.
 */
void UI_Init(void)
{
    // Llama a la inicialización del hardware de la pantalla.
    TFT_Init();
    TFT_ClearScreen(TFT_COLOR_BLACK);
}

/**
 * @brief Dibuja la pregunta actual y sus opciones en la pantalla.
 */
void UI_DrawQuestion(const Question* q_data, uint8_t selection)
{
    uint16_t y_start = 30; // Posición Y de inicio de la pregunta

    // 1. Limpiar el área de la pregunta/opciones
    TFT_ClearScreen(TFT_COLOR_BLACK);

    // 2. Dibujar el texto de la Pregunta
    // Usamos el color blanco para la pregunta
    TFT_DrawString(MARGIN_X, y_start, q_data->q, TFT_COLOR_WHITE, TFT_COLOR_BLACK);

    y_start += FONT_HEIGHT + 20; // Saltar línea para opciones

    // 3. Dibujar las 4 opciones
    for(uint8_t i = 0; i < 4; i++)
    {
        uint16_t current_y = y_start + (i * OPTION_GAP);

        // Define los colores según si la opción está seleccionada
        uint16_t fg = (i == selection) ? TFT_COLOR_BLACK : TFT_COLOR_WHITE; // Texto: Negro si seleccionado
        uint16_t bg = (i == selection) ? TFT_COLOR_YELLOW : TFT_COLOR_BLACK; // Fondo: Amarillo si seleccionado

        // Formato: "A. Respuesta X"
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "%c. %s", 'A' + i, q_data->a[i]);

        // Dibujar la línea de texto
        TFT_DrawString(MARGIN_X + 10, current_y, buffer, fg, bg);
    }
}

/**
 * @brief Actualiza y dibuja la puntuación en un área de la pantalla.
 */
void UI_DrawScore(uint32_t score)
{
    char score_str[20];

    // Formato: "PUNTOS: 12"
    snprintf(score_str, sizeof(score_str), "PUNTOS: %lu", score);

    // Asume que la puntuación va en la esquina superior derecha (ejemplo: x=180, y=10)
    uint16_t x_pos = 180;
    uint16_t y_pos = 10;

    // Dibujar la puntuación (usamos un fondo del color del área de la puntuación si es una actualización)
    TFT_DrawString(x_pos, y_pos, score_str, TFT_COLOR_CYAN, TFT_COLOR_BLACK);
}

// Implementación de la función auxiliar (solo para mostrar cómo sería la estructura)
// (Nota: TFT_DrawString probablemente llama a TFT_DrawChar internamente)
/*
static void UI_DrawLine(uint16_t x, uint16_t y, const char *str, uint16_t fg_color, uint16_t bg_color, bool selected)
{
    // ... Implementación que llama a TFT_DrawString o TFT_DrawChar en bucle ...
}
*/
