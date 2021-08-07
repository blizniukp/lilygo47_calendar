#include <Arduino.h>

#include "config.h"
#include <WiFiClientSecure.h>

String httpsGet(void)
{
  char redirectUrl[500] = {0};
  bool gotRecirectUrl = false;
  String bodyResponse = "";

  WiFiClientSecure client;

  client.setInsecure();

  Serial.println("\nStarting connection to server...");
  if (!client.connect(HOST, HTTPS_PORT))
  {
    Serial.println("Connection failed!");
    return "";
  }

  Serial.println("Connected to server!");
  client.printf("GET %s HTTP/1.0\n", URL);
  client.println("Host: script.google.com");
  client.println("Connection: close");
  client.println();

  while (client.connected())
  {
    String line = client.readStringUntil('\n');
    if (line == "\r")
    {
      Serial.println("headers received");
      break;
    }
    if (line.startsWith("Location:"))
    {
      memcpy(redirectUrl, (line.c_str()) + 10, 500);
      gotRecirectUrl = true;
      break;
    }
  }

  client.stop();

  if (!gotRecirectUrl)
    return "";

  if (!client.connect("script.googleusercontent.com", HTTPS_PORT))
  {
    Serial.println("Connection failed!");
    return "";
  }

  Serial.println("Goto redirected URL");
  client.printf("GET %s HTTP/1.0\n", redirectUrl);
  client.println("Host: script.googleusercontent.com");
  client.println("Connection: close");
  client.println();

  while (client.connected())
  {
    String line = client.readStringUntil('\n');
    if (line == "\r")
    {
      Serial.println("headers received");
      break;
    }
  }

  while (client.available())
  {
    bodyResponse += client.readString();
  }

  client.stop();
  return bodyResponse;
}
