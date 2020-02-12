#include <InfluxDbClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

char *ssid = "cabantous"; //WiFi AP Name
char *password = "B6krJhuZ"; //WiFi Password

#define DEVICE "ESP8266"

#define INFLUXDB_URL "http://192.168.68.164:8086"
#define INFLUXDB_DB_NAME "HomeKit"
#define INFLUXDB_USER "Chris"
#define INFLUXDB_PASSWORD "5ZG2fs"

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_DB_NAME);
Point sensor("wifi_status");

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
  // client.setConnectionParamsV1(INFLUXDB_URL, INFLUXDB_DB_NAME, INFLUXDB_USER, INFLUXDB_PASSWORD);

  sensor.addTag("device", DEVICE);
  sensor.addTag("SSID", WiFi.SSID());
}

void loop()
{
  // Store measured value into point
  sensor.clearFields();
  // Report RSSI of currently connected network
  sensor.addField("rssi", WiFi.RSSI());
  // Print what are we exactly writing
  Serial.print("Writing: ");
  Serial.println(sensor.toLineProtocol());

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
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  //Wait 1s
  Serial.println("Wait 1s");
  delay(1000);
}
