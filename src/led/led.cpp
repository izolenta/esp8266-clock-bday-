#include "led.h"

CRGB leds[NUM_LEDS];
uint8_t prevMinute = 99;
int8_t currentPalette = 0;

CRGB palette[17][3];

uint16_t xyLED(int16_t x, int16_t y) {
  if (x < 0 || x > 17 || y < 0 || y > 5) {
    return -1;
  }
  uint16_t targetLED = 0;
  if (x % 2 == 0) {
    targetLED = x / 2 + (4-y) * (RES_X + 1);
  } else {
    targetLED = RES_X + 1 - x + x / 2 + (4-y) * (RES_X + 1);
  }
  return targetLED;
}

void ledClear() {
  FastLED.clear();
  prevMinute = 99;
}

void ledBegin() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
      .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(255);
  FastLED.clear();
  for (int i=0; i<8; i++) {
    palette[i][0] = CHSV(i * 32, 255, 255);
    palette[i][1] = CHSV((i * 32 + 85) % 256, 255, 255);
    palette[i][2] = CHSV((i * 32 + 170) % 256, 255, 255);
  }
  for (int i = 0; i < 8; i++) {
    palette[i + 8][0] = CHSV(i * 32, 255, 255);
    palette[i + 8][1] = CHSV(i * 32, 255, 255);
    palette[i + 8][2] = CHSV(i * 32, 255, 255);
  }
  palette[16][0] = CHSV(0, 0, 255);
  palette[16][1] = CHSV(0, 0, 255);
  palette[16][2] = CHSV(0, 0, 255);
  FastLED.show();
}

void outText(String text, int16_t x, bool isScrolling) {
  outText(text, x, CRGB(0,0,1), isScrolling);
}

void outText(String text, int16_t x, CRGB color, bool isScrolling) {
  uint8_t gap = isScrolling? 5 : 4;
  int16_t currentX = x;
  for (uint16_t i = 0; i<text.length(); i++) {
    if (currentX < -3) {
      currentX += gap;
      continue;
    }
    if (currentX > 17) {
      break;
    }
    uint8_t ch = symOrder.indexOf(text.charAt(i));
    if (ch < 0) {
      ch = unknownChar;
    }

    if (color == CRGB(0, 0, 1)) {
      printCh(ch, currentX, CHSV(random8(), 255, 255));
    } else {
      printCh(ch, currentX, color);
    }
    currentX += gap;
  }
  FastLED.show();
}

void outClock(uint8_t hours, uint8_t mins, bool showDelimiter) {
  if (prevMinute != mins) {
    FastLED.clear();
    prevMinute = mins;
  }
  String hrs = String(hours);
  if (hours < 10) {
    outText(hrs, 4, palette[currentPalette][0], false);
  }
  else {
    outText(hrs, 0, palette[currentPalette][0], false);
  }
  outText(":", 7, showDelimiter ? palette[currentPalette][1] : CRGB::Black,
          false);
  String min = String(mins);
  if (mins < 10) {
    min = "0"+min;
  }
  outText(min, 10, palette[currentPalette][2], false);
  FastLED.show();
}

void movePalette(bool forward) {
  if (forward) currentPalette++;
  else currentPalette--;
  Serial.println(currentPalette, DEC);
  if (currentPalette > 16) currentPalette = 0;
  if (currentPalette < 0) currentPalette = 16;
  prevMinute = 99;
}

void testOutput() {
  leds[37] = CRGB::Yellow;
  FastLED.show();
}

void printCh(uint16_t ch, int16_t x, CRGB color) {
  const uint8_t *arr = characters[ch];
  for (u_int8_t i = 0; i<5; i++) {
    for (u_int8_t j = 0; j < 3; j++) {
      if (arr[i * 3 + j] == 1) {
        int16_t addr = xyLED(x + j, i);
        if (addr >=0 && addr < NUM_LEDS) {
          leds[addr] = color;
        }
      }
    }
  }
}

void askForNetwork() { 
  FastLED.clear();
  FastLED.clearData();
  FastLED.delay(200);
  leds[(xyLED(8, 4))] = CHSV(HUE_ORANGE, 255, 255);
  leds[(xyLED(8, 3))] = CHSV(HUE_ORANGE, 255, 255);
  leds[(xyLED(8, 2))] = CHSV(HUE_ORANGE, 255, 255);
  leds[(xyLED(6, 1))] = CHSV(HUE_ORANGE, 255, 255);
  leds[(xyLED(5, 2))] = CHSV(HUE_ORANGE, 255, 255);
  leds[(xyLED(6, 3))] = CHSV(HUE_ORANGE, 255, 255);
  leds[(xyLED(3, 1))] = CHSV(HUE_ORANGE, 255, 255);
  leds[(xyLED(3, 2))] = CHSV(HUE_ORANGE, 255, 255);
  leds[(xyLED(3, 3))] = CHSV(HUE_ORANGE, 255, 255);
  leds[(xyLED(4, 0))] = CHSV(HUE_ORANGE, 255, 255);
  leds[(xyLED(4, 4))] = CHSV(HUE_ORANGE, 255, 255);
  leds[(xyLED(11, 2))] = CHSV(HUE_ORANGE, 255, 255);
  leds[(xyLED(10, 3))] = CHSV(HUE_ORANGE, 255, 255);
  leds[(xyLED(10, 1))] = CHSV(HUE_ORANGE, 255, 255);
  leds[(xyLED(13, 1))] = CHSV(HUE_ORANGE, 255, 255);
  leds[(xyLED(13, 2))] = CHSV(HUE_ORANGE, 255, 255);
  leds[(xyLED(13, 3))] = CHSV(HUE_ORANGE, 255, 255);
  leds[(xyLED(12, 0))] = CHSV(HUE_ORANGE, 255, 255);
  leds[(xyLED(12, 4))] = CHSV(HUE_ORANGE, 255, 255);
  FastLED.show();
}

void waitForConnectionScreen() {
  FastLED.clear();
  leds[(xyLED(7, 1))] = CHSV(HUE_GREEN, 255, 255);
  leds[(xyLED(9, 1))] = CHSV(HUE_GREEN, 255, 255);
  leds[(xyLED(7, 0))] = CHSV(HUE_GREEN, 255, 255);
  leds[(xyLED(9, 0))] = CHSV(HUE_GREEN, 255, 255);
  leds[(xyLED(6, 3))] = CHSV(HUE_GREEN, 255, 255);
  leds[(xyLED(10, 3))] = CHSV(HUE_GREEN, 255, 255);
  leds[(xyLED(7, 4))] = CHSV(HUE_GREEN, 255, 255);
  leds[(xyLED(8, 4))] = CHSV(HUE_GREEN, 255, 255);
  leds[(xyLED(9, 4))] = CHSV(HUE_GREEN, 255, 255);
  FastLED.show();
  FastLED.delay(2000);
  leds[(xyLED(7, 0))] = CHSV(0, 0, 0);
  FastLED.show();
  FastLED.delay(100);
  leds[(xyLED(7, 1))] = CHSV(0, 0, 0);
  FastLED.show();
  FastLED.delay(700);
  leds[(xyLED(7, 1))] = CHSV(HUE_GREEN, 255, 255);
  FastLED.show();
  FastLED.delay(100);
  leds[(xyLED(7, 0))] = CHSV(HUE_GREEN, 255, 255);
  FastLED.show();
  FastLED.delay(2000);
}

void setCurrentPalette(int16_t palette) {
  currentPalette = palette;
}

int16_t getCurrentPalette() { return currentPalette; }