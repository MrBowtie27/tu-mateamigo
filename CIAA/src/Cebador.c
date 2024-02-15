#include "sapi.h"        // <= Biblioteca sAPI
#include "ds18b20.h"
#include "esp32_uart.h"
#include "types.h"

#define CalentadorAgua GPIO1
#define ElectroValvula GPIO3
#define SensorTemperatura GPIO5

#define ENCENDER_CALENTADOR gpioWrite(CalentadorAgua, LOW)
#define APAGAR_CALENTADOR gpioWrite(CalentadorAgua, HIGH)
#define ENCENDER_BOMBA gpioWrite(ElectroValvula,LOW)
#define APAGAR_BOMBA gpioWrite(ElectroValvula,HIGH)

#define TEMP_DIFF_THRESHOLD_HEAT 0
#define TEMP_DIFF_THRESHOLD_MANTAIN 0

state estado;

float temperatura = 0, temperatura_objetivo = 30;
Nivel nivel;

float distanceInCms;
uint8_t DetectoMate = 0, ConfirmoDeteccion = 0, DetectoRetiro = 0;

void Iniciar_MEF(){
   estado = Default;
}

void actualizarCalentamiento() {
   static uint8_t calentandoBool = 0;
   static uint8_t prevenirHisteresis = 0;
   static uint8_t contHisteresis = 0;
   // Medir temperatura
   temperatura = leerTemperatura();
   printf("Temperatura: %.2f \r\n", temperatura);
   
   if (prevenirHisteresis && contHisteresis < 40) {
      contHisteresis++;
   } else {
      if (estado != Calentando) {
         // Activar  o desactivar calentador segun corresponda
         if (calentandoBool == 0){
            if (temperatura < temperatura_objetivo - TEMP_DIFF_THRESHOLD_MANTAIN) {
               calentandoBool = 1;
               ENCENDER_CALENTADOR;
               prevenirHisteresis = 1;
               contHisteresis = 0;

            }
         }
         else {
            if (temperatura >= temperatura_objetivo) {
               calentandoBool = 0;
               APAGAR_CALENTADOR;
               prevenirHisteresis = 1;
               contHisteresis = 0;
            }
         }
      }
   }
}

void Pin_Init(){
   gpioInit(ElectroValvula, GPIO_OUTPUT);
   gpioInit(CalentadorAgua, GPIO_OUTPUT);
}

void timerInterrupt(void* params) {
   static volatile uint8_t cont = 0;
   
   // Actualizar sistema de calentamiento cada 2 interrupciones (100ms a tick de 50ms)
   if (cont%2 == 0) {
      actualizarCalentamiento();
   }

   // Actualizar sistema de calentamiento cada 3 interrupciones (150ms a tick de 50ms)
   if (cont%3 == 0) {
      sendDatos();
   }
   cont = (cont + 1)%200; // Resetear cada 200 interrupciones
}

void ActualizarMEF(void)
{
   switch(estado){

      case Default:

         if (temperatura < temperatura_objetivo - TEMP_DIFF_THRESHOLD_HEAT) {
            estado = Calentando;
            //ENCENDER_CALENTADOR;
            break;
         }

         distanceInCms = ultrasonicSensorGetDistance(ULTRASONIC_SENSOR_0, CM);
         printf("Distance (Default): %.2f mm\r\n", distanceInCms*10);
         if (distanceInCms*10 < 40) //Si detecto el mate cambio de estado
             DetectoMate = 1;
         if(DetectoMate)
            { DetectoMate = 0; estado=Deteccion;}
      break;

      case Calentando:
         printf("Calentando - ");
         if(temperatura >= temperatura_objetivo) {
            APAGAR_CALENTADOR;
            estado = Default;
         }
      break;

      case Deteccion:
         distanceInCms = ultrasonicSensorGetDistance(ULTRASONIC_SENSOR_0, CM);
         printf("Distance (Deteccion): %.2f mm\r\n", distanceInCms*10);
         if (distanceInCms*10 < 40) 
             ConfirmoDeteccion = 1;
         if(ConfirmoDeteccion)
            { ConfirmoDeteccion = 0; estado=Sirviendo;}
         else
            { estado=Default;}
            
      break;

      case Sirviendo:
         printf("Sirviendo\r\n");
         ENCENDER_BOMBA;
         switch(nivel) {
            case NivelBajo:
               delay(1000);
            break;
            case NivelMedio:
               delay(1000);
            break;
            case NivelAlto:
               delay(1000);
            break;
         }
         APAGAR_BOMBA;
         estado=Retiro;
      break;

      case Retiro:
         distanceInCms = ultrasonicSensorGetDistance(ULTRASONIC_SENSOR_0, CM);
         printf("Distance (Retiro): %.2f mm\r\n", distanceInCms*10);
         if (distanceInCms*10 > 40) //Si no detecto el mate cambio de estado
            DetectoRetiro = 1; // Sacar
         if(DetectoRetiro) { // Sacar
            DetectoRetiro = 0; // Sacar
            estado = Default;
            delay(500);
         }
      break;
   }
}

int main( void )
{
   boardConfig();
   uartConfig( UART_USB, 115200 ); // Inicializar periferico UART_USB

   // Inicializacion de sensores
   initTemperatureSensor();
   delay(2000);
   ultrasonicSensorConfig( ULTRASONIC_SENSOR_0, ULTRASONIC_SENSOR_ENABLE );
   configurarUART();

   // Configurar interrupcion de timer cada 50 ms
   tickConfig(50);
   tickCallbackSet( &timerInterrupt, NULL );

   delay(100);

   Iniciar_MEF();
   Pin_Init();
   APAGAR_CALENTADOR;
   APAGAR_BOMBA;

   // ---------- REPETIR POR SIEMPRE --------------------------
   while(1) {
      ActualizarMEF();
      delay(200);
   }

   // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
   // directamenteno sobre un microcontroladore y no es llamado por ningun
   // Sistema Operativo, como en el caso de un programa para PC.
   return 0;
}