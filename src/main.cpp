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
int brillo = 10;
int tempBrillo = 5;
static unsigned long twoPoints = 0;
static unsigned long animacion = 0;
bool twoP = true;
int numAnim = 3;
int anim = 6;
int intervaloAnimaciones = 20000;
int hh = 0;
int mm = 0;

int figuras[30][SCREEN_ROWS][SCREEN_ROWS] = {
  // whatsapp
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
  // facebook
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
  // linkedin
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
  // correo
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
  // abeja
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 255, 255, 0  , 0  },
    {0  , 0  , 0  , 255, 255, 0  , 0  , 0  },
    {0  , 245, 245, 245, 50 , 245, 50 , 0  },
    {0  , 245, 50 , 245, 50 , 245, 50 , 50 },
    {0  , 245, 245, 245, 50 , 245, 50 , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 255, 255, 0  , 0  },
    {0  , 0  , 0  , 255, 255, 0  , 0  , 0  },
    {0  , 211, 211, 211, 50 , 211, 50 , 0  },
    {0  , 211, 50 , 211, 50 , 211, 50 , 50 },
    {0  , 211, 211, 211, 50 , 211, 50 , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 255, 255, 0  , 0  },
    {0  , 0  , 0  , 255, 255, 0  , 0  , 0  },
    {0  , 90 , 90 , 90 , 50 , 90 , 50 , 0  },
    {0  , 90 , 50 , 90 , 50 , 90 , 50 , 50 },
    {0  , 90 , 90 , 90 , 50 , 90 , 50 , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
  },
  // abeja 2
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 255, 0  , 0  },
    {0  , 0  , 0  , 255, 255, 255, 0  , 0  },
    {0  , 245, 245, 245, 50 , 245, 50 , 0  },
    {0  , 245, 50 , 245, 50 , 245, 50 , 50 },
    {0  , 245, 245, 245, 50 , 245, 50 , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 255, 0  , 0  },
    {0  , 0  , 0  , 255, 255, 255, 0  , 0  },
    {0  , 211, 211, 211, 50 , 211, 50 , 0  },
    {0  , 211, 50 , 211, 50 , 211, 50 , 50 },
    {0  , 211, 211, 211, 50 , 211, 50 , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 255, 0  , 0  },
    {0  , 0  , 0  , 255, 255, 255, 0  , 0  },
    {0  , 90 , 90 , 90 , 50 , 90 , 50 , 0  },
    {0  , 90 , 50 , 90 , 50 , 90 , 50 , 50 },
    {0  , 90 , 90 , 90 , 50 , 90 , 50 , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
  },
  //amongUs
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 255, 255, 255, 0  , 0  , 0  , 0  },
    {255, 255, 0  , 0  , 0  , 0  , 0  , 0  },
    {255, 255, 255, 255, 0  , 0  , 0  , 0  },
    {0  , 255, 255, 255, 0  , 0  , 0  , 0  },
    {0  , 255, 0  , 255, 0  , 0  , 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 255, 255, 255, 0  , 0  , 0  , 0  },
    {255, 255, 0  , 0  , 0  , 0  , 0  , 0  },
    {255, 255, 255, 255, 0  , 0  , 0  , 0  },
    {0  , 255, 255, 255, 0  , 0  , 0  , 0  },
    {0  , 255, 0  , 255, 0  , 0  , 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 255, 255, 255, 0  , 0  , 0  , 0  },
    {255, 255, 0  , 0  , 0  , 0  , 0  , 0  },
    {255, 255, 255, 255, 0  , 0  , 0  , 0  },
    {0  , 255, 255, 255, 0  , 0  , 0  , 0  },
    {0  , 255, 0  , 255, 0  , 0  , 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 255, 255, 255, 0  , 0  , 0  , 0  },
    {255, 255, 0  , 0  , 0  , 0  , 0  , 0  },
    {255, 255, 255, 255, 0  , 0  , 0  , 0  },
    {0  , 255, 255, 255, 0  , 0  , 0  , 0  },
    {0  , 0  , 255, 0  , 0  , 0  , 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 255, 255, 255, 0  , 0  , 0  , 0  },
    {255, 255, 0  , 0  , 0  , 0  , 0  , 0  },
    {255, 255, 255, 255, 0  , 0  , 0  , 0  },
    {0  , 255, 255, 255, 0  , 0  , 0  , 0  },
    {0  , 0  , 255, 0  , 0  , 0  , 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 255, 255, 255, 0  , 0  , 0  , 0  },
    {255, 255, 0  , 0  , 0  , 0  , 0  , 0  },
    {255, 255, 255, 255, 0  , 0  , 0  , 0  },
    {0  , 255, 255, 255, 0  , 0  , 0  , 0  },
    {0  , 0  , 255, 0  , 0  , 0  , 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 255, 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 255, 0  , 0  , 0  , 0  , 0  },
    {255, 255, 255, 255, 0  , 0  , 0  , 0  },
    {0  , 255, 255, 255, 0  , 0  , 0  , 0  },
    {0  , 255, 0  , 255, 0  , 0  , 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 255, 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 255, 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 255, 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 255, 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 137, 137, 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 188, 188, 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
  },
  { 
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 207, 207  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  },
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

void showAmongUs(int number, int startX, int startY, int R, int G, int B) {
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
          pantalla.setPixelColor(ledIndex, pantalla.Color(figuras[number*3][x][y]/R, figuras[number*3+1][x][y]/G, figuras[number*3+2][x][y]/B));
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
    hh = hora_actual.substring(0, 2).toInt();
    if (hh > 12) {
      hh = hh - 12;
    }
    mm = hora_actual.substring(3, 5).toInt();

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

void reiniciarAnimacion(int &step){
  animacion = millis();
  step = 0;
  anim = random(4, 4 + numAnim);
  intervaloAnimaciones = random(10000, 600000);
}

void animAbeja(int &step, unsigned long &lastStepTime) {
    step++;
    if (step <= 10) {
        pantalla.clear();
        mostrarHora(1, 0 - step*2);
        pantalla.show();
    } else {
        if (step <= 70) {
            int temp = step - 10;
            pantalla.clear();
            if (temp % 3 == 0){
              showFigura(4,0,0 + (29 - temp));
              showFigura(4,-3,7 + (29 - temp));
              showFigura(4,1,14 + (29 - temp));
              showFigura(4,-1,22 + (29 - temp));
            }else{
              showFigura(5,0,0 + (29 - temp));
              showFigura(5,-3,7 + (29 - temp));
              showFigura(5,1,14 + (29 - temp));
              showFigura(5,-1,22 + (29 - temp));
            }
            
            pantalla.show();
        } else {
            if (step <= 80) {
                int temp = step - 70;
                pantalla.clear();
                mostrarHora(1, 20 - temp*2);
                pantalla.show();
            } else {
              reiniciarAnimacion(step);
            }
        }
    }
    lastStepTime = millis();
}

void animAmongUs(int &step, unsigned long &lastStepTime) {
    step++;
    if (step <= 10) {
        pantalla.clear();
        mostrarHora(1, 0 + step*2);
        pantalla.show();
    } else {
        if (step <= 25) {
            int temp = step - 10;
            pantalla.clear();
            if (temp % 2 == 0){
              showAmongUs(6,0,0 + (temp), 1, 255, 255);
              showFigura(9,0,0 + (temp));
            }else{
              showAmongUs(7,0,0 + (temp), 1, 255, 255);
              showFigura(9,0,0 + (temp));
            }
            
            pantalla.show();
        } else {
          if (step <= 35)
          {
            int temp = step - 30;
            pantalla.clear();
            
            showAmongUs(6,0,15, 1, 255, 255);
            showFigura(9,0,15);

            if (temp % 2 == 0){
              showAmongUs(6,0,0 + (temp*2), hh/3 + 1, mm/15 + 1, (hh/3 + mm/15)/2 + 1);
              showFigura(9,0,0 + (temp*2));
            }else{
              showAmongUs(7,0,0 + (temp*2), hh/3 + 1, mm/15 + 1, (hh/3 + mm/15)/2 + 1);
              showFigura(9,0,0 + (temp*2));
            }

            pantalla.show();
          } else {
            if (step <= 39)
            {
              if (step == 36)
              {
                pantalla.clear();
              }
              

              int temp = step - 35;
              
              int x = 5;  
              int y = 15 + temp;
              int ledIndex = 0;

              if (y % 2 == 0) {
                ledIndex = x + (y) * SCREEN_ROWS;
              } else {
                ledIndex = (SCREEN_ROWS - x - 1) + (y) * SCREEN_ROWS;
              }

              if (x < 8 && x >= 0) {
                pantalla.setPixelColor(ledIndex, pantalla.Color(255, 255, 255));
              }

              pantalla.show();
            } else{
              if(step <= 70){
                pantalla.clear();
                

                int temp = step - 39;
                
                showFigura(8,0,15);

                showAmongUs(6,0,18 + temp, hh/3 + 1, mm/15 + 1, (hh/3 + mm/15)/2 + 1);
                showFigura(9,0,18 + temp);

                pantalla.show();

              }else{
                if (step <= 80) {
                    int temp = step - 70;
                    pantalla.clear();
                    mostrarHora(1, 20 - temp*2);
                    pantalla.show();
                } else {
                  reiniciarAnimacion(step);
                }
              }
            }
          }
        }
    }
    lastStepTime = millis();
}

void animAmongUsRun(int &step, unsigned long &lastStepTime) {
    step++;
    if (step <= 70) {
        pantalla.clear();

        mostrarHora(1, 0);

        if (step % 2 == 0){
          showAmongUs(6,0,0 + (step), 1, 255, 255);
          showFigura(9,0,0 + (step));
          showAmongUs(6,0,-70 + step*2.5, hh/3 + 1, mm/15 + 1, (hh/3 + mm/15)/2 + 1);
          showFigura(9,0,-70 + step*2.5);
        }else{
          showAmongUs(7,0,0 + (step), 1, 255, 255);
          showFigura(9,0,0 + (step));
          showAmongUs(7,0,-70 + step*2.5, hh/3 + 1, mm/15 + 1, (hh/3 + mm/15)/2 + 1);
          showFigura(9,0,-70 + step*2.5);
        }
        pantalla.show();
    } else{
      reiniciarAnimacion(step);
    }
          
    lastStepTime = millis();
}

void animIntermedio(int numAnim){
  static int step = 0;
  static unsigned long lastStepTime = 0;
  const unsigned long stepDelay = 10;  // Delay between steps in milliseconds

  if (millis() - lastStepTime > stepDelay) {
    switch (numAnim)
    {
    case 4:
      animAbeja(step, lastStepTime);
      break;
    case 5:
      animAmongUs(step, lastStepTime);
      break;
    case 6:
      animAmongUsRun(step, lastStepTime);
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
            anim = random(4, 4 + numAnim);
            intervaloAnimaciones = random(20000, 900000);
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
  randomSeed(analogRead(0));
}

void loop() {
  SinricPro.handle(); // Manejar las comunicaciones con SinricPro
  server.handleClient();

  // Obtener y mostrar la hora cada minuto
  static unsigned long lastTime = 0;
  if (millis() - lastTime > 100) {
    lastTime = millis();
    if (millis() - animacion > intervaloAnimaciones)
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
