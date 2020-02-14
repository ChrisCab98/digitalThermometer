#include <Adafruit_SSD1306.h>
#include <splash.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_SPITFT.h>
#include <gfxfont.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "Wire.h"

#include <InfluxDbClient.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

// char *ssid = "cabantous"; //WiFi AP Name
// char *password = "B6krJhuZ"; //WiFi Password

char *ssid = "TP-Link_1526"; //WiFi AP Name
char *password = "00102698"; //WiFi Password

#define DEVICE "ESP8266"

// #define INFLUXDB_URL "http://192.168.68.143:8086" // DVB
#define INFLUXDB_URL "http://192.168.1.25:8086" // Home
#define INFLUXDB_DB_NAME "HomeKit"
#define INFLUXDB_USER "Chris"
#define INFLUXDB_PASSWORD "5ZG2fs"

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_DB_NAME);
Point sensor("wifi_status");
Point BME280_1("Temperature");

Adafruit_BME280 capteur;

Adafruit_SSD1306 display(-1);

void setup()
{
  Serial.begin(115200);

  // initialize with the I2C addr 0x3C
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Clear the buffer.
  display.clearDisplay();

  // Display Text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 14);
  display.println("Connecting to WiFi ...");
  display.display();
  delay(2000);
  display.clearDisplay();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  client.setConnectionParamsV1(INFLUXDB_URL, INFLUXDB_DB_NAME);

  sensor.addTag("device", DEVICE);
  sensor.addTag("SSID", WiFi.SSID());

  BME280_1.addTag("Sensor_ID", DEVICE);

  Serial.println("Starting BME280... ");
  delay(10); // attente de la mise en route du capteur. 2 ms minimum

  capteur.begin(0x76);
  // Clear the buffer.
  display.clearDisplay();

  // Display Text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 14);
  display.println("Connected !");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop()
{
  // Store measured value into point
  sensor.clearFields();
  BME280_1.clearFields();

  // Report RSSI of currently connected network
  sensor.addField("rssi", WiFi.RSSI());

  BME280_1.addField("Temperature", capteur.readTemperature());
  BME280_1.addField("Pressure", capteur.readPressure());
  BME280_1.addField("Humidity", capteur.readHumidity());

  // Print what are we exactly writing
  Serial.print("Writing: ");
  Serial.println(sensor.toLineProtocol());
  Serial.println(BME280_1.toLineProtocol());

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Wifi Connection lost, Reconnect");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the WiFi network");
  }

  // Write point
  if (!client.writePoint(sensor))
  {
    Serial.print("InfluxDB write Sensor failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  // Write point
  if (!client.writePoint(BME280_1))
  {
    Serial.print("InfluxDB write BME280 failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(32, 14);
  display.println(capteur.readTemperature());
  display.display();
  // Wait 1s
  Serial.println("Wait 1s");
  delay(1000);
  display.clearDisplay();

  // ESP.deepSleepInstant(5000*1000);
}