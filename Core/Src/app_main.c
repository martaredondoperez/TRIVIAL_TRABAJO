/**
 * @file    app_main.c
 * @brief   Implementación de las funciones principales de la aplicación.
 * Proporciona la interfaz entre el código de la aplicación (GAME)
 * y el hardware/sistema operativo (HAL).
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"      // Para incluir stm32f4xx_hal.h indirectamente
#include "app_main.h"  // Prototipos de las funciones APP
#include "game.h"      // Prototipos del módulo de juego (GAME_Init, GAME_Tick)

/* Private defines -----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

// Esta variable es global y definida en el HAL (stm32f4xx_hal.c).
// Representa el conteo de ticks (milisegundos) desde el inicio del sistema.
extern volatile uint32_t uwTick;

/* Function implementations --------------------------------------------------*/

/**
 * @brief Devuelve el número de milisegundos transcurridos desde el inicio del sistema.
 * @retval uint32_t Milisegundos transcurridos.
 */
uint32_t APP_Millis(void){
    // Usa la variable global del HAL
    return uwTick;
}

/**
 * @brief Inicializa la capa de la aplicación.
 * * * Esta función se llama una vez al inicio del programa.
 */
void APP_Init(void){
    // 1. Inicializa el módulo de juego (u otras configuraciones de aplicación)
    GAME_Init();

    // 2. Puedes añadir más inicializaciones de módulos aquí
    // Ejemplo: JOY_Init();
}

/**
 * @brief Bucle principal de la aplicación.
 * * * Esta función se llama repetidamente dentro del bucle while(1) de main().
 */
void APP_Loop(void){
    // 1. Llama a la función de "tick" del juego para procesar la lógica y actualizar.
    GAME_Tick();

    // 2. Puedes añadir otras tareas periódicas o de bajo nivel aquí.
    // Ejemplo: Procesamiento de sensores, comunicaciones, etc.
}
