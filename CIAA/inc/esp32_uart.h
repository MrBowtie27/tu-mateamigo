#ifndef _ESP32_UART_H
#define _ESP32_UART_H

#include "sapi.h"
#include "types.h"
#define UART_COM_TEMP 0
#define UART_COM_NIVEL 1

#define ESTADO_LISTO 0
#define ESTADO_CALENTANDO 1


extern state estado;
extern float temperatura, temperatura_objetivo;
extern Nivel nivel;

#endif
