// Inlcusiones

#include "ds18b20.h"         // <= Su propia cabecera

void printInBin(int8_t byte) {
   uint8_t i;
   for(i = 0; i < 8; i++) {
      printf("%d", (byte >> i) & 1);
   }
}

void initTemperatureSensor()
{
    Board_Init();
   gpioInit(GPIO3, GPIO_INPUT);
}

void delayUs(unsigned int n) {
   int count = 0;
   while (1){
          __asm__ volatile (
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
               "nop\n\t"
         );
      if (++count == n) {
         //gpioToggle(GPIO3); // Sacar;
         //count = 0; // Sacar
         break;
      }
   }
   
}

uint8_t detectar() {
   uint8_t flag = 0, timeout = 0;;
   // Pulso de reset
   gpioInit(GPIO3, GPIO_OUTPUT);
   gpioWrite(GPIO3, LOW);
   delayUs(DELAY_500US);
   
   // Pulso de presencia
   gpioConfig(GPIO3, GPIO_INPUT);
   while(gpioRead(GPIO3) == HIGH && timeout < 20) {
      delayUs(DELAY_5US);
      timeout++;
   }
   
   while(gpioRead(GPIO3) == LOW) {
      flag = 1;
   }
   
   
   /*if (flag)
      printf("Detectado!!!!\r\n");
   else
      printf(":( %d\r\n", timeout);*/
   return flag;
}

void write_bit(uint8_t bit) {
   if (bit&1) {
      // WRITE 1 SLOT
      gpioConfig(GPIO3, GPIO_OUTPUT);
      gpioWrite(GPIO3, LOW);
      delayUs(DELAY_5US);
      gpioConfig(GPIO3, GPIO_INPUT);
      delayUs(DELAY_80US);
   } else {
      // WRITE 0 SLOT
      gpioConfig(GPIO3, GPIO_OUTPUT);
      gpioWrite(GPIO3, LOW);
      delayUs(DELAY_80US);
      gpioConfig(GPIO3, GPIO_INPUT);
      delayUs(DELAY_5US);
   }
}

void write(uint8_t byte) {
   uint8_t mask;
   for(mask = 1; mask; mask<<=1) {
      //printf("Writing: %d\r\n", (byte&mask) ? 1 : 0);
      write_bit((byte&mask) ? 1 : 0);
   }
}

uint8_t read_bit() {
   uint8_t bit;
   
   gpioConfig(GPIO3, GPIO_OUTPUT);
   gpioWrite(GPIO3, LOW);
   delayUs(DELAY_2US);
   
   gpioConfig(GPIO3, GPIO_INPUT);
   delayUs(DELAY_5US);
   bit = gpioRead(GPIO3);
   
   delayUs(DELAY_80US);
   
   return bit;
}

uint8_t read() {
   uint8_t mask;
   uint8_t ret = 0;
   for(mask = 1; mask; mask<<=1){
      if(read_bit()) ret |= mask;
      //printf("Bit %d: %d\r\n", mask, ret);
   }
   
   return ret;
}

void read_bytes(int8_t *buff, uint8_t len) {
   uint8_t i;
   for(i = 0; i < len; i++){
      buff[i] = read();
      //printf("Byte %d: ", i);
      //printInBin(buff[i]);
      //printf("\r\n");
   }
}

float leerTemperatura() {
   uint8_t scratchpad[9];
   
   if(!detectar())
      return -1;
   write(0xCC); // Skip
   write(0x44); // Convert T
   
   // Wait for conversion
   while (gpioRead(GPIO3) == LOW) {
      //printf("Convirtiendo...\r\n");
   }
   
   if(!detectar())
      return -1;
   
   write(0xCC); // Skip
   write(0xBE); // Read scratchpad
   
   read_bytes(scratchpad, 9);
   
   if(!detectar())
      return -1;
   
   int16_t raw = (scratchpad[1] << 8) | scratchpad[0];
   
   return ( (float) raw / 16.0);
}