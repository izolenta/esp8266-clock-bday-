#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "../data/data.h"

struct ConfigData {
  uint8_t overrideTimezone;
  uint8_t lastUsedPalette;
};

ConfigData readConfig();
bool saveConfig(ConfigData data);