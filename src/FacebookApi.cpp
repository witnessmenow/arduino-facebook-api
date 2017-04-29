/*
   Copyright (c) 2017 Brian Lough. All right reserved.

   FacebookApi - An Arduino wrapper for the Facebook API

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include "FacebookApi.h"

FacebookApi::FacebookApi(Client &client, String accessToken)	{
	this->client = &client;
  _accessToken = accessToken;
}

String FacebookApi::sendGetToFacebook(String command) {
	String body;
	String headers;
	bool finishedHeaders = false;
	bool currentLineIsBlank = true;
	unsigned long now;
	bool avail;
	// Connect with facebook api over ssl
	if (client->connect(FB_API_HOST, FB_API_SSL_PORT)) {
		//Serial.println(".... connected to server");
		String a="";
		char c;
		int ch_count=0;
		client->println("GET " + command +  " HTTP/1.1");
		client->println("Host: " FB_API_HOST);
		client->println("User-Agent: arduino/1.0.0");
		client->println("");
		now=millis();
		avail=false;
		while (millis() - now < FB_API_TIMEOUT) {
			while (client->available()) {

				// Allow body to be parsed before finishing
				avail = finishedHeaders;
				char c = client->read();
				// Serial.write(c);

				if(!finishedHeaders){
					if (currentLineIsBlank && c == '\n') {
						finishedHeaders = true;
						//Serial.println(headers);
					}
					else{
						headers = headers + c;

					}
				} else {
					if (ch_count < maxMessageLength)  {
						body=body+c;
						ch_count++;
					}
				}

				if (c == '\n') {
					currentLineIsBlank = true;
				}else if (c != '\r') {
					currentLineIsBlank = false;
				}
			}
			if (avail) {
        // For some reason the body is coming back with a number on the outside of the start and end of it, we only want the JsonObject
        int firstJsonCharacterIndex = body.indexOf("{");
        int lastJsonCharacterIndex = body.lastIndexOf("}");
        if(firstJsonCharacterIndex != -1 && lastJsonCharacterIndex != -1){
          body = body.substring(firstJsonCharacterIndex, lastJsonCharacterIndex + 1);
        }
				// Serial.println("Body:");
				// Serial.println(body);
				// Serial.println("END");
				break;
			}
		}
	}

	return body;
}

String FacebookApi::getFriends(){
	String command="/v2.9/me/friends?access_token="+_accessToken;
	//Serial.println(command);
	return sendGetToFacebook(command);  //recieve reply from facebook
}

String FacebookApi::extendAccessToken(String appId, String appSecret){
	String command="/oauth/access_token?client_id="+ appId +"&client_secret=" + appSecret + "&grant_type=fb_exchange_token&fb_exchange_token="+_accessToken;
	//Serial.println(command);
	String response = sendGetToFacebook(command);
  DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(response);
  if (root.success()) {
		if (root.containsKey("access_token")) {
			Serial.println("Updated Token");
			_accessToken = root["access_token"].as<String>();
			return _accessToken;
		} else {
      Serial.println("JSON respnse was not as expected");
    }
  } else {
    Serial.println("Failed to parse JSON");
  }
	return "";
}

int FacebookApi::getTotalFriends(){
  String response = getFriends();
  DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(response);
  if (root.success()) {
		if (root.containsKey("summary")) {
			return root["summary"]["total_count"].as<int>();
		} else {
      Serial.println("JSON respnse was not as expected");
    }
  } else {
    Serial.println("Failed to parse JSON");
  }

  return -1;
}
