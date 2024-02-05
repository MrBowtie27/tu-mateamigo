#ifndef _DS18B20_H_
#define _DS18B20_H_

#include "sapi.h"        // <= Biblioteca sAPI
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------- CONSTANTES --------------------- */
#define DELAY_2US 2
#define DELAY_5US 15
#define DELAY_80US 320
#define DELAY_100US 400
#define DELAY_400US 1600
#define DELAY_500US 2000
#define DS18B20_PIN GPIO5

/* --------------------- PROTOTYPES --------------------- */

// Inicializacion de configuracion
void initTemperatureSensor();

// Retraso en microsegundos (solo funciona con constantes de precompilador)
void delayUs(unsigned);

// Deteccion de un sensor
uint8_t detectar();

// Escribir solo un bit en el bus
void write_bit(uint8_t);

// Escribir un byte entero en el bus
void write(uint8_t);

// Leer solo un bit del bus
uint8_t read_bit();

// Leer un byte entero del bus
uint8_t read();

// Leer varios bytes del bus
void read_bytes(int8_t*, uint8_t);

// Medir temperatura con el sensor
float leerTemperatura();


#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* _DS18B20_H_ */
