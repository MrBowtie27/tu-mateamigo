#include "esp32_uart.h"

void rxComando ( void *params )
{
   uint8_t msg = uartRxRead( UART_232 );
   printf( "Recibimos <<%d>> por UART\r\n", msg );
   
   uint8_t data = msg & 0b01111111;
   switch (msg >> 7) {
      case UART_COM_TEMP:
         temperatura_objetivo = (float) data;
         printf("Se cambio la temperatura a: %d\r\n", temperatura_objetivo);
      break;
      case UART_COM_NIVEL:
         nivel = (data == 0) ? NivelBajo: ( (data == 1)? NivelMedio : NivelAlto );
         printf("Se cambio el nivel a: %d\r\n", nivel);
      break;
   }
}

void configurarUART() {
   /* Inicializar la UART_232 junto con las interrupciones de Tx y Rx */
   uartConfig(UART_232, 9600);
   // Seteo un callback al evento de recepcion y habilito su interrupcion
   uartCallbackSet(UART_232, UART_RECEIVE, rxComando, NULL);
   // Habilito todas las interrupciones de UART_USB
   uartInterrupt(UART_232, true);
}

void sendDatos() {
   uartWriteByte(UART_232, 0xFF);
   uartWriteByte(UART_232, temperatura_objetivo);
   uartWriteByte(UART_232, nivel);
   uartWriteByte(UART_232, estado);
}
