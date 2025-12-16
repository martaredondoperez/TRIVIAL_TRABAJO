/*
 * ui.h
 *
 *  Created on: Dec 15, 2025
 *      Author: Marta
 */

#ifndef INC_UI_H_
#define INC_UI_H_


#include <stdint.h>
#include "trivia_db.h" // Necesario para la estructura Question

/**
 * @brief Inicializa el módulo de la Interfaz de Usuario.
 * Debe llamar a la inicialización del hardware TFT.
 */
void UI_Init(void);

/**
 * @brief Dibuja la pregunta actual y sus opciones en la pantalla.
 * @param q_data Puntero a la estructura de la pregunta a dibujar.
 * @param selection El índice (0-3) de la respuesta que está actualmente seleccionada.
 * Esta opción se dibujará de forma resaltada.
 */
void UI_DrawQuestion(const Question* q_data, uint8_t selection);

/**
 * @brief Dibuja la puntuación actual en un área de la pantalla.
 * @param score La puntuación actual del jugador.
 */
void UI_DrawScore(uint32_t score);


// También podrías añadir una función para pantallas de feedback:
// void UI_DrawFeedback(bool correct);


#endif /* INC_UI_H_ */
