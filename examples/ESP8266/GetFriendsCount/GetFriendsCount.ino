/*********************************************************************
 *  Get your Facebook friends count  *
 *                                                                   *
 *  By Brian Lough                                                   *
 *  https://www.youtube.com/channel/UCezJOfu7OtqGzd5xrP3q6WA         *
 *********************************************************************/

#include <FacebookApi.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <ArduinoJson.h> // This Sketch doesn't technically need this, but the library does so it must be installed.

//------- Replace the following! ------
char ssid[] = "xxx";       // your network SSID (name)
char password[] = "yyyy";  // your network key

// You will need to generate an access token
// It needs to be a key created from the account you want to check the friends count on.
// Make sure this request works using GraphApiExplorer Opiton here first:
// https://developers.facebook.com/docs/graph-api/reference/user/friends/
#define FACEBOOK_ACCESS_TOKEN "BIG_LONG_ACCESS_TOKEN"


WiFiClientSecure client;
FacebookApi api(client, FACEBOOK_ACCESS_TOKEN);

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


}

void loop() {

  if (millis() > api_due_time)  {
    int friendsCount = api.getTotalFriends();
    if(friendsCount >= 0)
    {
      Serial.print("Facebook Friends: ");
      Serial.println(friendsCount);
    } else {
      Serial.println("Error getting friends count");
    }
    
    api_due_time = millis() + api_delay;
  }
}
