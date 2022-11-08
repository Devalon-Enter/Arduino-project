#include <Ultrasonic.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <time.h>
#include <stdio.h>

#include "arduino_secrets.h"

//==============================================================================================

Ultrasonic ultrasonic(7);

// + "&"+

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

long measuredLength;
String schrankAuf = "2022-11-04 12:00:00";
String schrankZu = "2022-11-04 13:00:00";
int schrankOffenDauer = 60;

IPAddress server_addr(185,27,134,117);
WiFiClient client;


String http_url = "POST /http/insertData.php";
String http_request;
int http_ServerStatus = false;

//===============================================================================================

void httppost_request(long measuredLength) {
  client.stop();

  if (!client.connected()) {
    client.connect(server_addr, 80);
    if (!client.connected()) {
      http_ServerStatus = false;
      Serial.println("The server is not connected");
    } else {
      http_ServerStatus = true;
      Serial.println("The server is connected");
    }
  }
  if (http_ServerStatus) {
    http_request = http_url+ schrankAuf+ "&"+ schrankZu+ "&"+ schrankOffenDauer;
    client.println(http_request);
    Serial.println("Url: \n");
    Serial.println(http_request); 
  }
}

// void getTime() {
//   time_t rawtime;
//   struct tm * timeinfo;

//   time(&rawtime);
//   timeinfo = localtime(&rawtime);
//   Serial.println("Current local time and date: %s", asctime(timeinfo));
// }


void getMeasuredLength() {
  // put your main code here, to run repeatedly:
  Serial.println("The measured distance");
  measuredLength = ultrasonic.MeasureInCentimeters();
  Serial.println(measuredLength);
  delay(1000);

  if (measuredLength >= 150) {
    httppost_request(measuredLength);
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  while(!Serial) {
    ;
  }

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("communication with WiFi module failed");
    while (true);
  }

  while (status != WL_CONNECTED) {
    Serial.println("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);

    status = WiFi.begin(ssid, pass);

    delay(10000);
  }

  Serial.print("You're connected to the network");
  printCurrentNet();

}

void loop() {
  delay(10000);
  printCurrentNet();

  getMeasuredLength();
  // getTime();
}


//==============================================================================================


void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}
