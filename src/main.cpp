#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define PIN 19        // Pin digital al que está conectado el primer LED
#define NUMPIXELS 256 // Número total de LEDs en la matriz

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// put function declarations here:
void lightUpLeds();

void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Initialize the LED strip
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  lightUpLeds();
}

void lightUpLeds() {
  for (int i = 0; i < NUMPIXELS; i++) {
    Serial.print(i);
    strip.setPixelColor(i, strip.Color(0, 0, 255)); // Enciende el LED con color rojo
    strip.show();
    delay(100); // Retardo para ver el efecto de encendido de cada LED
    strip.setPixelColor(i, strip.Color(0, 0, 0)); // Apaga el LED después del retardo
  }
}
