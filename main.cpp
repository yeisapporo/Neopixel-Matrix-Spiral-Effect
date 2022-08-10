// Rainbow-enhanced theater marquee with spiral effect.
// for Neopixel 8x8 matrix.
// Modified by: Kazuteru Yamada(yeisapporo).

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN   (6)
#define LED_COUNT (8 * 8)

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

#define EZMTS_USE_TIMER (0)
#include <ezMTS.hpp>

ezMTS task(2, EZMTS_MILLISEC);
int taskIdMain;
int taskIdSpiral;

char spiralTable[LED_COUNT] = {
   0,  1,  2,  3,  4,  5,  6,  7,
  15, 23, 31, 39, 47, 55, 63, 62,
  61, 60, 59, 58, 57, 56, 48, 40,
  32, 24, 16,  8,  9, 10, 11, 12,
  13, 14, 22, 30, 38, 46, 54, 53,
  52, 51, 50, 49, 41, 33, 25, 17,
  18, 19, 20, 21, 29, 37, 45, 44,
  43, 42, 34, 26, 27, 28, 36, 35,
};
char onOffTable[LED_COUNT];

int spiralControl(void *dummy) {
  static int dir = 1;
  static int max = 0;

  for(int i = 0; i < LED_COUNT; i++) {
    onOffTable[i] = 1;
  }
  for(int i = 0; i < max; i++) {
    onOffTable[spiralTable[i]] = 0;
  } 
  max += dir;
  if(max == LED_COUNT || max == -1) {
    dir = -dir;
  } 

  return 0;
}

int theaterChaseRainbowWithSpriralEffect(void *dummy) {
  static int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  static int a = 0;
  for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
    strip.clear();         //   Set all pixels in RAM to 0 (off)
    // 'c' counts up from 'b' to end of strip in increments of 3...
    for(int c=b; c<strip.numPixels(); c += 3) {
      // hue of pixel 'c' is offset by an amount to make one full
      // revolution of the color wheel (range 65536) along the length
      // of the strip (strip.numPixels() steps):
      int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
      uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
      strip.setPixelColor(c, color * onOffTable[c]); // Set pixel 'c' to value 'color'
    }

    strip.show();                // Update strip with new contents
    firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
  }

  if(++a > 30) {
    a = 0;
  }

  return 0;
}

void setup() {
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  taskIdMain = task.create(theaterChaseRainbowWithSpriralEffect);
  task.start(taskIdMain, 20, EZMTS_AT_ONCE);
  taskIdSpiral = task.create(spiralControl);
  task.start(taskIdSpiral, 50, EZMTS_AT_ONCE);
}

void loop() {
}
