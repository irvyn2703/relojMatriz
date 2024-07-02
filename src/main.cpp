// Su ID de dispositivo es: 6657daf16e1af35935fe191d
// Clave de aplicación: 98655e65-011d-4373-8406-c52bd2132884
// App secreta: 4eeee22e-b262-463c-989f-bcf66d9adbd6-5400c5ee-3c29-457e-ba0b-8f967020a9a5

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SinricPro.h>
#include <SinricProLight.h>
#include <WebServer.h>
#include "FS.h"
#include "config.h"

#define PIN 19          // Pin digital al que está conectado el primer LED
#define NUMPIXELS 256   // Número total de LEDs en la matriz
#define NUM_ROWS 5      // Número de filas para cada número
#define NUM_COLS 3      // Número de columnas para cada número
#define SCREEN_ROWS 8   // Número de filas en la pantalla LED
#define SCREEN_COLS 32  // Número de columnas en la pantalla LED

Adafruit_NeoPixel pantalla = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const char* time_api_url = "http://worldtimeapi.org/api/timezone/America/Mexico_City";

WebServer server(80);

String hora_actual = "";
int diaSemana = 0;
int rojo = 0;
int verde = 0;
int azul = 255;
int brillo = 5;
int tempBrillo = 5;
static unsigned long twoPoints = 0;
static unsigned long animacion = 0;
bool twoP = true;
int numAnim = 1;
int anim = 4;

int figuras[12][SCREEN_ROWS][SCREEN_ROWS] = {
  { 
    {0  , 0  , 255, 255, 255, 255, 0  , 0  },
    {0  , 255, 0  , 0  , 0  , 0  , 255, 0  },
    {255, 0  , 255, 0  , 0  , 0  , 0  , 255},
    {255, 0  , 255, 0  , 0  , 0  , 0  , 255},
    {255, 0  , 0  , 255, 0  , 0  , 0  , 255},
    {255, 0  , 0  , 0  , 255, 255, 0  , 255},
    {0  , 255, 0  , 0  , 0  , 0  , 255, 0  },
    {0  , 0  , 255, 255, 255, 255, 0  , 0  },
  },
  { 
    {0  , 0  , 255, 255, 255, 255, 0  , 0  },
    {0  , 255, 255, 255, 255, 255, 255, 0  },
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {0  , 255, 255, 255, 255, 255, 255, 0  },
    {0  , 0  , 255, 255, 255, 255, 0  , 0  },
  },
  { 
    {0  , 0  , 255, 255, 255, 255, 0  , 0  },
    {0  , 255, 0  , 0  , 0  , 0  , 255, 0  },
    {255, 0  , 255, 0  , 0  , 0  , 0  , 255},
    {255, 0  , 255, 0  , 0  , 0  , 0  , 255},
    {255, 0  , 0  , 255, 0  , 0  , 0  , 255},
    {255, 0  , 0  , 0  , 255, 255, 0  , 255},
    {0  , 255, 0  , 0  , 0  , 0  , 255, 0  },
    {0  , 0  , 255, 255, 255, 255, 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 255, 255, 255, 0  },
    {0  , 0  , 0  , 0  , 255, 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 255, 0  , 0  , 0  },
    {0  , 0  , 0  , 255, 255, 255, 255, 0  },
    {0  , 0  , 0  , 0  , 255, 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 255, 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 255, 0  , 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 255, 255, 255, 0  },
    {0  , 0  , 0  , 0  , 255, 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 255, 0  , 0  , 0  },
    {0  , 0  , 0  , 255, 255, 255, 255, 0  },
    {0  , 0  , 0  , 0  , 255, 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 255, 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 255, 0  , 0  , 0  },
  },
  { 
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 255, 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 255, 0  , 0  , 255, 255, 0  , 0  },
    {0  , 255, 0  , 255, 0  , 0  , 255, 0  },
    {0  , 255, 0  , 255, 0  , 0  , 255, 0  },
    {0  , 255, 0  , 255, 0  , 0  , 255, 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 255, 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 255, 0  , 0  , 255, 255, 0  , 0  },
    {0  , 255, 0  , 255, 0  , 0  , 255, 0  },
    {0  , 255, 0  , 255, 0  , 0  , 255, 0  },
    {0  , 255, 0  , 255, 0  , 0  , 255, 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
  },
  { 
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 255, 255, 255, 255, 255, 255, 0  },
    {255, 0  , 255, 255, 255, 255, 0  , 255},
    {255, 255, 0  , 255, 255, 0  , 255, 255},
    {255, 255, 255, 0  , 0  , 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {125, 255, 255, 255, 255, 255, 255, 125},
    {255, 125, 255, 255, 255, 255, 125, 255},
    {255, 255, 125, 255, 255, 125, 255, 255},
    {255, 255, 255, 125, 125, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255},
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
  },
};

int numeros[11][NUM_ROWS][NUM_COLS] = {
  // Definición de los números y los dos puntos
  // 0
  { 
    {1, 1, 1},
    {1, 0, 1},
    {1, 0, 1},
    {1, 0, 1},
    {1, 1, 1}
  },
  // 1
  { 
    {0, 1, 0},
    {1, 1, 0},
    {0, 1, 0},
    {0, 1, 0},
    {1, 1, 1}
  },
  // 2
  { 
    {1, 1, 1},
    {0, 0, 1},
    {1, 1, 1},
    {1, 0, 0},
    {1, 1, 1}
  },
  // 3
  { 
    {1, 1, 1},
    {0, 0, 1},
    {1, 1, 1},
    {0, 0, 1},
    {1, 1, 1}
  },
  // 4
  { 
    {1, 0, 1},
    {1, 0, 1},
    {1, 1, 1},
    {0, 0, 1},
    {0, 0, 1}
  },
  // 5
  { 
    {1, 1, 1},
    {1, 0, 0},
    {1, 1, 1},
    {0, 0, 1},
    {1, 1, 1}
  },
  // 6
  { 
    {1, 1, 1},
    {1, 0, 0},
    {1, 1, 1},
    {1, 0, 1},
    {1, 1, 1}
  },
  // 7
  { 
    {1, 1, 1},
    {0, 0, 1},
    {0, 0, 1},
    {0, 0, 1},
    {0, 0, 1}
  },
  // 8
  { 
    {1, 1, 1},
    {1, 0, 1},
    {1, 1, 1},
    {1, 0, 1},
    {1, 1, 1}
  },
  // 9
  { 
    {1, 1, 1},
    {1, 0, 1},
    {1, 1, 1},
    {0, 0, 1},
    {0, 0, 1}
  },
  // dos puntos
  { 
    {0, 0, 0},
    {0, 1, 0},
    {0, 0, 0},
    {0, 1, 0},
    {0, 0, 0}
  },
};

void handleAction() {
  // Verificar si el parámetro 'msg' está presente en la URL
  if (server.hasArg("msg")) {
    String msg = server.arg("msg");
    
    
    // Aquí puedes añadir el código para realizar una acción basada en el valor del parámetro
    String message = "Mensaje recibido: " + msg;

    if (msg == "whatsapp") {
      animacion = 0;
      anim = 0;
      message = "notificación de " + msg + " recibida";
    }else{
      if (msg == "facebook") {
        animacion = 0;
        anim = 1;
        message = "notificación de " + msg + " recibida";
      }else{
        if (msg == "linkedin") {
          animacion = 0;
          anim = 2;
          message = "notificación de " + msg + " recibida";
        }else{
          if (msg == "correo") {
            animacion = 0;
            anim = 3;
            message = "notificación de " + msg + " recibida";
          }
        }
      }
    }
    

    // Responder con el mensaje personalizado
    server.send(200, "text/plain", message);
  } else {
    // Si el parámetro 'msg' no está presente, enviar un mensaje de error
    server.send(400, "text/plain", "Parámetro 'msg' faltante");
  }
}

// Función para mostrar un número en una posición específica en la pantalla
void showNumber(int number, int startX, int startY, int Red, int Green, int Blue) {
  for (int x = 0; x < NUM_ROWS; x++) {
    for (int y = 0; y < NUM_COLS; y++) {
      if (numeros[number][x][y] == 1) {
        int ledIndex = 0;
        if ((startY + y) % 2 == 0) {
          ledIndex = (x + startX) + (startY + y) * SCREEN_ROWS;
        } else {
          ledIndex = (SCREEN_ROWS - x - 1 - startX) + (startY + y) * SCREEN_ROWS;
        }
        if (x + startX < 8 && x + startX >= 0) {
          pantalla.setPixelColor(ledIndex, pantalla.Color(Red, Green, Blue));
        }
      }
    }
  }
}

// Función para mostrar un número en una posición específica en la pantalla
void showFigura(int number, int startX, int startY) {
  for (int x = 0; x < SCREEN_ROWS; x++) {
    for (int y = 0; y < SCREEN_ROWS; y++) {
      if (figuras[number*3][x][y] != 0 || figuras[number*3+1][x][y] != 0 || figuras[number*3+2][x][y] != 0) {
        int ledIndex = 0;
        if ((startY + y) % 2 == 0) {
          ledIndex = (x + startX) + (startY + y) * SCREEN_ROWS;
        } else {
          ledIndex = (SCREEN_ROWS - x - 1 - startX) + (startY + y) * SCREEN_ROWS;
        }
        if (x + startX < 8 && x + startX >= 0) {
          pantalla.setPixelColor(ledIndex, pantalla.Color(figuras[number*3][x][y], figuras[number*3+1][x][y], figuras[number*3+2][x][y]));
        }
      }
    }
  }
}

// Función para obtener la hora de la API
void obtenerHora() {
  HTTPClient http;
  http.begin(time_api_url);
  int httpCode = http.GET();
  
  if (httpCode > 0) {
    String payload = http.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);
    diaSemana = doc["day_of_week"];
    String datetime = doc["datetime"];
    hora_actual = datetime.substring(11, 16); // Extraer la hora en formato HH:MM
  } else {
    Serial.println("Error en la solicitud HTTP");
  }

  http.end();
}

// Función para mostrar la hora en la pantalla LED
void mostrarHora(int y, int x) {
  if (hora_actual.length() == 5) {
    int hh = hora_actual.substring(0, 2).toInt();
    if (hh > 12) {
      hh = hh - 12;
    }
    int mm = hora_actual.substring(3, 5).toInt();

    if (millis() - twoPoints > 1000) {
      twoP = !twoP;
      twoPoints = millis();
    }
    
    showNumber(hh / 10, y, 7 + x, 255, 255, 255); // Primera cifra de la hora
    showNumber(hh % 10, y, 11 + x, 255, 255, 255); // Segunda cifra de la hora
    if (twoP)
    {
      showNumber(10, y, 14 + x, 255, 255, 255); // Dos puntos
    }
    showNumber(mm / 10, y, 17 + x, 255, 255, 255); // Primera cifra de los minutos
    showNumber(mm % 10, y, 21 + x, 255, 255, 255); // Segunda cifra de los minutos

    if (y == 1 && x == 0)
    {
      int rTemp;
      int vTemp;
      int aTemp;
      for (int i = 0; i < 7; i++) {
        if (diaSemana-1 == i)
        {
          rTemp = rojo;
          vTemp = verde;
          aTemp = azul;
        }else
        {
          rTemp = rojo/4;
          vTemp = verde/4;
          aTemp = azul/4;
        }
        for (int j = 0; j < 3; j++) {
          if (j == 1) {
            pantalla.setPixelColor(SCREEN_ROWS * (2 + j + (i * 4)), pantalla.Color(rTemp, vTemp, aTemp));
          } else {
            pantalla.setPixelColor(7 + SCREEN_ROWS * (2 + j + (i * 4)), pantalla.Color(rTemp, vTemp, aTemp));
          }
        }
      }
    }
  }
}

// Callback para cambiar el estado de encendido/apagado
bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device %s power state changed to %s\n", deviceId.c_str(), state ? "on" : "off");
  if (state) {
    brillo = 5;
    pantalla.setBrightness(brillo);
  } else {
    brillo = 0;
    pantalla.setBrightness(brillo);
  }
  pantalla.show();
  return true;
}

// Callback para cambiar el color
bool onSetColor(const String &deviceId, byte &r, byte &g, byte &b) {
  Serial.printf("Device %s color changed to R:%d, G:%d, B:%d\n", deviceId.c_str(), r, g, b);
  rojo = r;
  verde = g;
  azul = b;
  return true;
}

// Callback para cambiar el brillo
bool onSetBrillo(const String &deviceId, int &b) {
  Serial.printf("Device %s color changed to brillo: %d", deviceId.c_str(), b);
  brillo = map(b,0,100,0,255);
  pantalla.setBrightness(brillo);
  return true;
}

void setupSinricPro() {
  SinricProLight &myLight = SinricPro[DEVICE_ID];
  myLight.onPowerState(onPowerState);
  myLight.onColor(onSetColor);
  myLight.onBrightness(onSetBrillo);
  myLight.onAdjustBrightness(onSetBrillo);

  SinricPro.begin(API_KEY, APP_SECRET);
}

void animIntermedio(int numAnim){
  static int step = 0;
  static unsigned long lastStepTime = 0;
  const unsigned long stepDelay = 10;  // Delay between steps in milliseconds

  if (millis() - lastStepTime > stepDelay) {
    switch (numAnim)
    {
    case 4:
      lastStepTime = millis();
      animacion = millis();
      break;
    
    
    default:
      step++;
      if (step <= 10)
      {
        pantalla.clear();
        mostrarHora(1,0 - step/2);
        showFigura(numAnim,0,32 - step);
        pantalla.show();
      }else{
        if (step <= 100)
        {
          pantalla.clear();
          mostrarHora(1,-5);
          showFigura(numAnim,0,22);
          pantalla.show();
        }else{
          if (step <= 110)
          {
            int temp = step - 100;
            pantalla.clear();
            mostrarHora(1,-5 + (temp/2));
            showFigura(numAnim,0,22 + temp);
            pantalla.show();
          }else{
            animacion = millis();
            step = 0;
            anim = 4;
          }
        }
      }
      lastStepTime = millis();
      break;
    }
    
  }
}

void animacionAleatoria() {
  animIntermedio(anim);
}


void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  pantalla.begin();
  pantalla.setBrightness(brillo);
  pantalla.clear();
  pantalla.show();

  server.on("/action", handleAction);
  server.begin();
  setupSinricPro(); // Configurar SinricPro

  obtenerHora(); // Obtener la hora al inicio
}

void loop() {
  SinricPro.handle(); // Manejar las comunicaciones con SinricPro
  server.handleClient();

  // Obtener y mostrar la hora cada minuto
  static unsigned long lastTime = 0;
  if (millis() - lastTime > 100) {
    lastTime = millis();
    if (millis() - animacion > 20000)
    {
      animacionAleatoria();
    }else{
      pantalla.clear();
      mostrarHora(1,0);
      pantalla.show();
    }
  }

  static unsigned long lastTime2 = 0;
  if (millis() - lastTime2 > 20000) {
    lastTime2 = millis();
    obtenerHora();
  }
}
