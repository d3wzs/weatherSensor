#include <TFT_eSPI.h>
#include <DHT.h>
#include <Adafruit_VEML7700.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define TFT_CS 5
#define TFT_DC 2
#define TFT_RST 27
#define DHTPIN 14
#define DHTTYPE DHT22
#define ONE_WIRE_BUS 4

#define BUTTON_1 32
#define BUTTON_2 33

DHT dht(DHTPIN, DHTTYPE);
TFT_eSPI tft = TFT_eSPI();
Adafruit_VEML7700 veml = Adafruit_VEML7700();
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
const unsigned long debounceDelay = 50;
const unsigned long doubleClickDelay = 300;

unsigned long clickTimer1 = 0;
int clickCount1 = 0;
unsigned long clickTimer2 = 0;
int clickCount2 = 0;

bool lastButton1State = HIGH;
bool lastButton2State = HIGH;

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);

  dht.begin();
  ds18b20.begin();
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("System Ready", tft.width() / 2, tft.height() / 2);

  if (!veml.begin()) {
    Serial.println("VEML7700 sensor not found");
    tft.fillScreen(TFT_BLACK);
    tft.drawString("VEML has bigg error", tft.width() / 2, tft.height() / 2);
    while (1) {
      delay(10);
    }
  }
}

void loop() {
  bool reading1 = digitalRead(BUTTON_1);
  bool reading2 = digitalRead(BUTTON_2);

  // biutton 1
  if (reading1 != lastButton1State) {
    lastDebounceTime1 = millis();
  }

  if ((millis() - lastDebounceTime1) > debounceDelay) {
    if (reading1 == LOW && lastButton1State == HIGH) {
      if (clickCount1 == 0) {
        clickTimer1 = millis();
        clickCount1 = 1;
      } else if (clickCount1 == 1 && (millis() - clickTimer1 < doubleClickDelay)) {
        clickCount1 = 2;
      }
    }
  }
  lastButton1State = reading1;

  if (clickCount1 > 0 && (millis() - clickTimer1 > doubleClickDelay)) {
    if (clickCount1 == 1) {
      float t = dht.readTemperature();

      tft.fillScreen(TFT_BLACK);
      String tempString = "Indoor Temp: " + String(t, 1) + " C";
      tft.drawString(tempString, tft.width() / 2, tft.height() / 2);

    } else if (clickCount1 == 2) {
      ds18b20.requestTemperatures();
      float outdoorTemp = ds18b20.getTempCByIndex(0);

      tft.fillScreen(TFT_BLACK);
      String outdoorString = "Outdoor Temp: " + String(outdoorTemp, 1) + " C";
      tft.drawString(outdoorString, tft.width() / 2, tft.height() / 2);
    }
    clickCount1 = 0;
  }
  // button 2
  if (reading2 != lastButton2State) {
    lastDebounceTime2 = millis();
  }

  if ((millis() - lastDebounceTime2) > debounceDelay) {
    if (reading2 == LOW && lastButton2State == HIGH) {
      if (clickCount2 == 0) {
        clickTimer2 = millis();
        clickCount2 = 1;
      } else if (clickCount2 == 1 && (millis() - clickTimer2 < doubleClickDelay)) {
        clickCount2 = 2;
      }
    }
  }
  lastButton2State = reading2;

  if (clickCount2 > 0 && (millis() - clickTimer2 > doubleClickDelay)) {
    if (clickCount2 == 1) {
      float lux = veml.readLux();

      tft.fillScreen(TFT_BLACK);
      String luxString = "Lux: " + String(lux, 1);
      tft.drawString(luxString, tft.width() / 2, tft.height() / 2);

    } else if (clickCount2 == 2) {
      // nothing for now 
    }
    clickCount2 = 0;
  }
}
