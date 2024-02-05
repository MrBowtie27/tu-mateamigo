#include "sapi.h"        // <= Biblioteca sAPI
#include "ds18b20.h"

#define CalentadorAgua GPIO1
#define ElectroValvula GPIO3
#define SensorTemperatura GPIO5

#define ENCENDER_CALENTADOR gpioWrite(CalentadorAgua, LOW)
#define APAGAR_CALENTADOR gpioWrite(CalentadorAgua, HIGH)
#define ENCENDER_BOMBA gpioWrite(ElectroValvula,LOW)
#define APAGAR_BOMBA gpioWrite(ElectroValvula,HIGH)


typedef enum{Default,Calentando,Deteccion,Sirviendo,Retiro,Configuracion} state;
   state estado;

float temperatura = 0;

void Iniciar_MEF(){
   estado=Default;
}

void Pin_Init(){
   gpioInit(ElectroValvula,GPIO_OUTPUT);
   gpioInit(CalentadorAgua,GPIO_OUTPUT);
}

float distanceInCms;
uint8_t DetectoMate=0, ConfirmoDeteccion=0, DetectoRetiro=0, DetectoBotonConfig=0, DetectoConfirmacion=0;
uint8_t NivelAlto=1, NivelBajo=0, NivelMedio=0;

void ActualizarMEF(void)
{
   switch(estado){

      case Default:
         
         if(DetectoBotonConfig)
            { estado=Configuracion;}

         if(temperatura < 30) {
            ENCENDER_CALENTADOR;
            estado = Calentando;
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
         if(temperatura > 30) {
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
         if(NivelBajo) {delay(1000);}
         if(NivelMedio) {delay(1000);}
         if(NivelAlto) {delay(1000);}
         APAGAR_BOMBA;
         estado=Retiro;
      break;

      case Retiro:
         distanceInCms = ultrasonicSensorGetDistance(ULTRASONIC_SENSOR_0, CM);
         printf("Distance (Retiro): %.2f mm\r\n", distanceInCms*10);
         if (distanceInCms*10 > 40) //Si no detecto el mate cambio de estado
             DetectoRetiro = 1;
         if(DetectoRetiro)
         {  DetectoRetiro = 0;estado=Default;}
      break;

      case Configuracion:
         if(DetectoConfirmacion)
         { estado=Default;} 
      break;

   }
}

int main( void )
{
   boardConfig();
   uartConfig( UART_USB, 115200 ); // Inicializar periferico UART_USB
   initTemperatureSensor();
   printf("Antes\r\n");
   delay(2000);
   ultrasonicSensorConfig( ULTRASONIC_SENSOR_0, ULTRASONIC_SENSOR_ENABLE );
   printf("Despues\r\n");
   delay(100);
   Iniciar_MEF();
   Pin_Init();
   APAGAR_CALENTADOR;

   // ---------- REPETIR POR SIEMPRE --------------------------
   while(1) {
      temperatura = leerTemperatura();
      printf("Temperatura: %.2f \r\n", temperatura);
      delay(2000);
      ActualizarMEF();
   }

   // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
   // directamenteno sobre un microcontroladore y no es llamado por ningun
   // Sistema Operativo, como en el caso de un programa para PC.
   return 0;
}