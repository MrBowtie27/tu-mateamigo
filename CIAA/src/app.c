// Inlcusiones

#include "app.h"         // <= Su propia cabecera

int main(void)
{  
   boardConfig();
	initTemperatureSensor();

   float temp = 0;

	while (1)
	{
      temp = leerTemperatura();
      printf("Temperatura: %.2f\r\n", temp);
      delay(2000);
	}
}
