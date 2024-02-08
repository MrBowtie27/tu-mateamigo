#include <HardwareSerial.h>


//---------------------VARIABLES GLOBALES-------------------------
#define ESTADO_LISTO 0
#define ESTADO_CALENTANDO 1

int contconexion = 0;

String header; // Variable para guardar el HTTP request
HardwareSerial UART_CIAA(2);


const int salida = 2;
unsigned int nivel = 1, nivel_CIAA;
unsigned int temperatura = 80, temperatura_CIAA;
unsigned int estado_CIAA = ESTADO_LISTO;


//------------------------ FUNCTIONS ------------------------
#define SET_TEMPERATURA 0
#define SET_NIVEL 1

void sendCommand(unsigned char command, unsigned char data) {
  unsigned char msg = (command << 7) | (data & 0x7F);
  Serial.println("Mandando: " + String((int) msg));
  UART_CIAA.write(msg);
}

String getEstado(unsigned int estado) {
  switch(estado) {
    case ESTADO_LISTO:
      return "Listo";
      break;
    case ESTADO_CALENTANDO:
      return "Calentando";
      break;
    default:
      return "ERROR";
  }
}

//---------------------------TASKS--------------------------------
// Tarea que periodicamente chequea que la placa le haya mandado sus mediciones
// se utiliza este metodo ya que no es posible utilizar interrupciones de UART
// en Arduino IDE
void getData(void* param) {
  while(1) {
  Serial.write("Recibiendo cositas\r\n");
    while (UART_CIAA.available()) {
      char data = UART_CIAA.read();
      if (data == 0xFF) {
        while(!UART_CIAA.available());
        temperatura_CIAA = UART_CIAA.read();
        Serial.println("Temperatura placa: " + String(temperatura_CIAA));
        while(!UART_CIAA.available());
        nivel_CIAA = UART_CIAA.read();
        Serial.println("Vivel placa: " + String(nivel_CIAA));
        while(!UART_CIAA.available());
        estado_CIAA = UART_CIAA.read();
        Serial.println("Estado placa " + String(estado_CIAA) + " " + getEstado(estado_CIAA));
      }
    }

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

// Tarea que eperiodicamente manda el nivel y la temperatura objetivo 
// para que la placa y el esp esten sincronizados
void sendData(void* param) {
  while(1) {
    Serial.write("mandando cositas\r\n");
    sendCommand(SET_TEMPERATURA, temperatura); // piso de 40 grados
    sendCommand(SET_NIVEL, nivel);
    vTaskDelay(pdMS_TO_TICKS(2500));
  }
}

//---------------------------SETUP--------------------------------
void setup() {
  Serial.begin(115200);
  UART_CIAA.begin(9600, SERIAL_8N1, 16, 17); // Setup de conexion con EDU-CIAA
  Serial.println("");

  // Crear tasks de UART
  xTaskCreate(getData, "getData", 1000, NULL, 1, NULL);
  //xTaskCreate(sendData, "wait", 1000, NULL, 2, NULL);
  //vTaskStartScheduler();
}

//----------------------------LOOP----------------------------------
void loop(){
  delay(3000);
}