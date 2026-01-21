/*
 * trivial_questions.c
 *
 *  Created on: Jan 16, 2026
 *      Author: Marta
 */

#include "trivial_questions.h"

static const Question questions[] = {
  {"GPIO es?", {" Protocolo","Pin E/S","Timer"," Memoria"}, 1, DIFF_EASY},
  {"¿UART es?", {" Síncrono","Asíncrono","Paralelo","PWM"}, 1, DIFF_EASY},
  {" ¿PWM genera?", {" Datos","Reloj","Señal","IRQ"}, 2, DIFF_EASY},
  {"¿ADC convierte?", {"D→A","A→D","PWM","SPI"}, 1, DIFF_EASY},
  {"¿SysTick es?", {"GPIO","Timer","ADC","UART"}, 1, DIFF_EASY},


  {"¿SPI usa reloj?", {"No","Sí","Opcional","PWM"}, 1, DIFF_MED},
  {"EXTI depende de", {"Puerto","Pin nº","Prioridad","NVIC"}, 1, DIFF_MED},
  {"ISR debe ser", {"Larga","Rápida"," Con delay","Bloqueante"}, 1, DIFF_MED},
  {"Variable en ISR", {"static","const","volatile","extern"}, 2, DIFF_MED},
  {"DMA sirve para", {"Retardos","GPIO","Datos rápidos","Debug"}, 2, DIFF_MED},
  {"EXTI3 comparte", {"PA3/PB3","PA3 solo","PB3 solo","No comparte"}, 0, DIFF_MED},
  {"Prio 0 es...", {"Max","Min","Media","Nula"}, 0, DIFF_MED},


  {"ISR debe ser", {"Corta","Larga","Con delay","Con printf"}, 0, DIFF_HARD},
  {"ADC 8bits da", {"0-255","0-1023","0-4095","0-511"}, 0, DIFF_HARD},
  {"ADC 12bits da", {"0-4095","0-255","0-1023","0-8191"}, 0, DIFF_HARD},
  {"Duty PWM es", {"CCR/(ARR+1)","ARR/CCR","PSC/ARR","CNT/ARR"}, 0, DIFF_HARD},
  {"Freq PWM es", {"fclk/(PSC+1)(ARR+1)","fclk*(PSC+1)(ARR+1)","ARR/PSC","PSC/ARR"}, 0, DIFF_HARD},
  {"SPI es...", {"Sincro","Async","1 hilo","Sin CS"}, 0, DIFF_HARD},
  {"I2C usa...", {"SDA+SCL","MOSI+MISO","TX+RX","CS+SCK"}, 0, DIFF_HARD},
  {"UART es...", {"Async","Sincro","2 relojes","Con CS"}, 0, DIFF_HARD},
  {"DMA evita...", {"CPU polling","GPIO","PWM","NVIC"}, 0, DIFF_HARD},
  {"DMA half IRQ", {"Mitad buf","1 muestra","Fin CPU","Start ADC"}, 0, DIFF_HARD},
  {"NVIC guarda...", {"Prioridades","PWM","ADC","GPIO"}, 0, DIFF_HARD},
  {"volatile sirve", {"Evita optim","Acelera CPU","Aumenta bits","Cambia ARR"}, 0, DIFF_HARD},

};

const Question* Questions_GetAll(uint32_t *count) {
  *count = (uint32_t)(sizeof(questions)/sizeof(questions[0]));
  return questions;
}
