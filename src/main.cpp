#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define PIN 19          // Pin digital al que está conectado el primer LED
#define NUMPIXELS 256   // Número total de LEDs en la matriz
#define NUM_ROWS 5      // Número de filas para cada número
#define NUM_COLS 3      // Número de columnas para cada número
#define SCREEN_ROWS 8   // Número de filas en la pantalla LED
#define SCREEN_COLS 32  // Número de columnas en la pantalla LED

Adafruit_NeoPixel pantalla = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Definición de los números del 0 al 9
int numeros[10][NUM_ROWS][NUM_COLS] = {
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
  }
};

// Función para mostrar un número en una posición específica en la pantalla
void showNumber(int number, int startX, int startY) {
  for (int x = 0; x < NUM_ROWS; x++) {
    for (int y = 0; y < NUM_COLS; y++) {
      if (numeros[number][x][y] == 1) {
        int ledIndex = 0;
        if ((startY + y) % 2 == 0)
        {
          ledIndex = (x + startX) + (startY + y) * SCREEN_ROWS;
        }else{
          ledIndex = (SCREEN_ROWS - x - 1 - startX) + (startY + y) * SCREEN_ROWS;
        }        
        if (x + startX < 8)
        {
          Serial.println(ledIndex);
          pantalla.setPixelColor(ledIndex, pantalla.Color(255, 255, 255)); // Color blanco para los LEDs encendidos
        }
      }
    }
  }
}

// Función para borrar un número de una posición específica en la pantalla
void clearNumber(int startX, int startY) {
  for (int y = 0; y < NUM_ROWS; y++) {
    for (int x = 0; x < NUM_COLS; x++) {
      int ledIndex = (startY + y) * SCREEN_COLS + startX + x;
      pantalla.setPixelColor(ledIndex, pantalla.Color(0, 0, 0)); // Color negro para apagar los LEDs
    }
  }
  pantalla.show();
}

void setup() {
  // Iniciar comunicación serial
  Serial.begin(115200);

  // Inicializar la matriz de LEDs
  pantalla.begin();
  pantalla.setBrightness(5);
  pantalla.clear(); // Apagar todos los LEDs
  pantalla.show();
}

void loop() {
  for (int i = 0; i < 29; i++)
  {
    for (int j = 0; j < 10; j++)
    {
      showNumber(j, 2, i);
      showNumber(j, 2, i+4);
      showNumber(j, 2, i+9);
      showNumber(j, 2, i+13);
      pantalla.show();
      delay(500);
      pantalla.clear();
    }  
  }
}
