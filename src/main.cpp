// Simple program with microcontroller NodeMCU esp8266 with led notification live feed btc-eur price (Binance API)
// Lazaros Psarokostas
// 2022

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <BoardConfig.h>

int red_light_pin = D7;
int green_light_pin = D6;
const char *host = "api.binance.com";
const int httpsPort = 443; // HTTPS= 443 and HTTP = 80

int brightness = 0; // how bright the LED is
int fadeAmount = 5; // how many points to fade the LED by
void setup()
{
  Serial.begin(115200);
  // Serial.println("test from file: " + String(ssid));
  WiFi.mode(WIFI_OFF); // Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        // Only Station No AP, This line hides the viewing of ESP as wifi hotspot
  WiFi.begin(ssid, password); // Connect to your WiFi router
  Serial.print("Connecting");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // IP address assigned to your ESP

  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
}

void RGB_color(int red_light_value, int green_light_value)
{
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
}

double num, num_older2 = 00000.00;
void loop()
{
  DynamicJsonDocument doc(1024);
  WiFiClientSecure httpsClient; // Declare object of class WiFiClient
  httpsClient.setFingerprint(fingerprint);
  httpsClient.setTimeout(5000); // 5 Seconds
  int r = 0;                    // retry counter
  while ((!httpsClient.connect(host, httpsPort)) && (r < 30))
  {
    delay(2000);
    Serial.print(".");
    r++;
  }

  // GET Data
  String Link = "/api/v3/ticker/price?symbol=BTCEUR";

  httpsClient.print(String("GET ") + Link + " HTTP/1.1\r\n" +
                    "Host: " + host + "\r\n" +
                    "Connection: close\r\n\r\n");

  while (httpsClient.connected())
  {
    String line = httpsClient.readStringUntil('\n');
    if (line == "\r")
    {
      break;
    }
  }

  String line;
  std::vector<double> num_olds;
  line = httpsClient.readStringUntil('\n');
  deserializeJson(doc, line);
  JsonObject obj = doc.as<JsonObject>();
  String price = obj["price"];
  num = price.toDouble();
  delay(1000);

  Serial.print("new value: ");
  Serial.println(num);
  Serial.print("previous value: ");
  Serial.println(num_older2);
  Serial.println("-------");

  if (num < num_older2)
  {
    RGB_color(255, 0); // Red
  }

  if (num > num_older2)
  {
    RGB_color(0, 255); // Green
  }

  if (num == 0 || num_older2 == 0)
  {
    RGB_color(0, 0); // Nothing
  }

  if (num == num_older2)
  {
    RGB_color(255, 255); // Red + Green
  }

  num_olds.push_back(num);
  if (!num_olds.empty())
  {
    num_older2 = num_olds[num_olds.size() - 1];
  }
}
