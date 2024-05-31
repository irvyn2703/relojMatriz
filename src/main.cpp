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
void mostrarHora(int y) {
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
    
    showNumber(hh / 10, y, 7, 255, 255, 255); // Primera cifra de la hora
    showNumber(hh % 10, y, 11, 255, 255, 255); // Segunda cifra de la hora
    if (twoP && y == 1)
    {
      showNumber(10, y, 14, 255, 255, 255); // Dos puntos
    }
    showNumber(mm / 10, y, 17, 255, 255, 255); // Primera cifra de los minutos
    showNumber(mm % 10, y, 21, 255, 255, 255); // Segunda cifra de los minutos

    if (y == 1)
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
          rTemp = rojo/3;
          vTemp = verde/3;
          aTemp = azul/3;
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
    brillo = tempBrillo;
    pantalla.setBrightness(brillo);
  } else {
    tempBrillo = brillo;
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

void animacionAleatoria() {
  
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");

  pantalla.begin();
  pantalla.setBrightness(brillo);
  pantalla.clear();
  pantalla.show();

  setupSinricPro(); // Configurar SinricPro

  obtenerHora(); // Obtener la hora al inicio
}

void loop() {
  SinricPro.handle(); // Manejar las comunicaciones con SinricPro

  // Obtener y mostrar la hora cada minuto
  static unsigned long lastTime = 0;
  if (millis() - lastTime > 100) {
    lastTime = millis();
    if (millis() - animacion > 10000)
    {
      animacionAleatoria();
    }else{
      pantalla.clear();
      mostrarHora(1);
      pantalla.show();
    }
  }

  static unsigned long lastTime2 = 0;
  if (millis() - lastTime2 > 20000) {
    lastTime2 = millis();
    obtenerHora();
  }
}
