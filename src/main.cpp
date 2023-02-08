#include <RTClib.h>
#include <GyverOS.h>
#include "led/led.h"
#include <LittleFS.h>
#include "WiFiManager.h"
#include "config/config.h"
#include <EncButton2.h>
#include <NTPClient.h>
#include <HttpClient.h>
#include <time.h>

RTC_DS3231 rtcBase;
EncButton2<EB_BTN> controlBtn(INPUT_PULLUP, 14);
GyverOS<3> OS;
WiFiManager wifiManager;
ConfigData state = {0, 0};
int32_t readTZ = 0;

uint8_t timerSkip = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

bool delimiter = true;
int8_t brightnessGap = 8;
uint8_t mode = MODE_CLOCK;
bool fsAvailable = false;

void clockTick() {
  outClock((rtcBase.now().hour() + state.overrideTimezone + (readTZ / 3600)) % 24, rtcBase.now().minute(), delimiter);
  delimiter = !delimiter;
}

void startAPw(WiFiManager *mgr) {
  Serial.println("Callback");
  askForNetwork();
}
void readOffset() {
  WiFiClient client;
  const int httpPort = 80;
  Serial.println("update offset");
  client.setTimeout(1500);
  if (!client.connect("ip-api.com", httpPort)) {
    Serial.println("connection failed");
    readTZ = 0;
  }
  HttpClient http = HttpClient(client, "ip-api.com");
  http.setTimeout(1500);
  String url = "json/?fields=offset";
  http.get(url);
  int statusCode = http.responseStatusCode();
  String response = http.responseBody();
  Serial.println("begin");
  Serial.println("GET");
  if (statusCode != 200) {
    Serial.print("Bad status code: ");
    Serial.println(statusCode, DEC);
    readTZ = 0;
  }
  StaticJsonDocument<128> doc;

  DeserializationError error = deserializeJson(doc, response);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    doc.garbageCollect();
    readTZ = 0;
  }

  int32_t offset = doc["offset"];    

  Serial.print("Read offset: ");
  Serial.println(offset, DEC);

  readTZ = offset;
}

void updateTime() {
  readOffset();
  timeClient.update();
  rtcBase.adjust(DateTime(timeClient.getEpochTime()));
  Serial.println("Adjusted time");
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("Beginning");
  if (!rtcBase.begin()) {
    Serial.println("Fail");
  }
  ledBegin();
  delay(200);
  waitForConnectionScreen();
  Serial.println("Begin RTC");
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
  }
  else {
    fsAvailable = true;
    state = readConfig();
    Serial.println(state.overrideTimezone);
    setCurrentPalette(state.lastUsedPalette);
  }  

  ledClear();
  wifiManager.setAPCallback(startAPw);
  wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 1, 1),
                                  IPAddress(192, 168, 1, 1),
                                  IPAddress(255, 255, 255, 0));
  wifiManager.autoConnect(AP_NAME, AP_PASSWORD);

  timeClient.begin();
  updateTime();

  OS.attach(0, clockTick, 500);
  OS.attach(1, updateTime, 3600000);
  controlBtn.setStepTimeout(100);
  controlBtn.setButtonLevel(HIGH);
}

void doHappyBirthday() {
  mode = MODE_HAPPYBIRTHDAY;
  OS.stop(0);
  for (int16_t x = 18; x > -135; x--) {
    FastLED.clear();
    outText("HAPPY BIRTHDAY ARKADIY!", x, true);
    FastLED.delay(50);
  }
  mode = MODE_CLOCK;
  OS.start(0);
}

void loop() {
  OS.tick();
  controlBtn.tick();
  if (rtcBase.now().day() == ARK_BDAY && rtcBase.now().month() == ARK_BMONTH && rtcBase.now().second() == 0) {
    doHappyBirthday();
  }
  if (controlBtn.hasClicks(1)) {
    movePalette(true);
    ledClear();
    clockTick();
    state.lastUsedPalette = getCurrentPalette();
    saveConfig(state);
  }
  if (controlBtn.hasClicks(2)) {
    movePalette(false);
    ledClear();
    clockTick();
    state.lastUsedPalette = getCurrentPalette();
    saveConfig(state);
  }
  if (controlBtn.hasClicks(5)) {
    ledClear();
    wifiManager.startConfigPortal(AP_NAME, AP_PASSWORD);
    ledClear();
  }
  if (controlBtn.hasClicks(4)) {
    doHappyBirthday();
  }
  if (controlBtn.step(1)) {
    Serial.println("BRI CHG +");
    uint8_t brigthness = FastLED.getBrightness();
    brigthness = max (5, min(255, brigthness + brightnessGap));
    Serial.println(brigthness, DEC);
    FastLED.setBrightness(brigthness);
    FastLED.show();
  }
  if (controlBtn.step(2)) {
    Serial.println("BRI CHG -");
    uint8_t brigthness = FastLED.getBrightness();
    brigthness = max(5, min(255, brigthness - brightnessGap));
    Serial.println(brigthness, DEC);
    FastLED.setBrightness(brigthness);
    FastLED.show();
  }
  if (controlBtn.step(3)) {
    timerSkip++;
    if (timerSkip == 5) {
      timerSkip = 0;
      state.overrideTimezone++;
      ledClear();
      clockTick();
      if (state.overrideTimezone == 24) {
        state.overrideTimezone = 0;
      }
    }
  }
  if (controlBtn.releaseStep(3)) {
    timerSkip = 0;
    saveConfig(state);
  }
}
