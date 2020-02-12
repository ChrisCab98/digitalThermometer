#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <InfluxDbClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include "Wire.h"

char *ssid = "cabantous";    //WiFi AP Name
char *password = "B6krJhuZ"; //WiFi Password

#define DEVICE "ESP8266"

#define INFLUXDB_URL "http://192.168.68.164:8086"
#define INFLUXDB_DB_NAME "HomeKit"
#define INFLUXDB_USER "Chris"
#define INFLUXDB_PASSWORD "5ZG2fs"

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_DB_NAME);
Point sensor("wifi_status");
Point BME280_1("Temperature");
Adafruit_BME280 capteur;

void setup()
{
  Serial.begin(115200);

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

  // Wait 1s
  Serial.println("Wait 1s");
  delay(1000);
}