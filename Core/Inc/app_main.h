/**
 * @file    app_main.h
 * @brief   Header file for the main Application module.
 * Contains prototypes for initialization, main loop, and utility functions.
 */

#ifndef APP_MAIN_H
#define APP_MAIN_H

// *******************************************************************
// * INCLUDES
// *******************************************************************

// Required for the uint32_t type definition used in APP_Millis
#include <stdint.h>

// *******************************************************************
// * EXPORTED FUNCTIONS PROTOTYPES
// *******************************************************************

/**
 * @brief Initializes all necessary application components.
 * * Should be called once at the start of the main program (e.g., in main.c, before the while(1) loop).
 */
void APP_Init(void);

/**
 * @brief Main application execution loop.
 * * Should be called repeatedly inside the main while(1) loop for continuous processing.
 */
void APP_Loop(void);

/**
 * @brief Returns the number of milliseconds elapsed since the application started.
 * * @return uint32_t Elapsed milliseconds.
 */
uint32_t APP_Millis(void);

#endif // APP_MAIN_H
