/*********************************************************************
 *  Get the fan count of an Facebook object. This could for instance *
 *  be a page.                                                       *
 *  https://developers.facebook.com/docs/graph-api/reference/page    *
 *                                                                   *
 *  By Brian Lough                                                   *
 *  https://www.youtube.com/channel/UCezJOfu7OtqGzd5xrP3q6WA         *
 *********************************************************************/

#include <FacebookApi.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <ArduinoJson.h> // This Sketch doesn't technically need this, but the library does so it must be installed.

// Include for SPIFFs
#include "FS.h"

//------- Replace the following! ------
char ssid[] = "xxx";       // your network SSID (name)
char password[] = "yyyy";  // your network key

// You will need to generate an access token
// It needs to be a key created from the account you want to check the friends count on.
// Make sure this request works using GraphApiExplorer Option here first:
// https://developers.facebook.com/docs/graph-api/reference/user/friends/

// These tokens expire so this one will be used once and use the
// extendAccessToken method to get a renwed one that lasts 60 days
String FACEBOOK_ACCESS_TOKEN = "";    // not needed for the page fan count
String FACEBOOK_APP_ID = "ENTER_YOUR_APP_ID";
String FACEBOOK_APP_SECRET = "ENTER_YOUR_APP_SECRET";

WiFiClientSecure client;
FacebookApi *api;

unsigned long api_delay = 5 * 60000; //time between api requests (5mins)
unsigned long api_due_time;

void setup() {
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  // loading the saved config if there is a new key saved there
  loadConfig();
  api = new FacebookApi(client, FACEBOOK_APP_ID, FACEBOOK_APP_SECRET, FACEBOOK_ACCESS_TOKEN);
}

bool loadConfig() {
  File configFile = SPIFFS.open("/counterConfig.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }

  FACEBOOK_ACCESS_TOKEN = json["facebookToken"].as<String>();
  FACEBOOK_APP_ID = json["facebookAppId"].as<String>();
  FACEBOOK_APP_SECRET = json["facebookAppSecret"].as<String>();
  return true;
}

bool saveConfig() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["facebookToken"] = FACEBOOK_ACCESS_TOKEN;
  json["facebookAppId"] = FACEBOOK_APP_ID;
  json["facebookAppSecret"] = FACEBOOK_APP_SECRET;

  File configFile = SPIFFS.open("/counterConfig.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;
}

void loop() {
  if (millis() > api_due_time) {
    int pageLikes = api->getPageFanCount("59520506051");
    if(pageLikes >= 0) {
      Serial.print("Facebook page likes: ");
      Serial.println(pageLikes);
    } else {
      Serial.println("Error getting likes");
    }
    api_due_time = millis() + api_delay;
  }
}
