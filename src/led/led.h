#include <FastLED.h>

#include "../data/data.h"

#define LED_PIN 13
#define NUM_LEDS 90
#define MAX_POWER_MILLIAMPS 2000
#define LED_TYPE WS2812
#define COLOR_ORDER GRB

void ledBegin();
void testOutput();
void ledClear();
void movePalette(bool forward);
void setCurrentPalette(int16_t palette);
int16_t getCurrentPalette();
void waitForConnectionScreen();
void askForNetwork();
void outText(String text, int16_t x, CRGB color, bool isScrolling);
void outText(String text, int16_t x, bool isScrolling);
void printCh(uint16_t ch, int16_t x, CRGB color);
void outClock(uint8_t hours, uint8_t mins, bool showDelimiter);