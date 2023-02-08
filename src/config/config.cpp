#include "config.h"

ConfigData readConfig() {
  Serial.println("READ CFG");
  File file = LittleFS.open("/settings.json", "r");
  if (file) {
    StaticJsonDocument<512> json;
    DeserializationError error = deserializeJson(json, file);
    serializeJsonPretty(json, Serial);
    if (!error) {
      Serial.println("Parsing JSON");
      const uint8_t override = json["overrideTimezone"] | 0;
      const uint8_t paletteUsed =json["paletteUsed"];
      Serial.println(override);
      Serial.println(paletteUsed, DEC);
      return {override, paletteUsed};
    } else {
      // Error loading JSON data
      Serial.println("Failed to load json config");
    }
  }
  Serial.println("READ CFG NONO");
  return {0, 0};
}

bool saveConfig(ConfigData data) {
  Serial.println("SAVE CFG");
  StaticJsonDocument<512> json;
  json["overrideTimezone"] = data.overrideTimezone;
  json["paletteUsed"] = data.lastUsedPalette;
  File file = LittleFS.open("/settings.json", "w");
  if (!file) {
    Serial.println("failed to open config file for writing");
    return false;
  }
  serializeJsonPretty(json, Serial);
  bool failure = serializeJson(json, file) == 0;
  if (failure) {
    // Error writing file
    Serial.println(F("Failed to write to file"));
  }
  file.close();
  return !failure;
}