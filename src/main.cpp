#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>


#define PIN 19          // Pin digital al que está conectado el primer LED
#define NUMPIXELS 256   // Número total de LEDs en la matriz
#define NUM_ROWS 5      // Número de filas para cada número
#define NUM_COLS 3      // Número de columnas para cada número
#define SCREEN_ROWS 8   // Número de filas en la pantalla LED
#define SCREEN_COLS 32  // Número de columnas en la pantalla LED

Adafruit_NeoPixel pantalla = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const char* ssid = "cuartoRed";         // Reemplaza con tu SSID
const char* password = "irvyn2703"; // Reemplaza con tu contraseña

const char* time_api_url = "http://worldtimeapi.org/api/timezone/America/Mexico_City";

String hora_actual = "";
int rojo = 0;
int verde = 0;
int azul = 255;

int numeros[11][NUM_ROWS][NUM_COLS] = {
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
  //Serial.print("entro ---> ");
  //Serial.println(number);
  for (int x = 0; x < NUM_ROWS; x++) {
    for (int y = 0; y < NUM_COLS; y++) {
      //Serial.println(numeros[number][x][y]);
      if (numeros[number][x][y] == 1) {
        int ledIndex = 0;
        if ((startY + y) % 2 == 0)
        {
          ledIndex = (x + startX) + (startY + y) * SCREEN_ROWS;
        }else{
          ledIndex = (SCREEN_ROWS - x - 1 - startX) + (startY + y) * SCREEN_ROWS;
        }        
        //Serial.print(" x * startX = ");
        //Serial.println(x + startX);
        if (x + startX < 8)
        {
          //Serial.println(ledIndex);
          pantalla.setPixelColor(ledIndex, pantalla.Color(Red, Green, Blue)); // Color blanco para los LEDs encendidos
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
    String datetime = doc["datetime"];
    hora_actual = datetime.substring(11, 16); // Extraer la hora en formato HH:MM
    Serial.println(hora_actual);
  } else {
    Serial.println("Error en la solicitud HTTP");
  }

  http.end();
}

// Función para mostrar la hora en la pantalla LED
void mostrarHora() {
  if (hora_actual.length() == 5) {
    //Serial.print("entro hora ---> ");
    //Serial.println(hora_actual);
    int hh = hora_actual.substring(0, 2).toInt();
    if (hh > 12)
    {
      hh = hh - 12;
    }
    
    int mm = hora_actual.substring(3, 5).toInt();


    showNumber(hh / 10, 1, 7, 255, 255, 255); // Primera cifra de la hora
    showNumber(hh % 10, 1, 11, 255, 255, 255); // Segunda cifra de la hora
    showNumber(10, 1, 14, 255, 255, 255); // Segunda cifra de la hora
    showNumber(mm / 10, 1, 17, 255, 255, 255); // Primera cifra de los minutos
    showNumber(mm % 10, 1, 21, 255, 255, 255); // Segunda cifra de los minutos

    for (int i = 0; i < 7; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        if (j == 1)
        {
          pantalla.setPixelColor(SCREEN_ROWS * (2 + j + (i * 4)), pantalla.Color(rojo/4, verde/4, azul/4));
        }else{
          pantalla.setPixelColor(7 + SCREEN_ROWS * (2 + j + (i * 4)), pantalla.Color(rojo/4, verde/4, azul/4));
        }
      }
    }
  }
}

void setup() {
  // Iniciar comunicación serial
  Serial.begin(115200);

  // Conectar a la red Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");

  // Inicializar la matriz de LEDs
  pantalla.begin();
  pantalla.setBrightness(5);
  pantalla.clear(); // Apagar todos los LEDs
  for (int i = 0; i < NUMPIXELS + 2; i++)
  {
    pantalla.setPixelColor(i-2, pantalla.Color(0, 0, 0));
    pantalla.setPixelColor(i, pantalla.Color(255, 255, 255));
    pantalla.show();
    delay(1);
  }
  
  pantalla.show();

  // Crear tarea para obtener la hora
  xTaskCreate(
    [] (void * parameter) {
      for (;;) {
        obtenerHora();
        vTaskDelay(60000 / portTICK_PERIOD_MS); // Actualizar cada minuto
      }
    },
    "ObtenerHoraTask",
    10000,
    NULL,
    1,
    NULL
  );
}

void loop() {
  pantalla.clear();
  mostrarHora();
  pantalla.show();
  delay(1000); // Actualizar la pantalla cada segundo
}
