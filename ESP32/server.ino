#include <WiFi.h>
#include <HardwareSerial.h>

//------------------Servidor Web en puerto 80---------------------

WiFiServer server(80);

//---------------------Credenciales de WiFi-----------------------

const char* ssid     = "Personal-B47-2.4GHz";
const char* password = "01442842419";

//---------------------VARIABLES GLOBALES-------------------------
#define ESTADO_LISTO 0
#define ESTADO_CALENTANDO 1
#define ESTADO_DETECCION 2
#define ESTADO_SIRVIENDO 3
#define ESTADO_RETIRO 4

int contconexion = 0;

String header; // Variable para guardar el HTTP request
HardwareSerial UART_CIAA(2);

typedef enum {Index, Config, Home} view_enum;
view_enum pagina_i;


const int salida = 2;
unsigned int nivel = 1, nivel_CIAA;
unsigned int temperatura = 35, temperatura_CIAA;
unsigned int estado_CIAA = ESTADO_LISTO;
int cargas;

//------------------------CODIGO HTML------------------------------
String ind = "<!DOCTYPE html>"
"<html>"
"<head>"
"<meta charset='utf-8' />"
"<title>Tu MateAmigo</title>"
"</head>"
"<body>"
"<center>"
"<p><a href='/home'><button style='height:200px;width:500px;background-color:rgb(51, 196, 172);border: 3px solid black;font-size: x-large;'>HOME</button></a></p>"
"<p><a href='/config'><button style='height:200px;width:500px;background-color:rgb(98, 168, 189);border: 3px solid black;font-size: x-large;'>CONFIGURACION</button></a></p>"
"</center>"
"</body>"
"</html>";

String home_1 = "<!DOCTYPE html>"
"<html lang='es'>"
"<head>"
"    <meta charset='UTF-8'>"
//recargar la pagina cada 5 segundos por el feedback
" <meta http-equiv='refresh' content='0.5'> "
"    <meta name='viewport' content='width=device-width, initial-scale=1.0'>"
"    <title>Tu MateAmigo</title>"
"    <link rel='stylesheet' href='https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@20..48,100..700,0..1,-50..200' />"
"    <style>"
"        body {"
"            font-family: Arial, sans-serif;"
"            margin: 0;"
"            padding: 0;"
"        }"
"        .container {"
"            max-width: 500px;"
"            margin: 20px auto;"
"            padding: 20px;"
"            border: 1px solid #ccc;"
"            position: relative;"
"        }"
"        .volver-btn {"
"            position: absolute;"
"            top: 10px;"
"            right: 10px;"
"        }"
"        .row {"
"            display: flex;"
"            margin-bottom: 10px;"
"        }"
"        .square {"
"            width: 40px;"
"            height: 40px;"
"            border: 1px solid #000;"
"            box-sizing: border-box;"
"            margin-left: 10px;"
"        }"
""
"        #miCajaTexto {"
"            font-size: 50px; /* Tamaño de la fuente */"
"            border: 1px solid #000; /* Borde para resaltar la caja de texto */"
"            padding: 10px; /* Espaciado interno para mejorar la apariencia */"
"            width: 300px; /* Ancho de la caja de texto */"
"            min-height: 100px; /* Altura mínima de la caja de texto */"
"            margin-left: auto; /* Centrar horizontalmente */"
"            margin-right: auto; /* Centrar horizontalmente */"
"        }"
"    </style>"
"</head>"
"<body>"
""
"<div class='container'>"
"  <p><a href='/index'><button class='volver-btn'>Volver</button></a></p>  "
"    <br>"
"    <br>" 
"    <div class='row'>"
"        <div><svg xmlns='http://www.w3.org/2000/svg' height='24' viewBox='0 -960 960 960' width='24'><path d='M480-80q-83 0-141.5-58.5T280-280q0-48 21-89.5t59-70.5v-320q0-50 35-85t85-35q50 0 85 35t35 85v320q38 29 59 70.5t21 89.5q0 83-58.5 141.5T480-80Zm-40-440h80v-40h-40v-40h40v-80h-40v-40h40v-40q0-17-11.5-28.5T480-800q-17 0-28.5 11.5T440-760v240Z'/></svg></div>"
"        <div>Temperatura actual del agua</div>";
String home_2=        "<div class='square'>80°C</div>"
"    </div>"
"    <div class='row'>"
"        <div><svg xmlns='http://www.w3.org/2000/svg' height='24' viewBox='0 -960 960 960' width='24'><path d='M419-80q-28 0-52.5-12T325-126L107-403l19-20q20-21 48-25t52 11l74 45v-328q0-17 11.5-28.5T340-760q17 0 29 11.5t12 28.5v472l-97-60 104 133q6 7 14 11t17 4h221q33 0 56.5-23.5T720-240v-160q0-17-11.5-28.5T680-440H461v-80h219q50 0 85 35t35 85v160q0 66-47 113T640-80H419ZM167-620q-13-22-20-47.5t-7-52.5q0-83 58.5-141.5T340-920q83 0 141.5 58.5T540-720q0 27-7 52.5T513-620l-69-40q8-14 12-28.5t4-31.5q0-50-35-85t-85-35q-50 0-85 35t-35 85q0 17 4 31.5t12 28.5l-69 40Zm335 280Zm-40 80q-17 0-28.5-11.5T280-120v-640q0-17 11.5-28.5T320-800h80v-80h160v80h80q17 0 28.5 11.5T680-760v280q-21 0-41 3.5T600-466v-254H360v560h94q8 23 19.5 43T501-80H320Z'/></svg></div>"
"        <div>Nivel seleccionado</div>"
"        <div class='square'>1</div>"
"    </div>"
""
"    <div class='row'>"
"        <div><svg xmlns='http://www.w3.org/2000/svg' height='24' viewBox='0 -960 960 960' width='24'><path d='M660-80v-120H560l140-200v120h100L660-80Zm-300-80Zm-40 80q-17 0-28.5-11.5T280-120v-640q0-17 11.5-28.5T320-800h80v-80h160v80h80q17 0 28.5 11.5T680-760v280q-21 0-41 3.5T600-466v-254H360v560h94q8 23 19.5 43T501-80H320Z'/></svg></div>"
"        <div>Cargas restantes</div>"
"        <div class='square'>3</div>"
"    </div>"
"    <div id='miCajaTexto' contenteditable='true'>LISTO</div>"
"</div>"
"</body>"
"</html>";

String config = "<!DOCTYPE html>"
"<html>"
"<head>"
"<meta charset='utf-8' />"
"<title>Tu MateAmigo</title>"
"<style>"
"    body {"
"        font-family: Arial, sans-serif;"
"        margin: 0;"
"        padding: 0;"
"    }"
""
"    .container {"
"        max-width: 500px;"
"        margin: 20px auto;"
"        padding: 20px;"
"        border: 1px solid #ccc;"
"        position: relative;"
"    }"
""
"    .volver-btn {"
"        position: absolute;"
"        top: 10px;"
"        right: 10px;"
"    }"
""
"    .row {"
"        display: flex;"
"        margin-bottom: 10px;"
"    }"
""
"    .square {"
"        width: 40px;"
"        height: 40px;"
"        border: 1px solid #000;"
"        box-sizing: border-box;"
"        margin-left: 10px;"
"    }"
"    .form-check-inline {"
"      display: inline-block;"
"      margin-right: 10px; /* Ajusta el margen entre los checkboxes */"
"    }"
""
"    /* Estilo opcional para mejorar la apariencia del checkbox */"
"    .form-check-inline input[type='checkbox'] {"
"      margin-right: 5px; /* Ajusta el margen entre el checkbox y el texto */"
"    }"
"</style>"
"</head>"
"<body>"
"    <script>"
"        function enviarParametros() {\n"
"            var valorNumero = document.getElementById('numeroInput').value;\n"
"            var radios = document.getElementsByName('nivel');\n"
"            var nivel = null;"
"             if(valorNumero>80 || valorNumero<30) {"
"             alert('Esa temperatura no esta permitida');"
"             return false;}"
"            for (var i = 0; i < radios.length; i++) {"
"            if (radios[i].checked) {"
"                nivel = radios[i].id;"
"                break;"
"            }"
"            }"
"            // Construye la URL con los parámetros \n"
"            if (nivel != null) {"
"             var url = '/index?' + "
"                        'numeroInput=' + valorNumero + "
"                        '&nivel=' + nivel ;\n"
"            } else {"
"             var url = '/index?' + "
"                        'numeroInput=' + valorNumero;\n"
"            }"
"            window.location.href = url;\n"
"          }\n"
""
"      </script>"
"<div class='container'>"
"<p><a onclick='enviarParametros()'><button class='volver-btn'>Volver</button></a></p>"
"<br>"
"<br>" 
"<div class='row'>"
"    <div><svg xmlns='http://www.w3.org/2000/svg' height='24' viewBox='0 -960 960 960' width='24'><path d='M480-80q-83 0-141.5-58.5T280-280q0-48 21-89.5t59-70.5v-320q0-50 35-85t85-35q50 0 85 35t35 85v320q38 29 59 70.5t21 89.5q0 83-58.5 141.5T480-80Zm-40-440h80v-40h-40v-40h40v-80h-40v-40h40v-40q0-17-11.5-28.5T480-800q-17 0-28.5 11.5T440-760v240Z'/></svg></div>"
"    <div>Temperatura maxima del agua</div>"
"    <input class='square' type='number' id='numeroInput' name='numeroInput' value='0' min='1'>"
"</div>"
""
"<div class='row'>"
"    <div><svg xmlns='http://www.w3.org/2000/svg' height='24' viewBox='0 -960 960 960' width='24'><path d='M419-80q-28 0-52.5-12T325-126L107-403l19-20q20-21 48-25t52 11l74 45v-328q0-17 11.5-28.5T340-760q17 0 29 11.5t12 28.5v472l-97-60 104 133q6 7 14 11t17 4h221q33 0 56.5-23.5T720-240v-160q0-17-11.5-28.5T680-440H461v-80h219q50 0 85 35t35 85v160q0 66-47 113T640-80H419ZM167-620q-13-22-20-47.5t-7-52.5q0-83 58.5-141.5T340-920q83 0 141.5 58.5T540-720q0 27-7 52.5T513-620l-69-40q8-14 12-28.5t4-31.5q0-50-35-85t-85-35q-50 0-85 35t-35 85q0 17 4 31.5t12 28.5l-69 40Zm335 280Z'/></svg></div>"
"    <div>Seleccione el nivel deseado</div>"
"</div>"
"<div class='row'>"
"    <div>"
"        <label class='form-check-inline'>"
"           <input type='radio' name='nivel' id='1'>Nivel 1"
"        </label>"
"       "
"       <label class='form-check-inline'>"
"           <input type='radio' name='nivel' id='2'>Nivel 2"
"       </label>"
"       "
"       <label class='form-check-inline'>"
"          <input type='radio' name='nivel' id='3'>Nivel 3"
"        </label>"
"    </div>"
"</div>"
""
"</div>"
"</body>"
"</html>";
String pagina = ind;


//------------------------ FUNCTIONS ------------------------
#define SET_TEMPERATURA 0
#define SET_NIVEL 1

void sendCommand(unsigned char command, unsigned char data) {
  unsigned char msg = (command << 7) | (data & 0x7F);
  //Serial.println("Mandando: " + String((int) msg));
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
    case ESTADO_DETECCION:
      return "Detectando";
      break;
    case ESTADO_SIRVIENDO:
      return "Sirviendo";
      break;
    case ESTADO_RETIRO:
      return "Retirar mate";
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
  //Serial.write("Recibiendo cositas\r\n");
    while (UART_CIAA.available()) {
      char data = UART_CIAA.read();
      if (data == 0xFF) {
        while(!UART_CIAA.available());
        temperatura_CIAA = UART_CIAA.read();
        //Serial.println("Temperatura placa: " + String(temperatura_CIAA));
        while(!UART_CIAA.available());
        nivel_CIAA = UART_CIAA.read();
        //Serial.println("Nivel placa: " + String(nivel_CIAA));
        while(!UART_CIAA.available());
        estado_CIAA = UART_CIAA.read();
        //Serial.println("Estado placa " + String(estado_CIAA) + " " + getEstado(estado_CIAA));
      }
    }

    vTaskDelay(pdMS_TO_TICKS(150));
  }
}

// Tarea que eperiodicamente manda el nivel y la temperatura objetivo 
// para que la placa y el esp esten sincronizados
void sendData(void* param) {
  while(1) {
    //Serial.write("mandando cositas\r\n");
    sendCommand(SET_TEMPERATURA, temperatura);
    sendCommand(SET_NIVEL, nivel);
    vTaskDelay(pdMS_TO_TICKS(2500));
  }
}

//---------------------------SETUP--------------------------------
void setup() {
  //Serial.begin(115200);
  UART_CIAA.begin(9600, SERIAL_8N1, 16, 17); // Setup de conexion con EDU-CIAA
  //Serial.println("");
  
  pinMode(salida, OUTPUT); 
  digitalWrite(salida, LOW);

  // Conexión WIFI
  WiFi.begin(ssid, password);
  //Cuenta hasta 50 si no se puede conectar lo cancela
  while (WiFi.status() != WL_CONNECTED and contconexion <50) { 
    ++contconexion;
    delay(500);
    //Serial.print(".");
  }
  if (contconexion <50) {
      //para usar con ip fija
      IPAddress ip(192,168,0,222); 
      IPAddress gateway(192,168,0,1); 
      IPAddress subnet(255,255,255,0); 
      WiFi.config(ip, gateway, subnet); 
      
      //Serial.println("");
      //Serial.println("WiFi conectado");
      //Serial.println(WiFi.localIP());
      server.begin(); // iniciamos el servidor
  }
  else { 
      //Serial.println("");
      //Serial.println("Error de conexion");
  }

  // Crear tasks de UART
  xTaskCreate(getData, "getData", 1000, NULL, 1, NULL);
  xTaskCreate(sendData, "wait", 1000, NULL, 2, NULL);
  vTaskStartScheduler();
}

//----------------------------LOOP----------------------------------

void loop(){
  WiFiClient client = server.available();   // Escucha a los clientes entrantes


  if (client) {                             // Si se conecta un nuevo cliente
    //Serial.println("New Client.");          // 
    String currentLine = "";                //
    while (client.connected()) {            // loop mientras el cliente está conectado
      if (client.available()) {             // si hay bytes para leer desde el cliente
        char c = client.read();             // lee un byte
        //Serial.write(c);                    // imprime ese byte en el monitor serial
        header += c;
        if (c == '\n') {                    // si el byte es un caracter de salto de linea
          // si la nueva linea está en blanco significa que es el fin del 
          // HTTP request del cliente, entonces respondemos:
          if (currentLine.length() == 0) {
            //Serial.println(header);
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            if (header.indexOf("GET /home") >= 0) {
              home_2 =        "<div class='square'>" + String(temperatura_CIAA) + "°C</div>"
                                "    </div>"
                                "    <div class='row'>"
                                "        <div><svg xmlns='http://www.w3.org/2000/svg' height='24' viewBox='0 -960 960 960' width='24'><path d='M660-80v-120H560l140-200v120h100L660-80Zm-300-80Zm-40 80q-17 0-28.5-11.5T280-120v-640q0-17 11.5-28.5T320-800h80v-80h160v80h80q17 0 28.5 11.5T680-760v280q-21 0-41 3.5T600-466v-254H360v560h94q8 23 19.5 43T501-80H320Z'/></svg></div>"
                                "        <div>Temperatura objetivo:</div>"
                                "        <div class='square'>" + String(temperatura) + "°C</div>"
                                "    </div>"
                                "    <div class='row'>"
                                "        <div><svg xmlns='http://www.w3.org/2000/svg' height='24' viewBox='0 -960 960 960' width='24'><path d='M419-80q-28 0-52.5-12T325-126L107-403l19-20q20-21 48-25t52 11l74 45v-328q0-17 11.5-28.5T340-760q17 0 29 11.5t12 28.5v472l-97-60 104 133q6 7 14 11t17 4h221q33 0 56.5-23.5T720-240v-160q0-17-11.5-28.5T680-440H461v-80h219q50 0 85 35t35 85v160q0 66-47 113T640-80H419ZM167-620q-13-22-20-47.5t-7-52.5q0-83 58.5-141.5T340-920q83 0 141.5 58.5T540-720q0 27-7 52.5T513-620l-69-40q8-14 12-28.5t4-31.5q0-50-35-85t-85-35q-50 0-85 35t-35 85q0 17 4 31.5t12 28.5l-69 40Zm335 280Zm-40 80q-17 0-28.5-11.5T280-120v-640q0-17 11.5-28.5T320-800h80v-80h160v80h80q17 0 28.5 11.5T680-760v280q-21 0-41 3.5T600-466v-254H360v560h94q8 23 19.5 43T501-80H320Z'/></svg></div>"
                                "        <div>Nivel seleccionado</div>"
                                "        <div class='square'>" + String(nivel_CIAA) + "</div>"
                                "    </div>"
                                "    <div id='miCajaTexto' contenteditable='true'>" + getEstado(estado_CIAA)+ "</div>"
                                "</div>"
                                "</body>"
                                "</html>";
              pagina = home_1 + home_2;
              pagina_i = Home;
            } else if (header.indexOf("GET /config") >= 0) {
              pagina = config;
              pagina_i = Config;
            } else if (header.indexOf("GET /index") >= 0) {
              if (pagina_i == Config) {
                // interpretar parámetos
                temperatura = header.substring(23,25).toInt();
                //Serial.println("Temperatura cambiada a " + String(temperatura));
                String n;
                uint32_t indexNivel = header.indexOf('&nivel=');
                if (indexNivel > 0) {
                  n = header.substring(indexNivel, header.indexOf("HTTP")-1);
                  //Serial.println(temperatura);
                  nivel = n.toInt();
                  //Serial.println("Nivel cambiado a " + String(nivel));
                }
              }
              pagina = ind;
              pagina_i = Index;

            }
          }
            
            // Muestra la página web
            client.println(pagina);
            
            // la respuesta HTTP temina con una linea en blanco
            client.println();
            break;
          } else if (c != '\r') {  // si C es distinto al caracter de retorno de carro
            currentLine += c;      // lo agrega al final de currentLine
          } 
          else { // si tenemos una nueva linea limpiamos currentLine
            currentLine = "";
          } 
      }
    }
    // Limpiamos la variable header
    header = "";
    // Cerramos la conexión
    client.stop();
    //Serial.println("Client disconnected.");
    //Serial.println("");
  }
}
