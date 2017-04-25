/*
Copyright (c) 2017 Brian Lough. All right reserved.

FacebookApi - An Arduino wrapper for the YouTube API

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


#ifndef FacebookApi_h
#define FacebookApi_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>

#define FB_API_HOST "graph.facebook.com"
#define FB_API_SSL_PORT 443
#define FB_API_TIMEOUT 1500

curl -i -X GET \
 "https://graph.facebook.com/v2.9/me/friends?access_token=EAACEdEose0cBAF7ZCXyG15vfmXZAUBCN2raTBcfFv5GEo1fvD2Gg4zAxZB85Wg7H11xp9gjeZA4aYZAtaVq7ekFMVhLItCxRxkWiFJLKyv8IYRS1DN5ZCB5zYMrTUYMd4vYAguSdulv5EeWJCnMM1euB5YhyjwVvNjpaZCKkkAMg6oAc0GF6BXlh8mvwQwByvsZD"

class FacebookApi
{
  public:
    FacebookApi (Client &client, String accessToken);
    String sendGetToFacebook(String command);
    String getFriends();
    int getTotalFriends();

  private:
    Client *client;
    String _accessToken;
    const int maxMessageLength = 10000;
};

#endif
