#include <ESP8266HTTPClient.h>

#include <ArduinoJson.h>

#include "ESP8266WiFi.h"

// your network SSID (name) & network password
char myssid[] = "UCLA_WEB";
char mypass[] = "";

// unwiredlabs Hostname & Geolocation Endpoint url
const char *host = "us1.unwiredlabs.com";
String endpoint = "/v2/process.php";
const int httpsPort = 443;

const char *fingerprint = "41 3F 95 84 0A E3 74 F6 2B C3 19 27 C9 67 F3 0C 38 D4 6F B6";

// UnwiredLabs API_Token. Signup here to get a free token https://unwiredlabs.com/trial
String token = "pk.73d4083cea6ff7527b49262ee534f8f1";
String jsonString = "{\n";

// Variables to store unwiredlabs response
double latitude = 0.0;
double longitude = 0.0;
double accuracy = 0.0;

void setup()
{
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.println("Setup done");

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(myssid);
  WiFi.begin(myssid, mypass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(".");

  char bssid[6];
  DynamicJsonBuffer jsonBuffer;

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  if (n == 0)
  {
    Serial.println("No networks available");
  }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
  }

  // now build the jsonString...
  jsonString = "{\n";
  jsonString += "\"token\" : \"";
  jsonString += token;
  jsonString += "\",\n";
  jsonString += "\"id\" : \"saikirandevice01\",\n";
  jsonString += "\"wifi\": [\n";
  for (int j = 0; j < n; ++j)
  {
    jsonString += "{\n";
    jsonString += "\"bssid\" : \"";
    jsonString += (WiFi.BSSIDstr(j));
    jsonString += "\",\n";
    jsonString += "\"signal\": ";
    jsonString += WiFi.RSSI(j);
    jsonString += "\n";
    if (j < n - 1)
    {
      jsonString += "},\n";
    }
    else
    {
      jsonString += "}\n";
    }
  }
  jsonString += ("]\n");
  jsonString += ("}\n");
  Serial.println(jsonString);

  WiFiClientSecure client;
  client.setFingerprint(fingerprint);

  // Connect to the client and make the api call
  Serial.println("Requesting URL: https://" + (String)host + endpoint);
  if (!client.connect(host, httpsPort))
  {
    Serial.println("Connection failed!\r\n");
  }

  if (client.verify(fingerprint, host))
  {
    Serial.println("Certificate fingerprints match.\r\n");
  }
  else
  {
    Serial.println("Certificate fingerprints don't match.\r\n");
  }

  // Construct headers and post body manually
  String http_request = "POST" + endpoint +
                        "/Messages HTTP/1.1\r\n" +
                        "Host: " + host + "\r\n" +
                        "Authorization: Bearer " + token + "\r\n" +
                        "Cache-control: no-cache\r\n" +
                        "Content-Type: " + "application/json\r\n" +
                        "Content-Length: " + jsonString.length() + "\r\n" +
                        "Connection: close\r\n" +
                        "\r\n" + jsonString + "\r\n";

  Serial.println("Sending http POST: \r\n" + http_request);
  client.println(http_request);

  // Read and parse all the lines of the reply from server
  Serial.println("request sent");
  while (client.connected())
  {
    String line = client.readStringUntil('\n');
    Serial.println(line);
    JsonObject &root = jsonBuffer.parseObject(line);
    if (root.success())
    {
      latitude = root["lat"];
      longitude = root["lon"];
      accuracy = root["accuracy"];

      Serial.println();
      Serial.print("Latitude = ");
      Serial.println(latitude, 6);
      Serial.print("Longitude = ");
      Serial.println(longitude, 6);
      Serial.print("Accuracy = ");
      Serial.println(accuracy);
    }
  }

  Serial.println("closing connection");
  Serial.println();
  client.stop();
}

void loop()
{
}