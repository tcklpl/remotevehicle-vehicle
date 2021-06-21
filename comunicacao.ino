#include <WiFi.h>
#include <WiFiUdp.h>
#include "wifilogin.h"

const char* ssid = WIFI_LOGIN_SSID;
const char* password = WIFI_LOGIN_PASS;

const int communication_port = 6887;
WiFiUDP Udp;
WiFiServer server(communication_port);
WiFiClient controladora;
char tcp_in_buffer[10], tcp_out_buffer[10];

#define STATUS_BOOTING_UP                       0
#define STATUS_AWAITING_TCP_CONNECTION_REQUEST  1
#define STATUS_CONNECTED_TO_CONTROLLER          2

uint8_t current_status;
unsigned long last_sent_broadcast = 0;
IPAddress broadcastIP;

uint16_t controller_generated_id = 0;
unsigned long last_sent_heartbeat = 0;
unsigned long last_recieved_heartbeat = 0;

void setup_broadcast_ip(IPAddress ip, IPAddress mask) {
  for (uint8_t i = 0; i < 4; i++) {
    if (mask[i] == 255)
      broadcastIP[i] = ip[i];
    else if (mask[i] == 0)
      broadcastIP[i] = 255;
    else
      broadcastIP[i] = ~mask[i];
  }
}

void clear_buffers() {
  tcp_in_buffer[0] = '\0';
  tcp_out_buffer[0] = '\0';
}

void setup() {
  current_status = STATUS_BOOTING_UP;
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  setup_broadcast_ip(WiFi.localIP(), WiFi.subnetMask());
  
  Serial.println("");
  Serial.print("WiFi connected at ");
  Serial.print(WiFi.localIP());
  Serial.print(" mask: ");
  Serial.print(WiFi.subnetMask());
  Serial.print(" broadcast: ");
  Serial.println(broadcastIP);
  
  Serial.println("initializing udp server...");
  if (!Udp.begin(communication_port)) {
    Serial.println("failed to initialize udp.");
    while(1);
  }

  Serial.println("initializing tcp server...");
  server.begin();
  
  current_status = STATUS_AWAITING_TCP_CONNECTION_REQUEST;
  randomSeed(millis());
  Serial.println("Initialization done!, broadcasting to network...");
}

void disconnect() {
  current_status = STATUS_AWAITING_TCP_CONNECTION_REQUEST;
  last_sent_heartbeat = 0;
  last_recieved_heartbeat = 0;
  controller_generated_id = 0;
  Serial.println("Disconnected, broadcasting to network...");
}

void handle_heartbeat() {
  if (current_status != STATUS_CONNECTED_TO_CONTROLLER) return;
  
  if (last_sent_heartbeat == 0 || last_recieved_heartbeat == 0) {
    last_sent_heartbeat = millis();
    last_recieved_heartbeat = last_sent_heartbeat;
  } else {
    // send hearbeats every 1s
    if (millis() - last_sent_heartbeat >= 1000) {
      last_sent_heartbeat = millis();
      controladora.write("CFHB");
    }
    // end connection if there was not a heartbeat in the last 5s
    if (millis() - last_recieved_heartbeat >= 5000) {
      Serial.println("Didn't recieve any heartbeat in the last  5s, disconnecting...");
      disconnect();
    }
  }
}

void loop() {
  switch (current_status) {
    case STATUS_AWAITING_TCP_CONNECTION_REQUEST: {

      // send broadcast every 2s
      if (millis() - last_sent_broadcast >= 2000) {
        Udp.beginPacket(broadcastIP, communication_port);
        Udp.print("CFBC");
        Udp.endPacket();
      }
      
      controladora = server.available();
      
      if (controladora) {
        if (controladora.connected()) {
          uint8_t s = 0;
          while (controladora.available()) tcp_in_buffer[s++] = controladora.read();
          if (strcmp(tcp_in_buffer, "CFRC") == 0) {
            // check if there is no controller connected at the moment
            if (controller_generated_id == 0) {
              Serial.println("Recieved TCP request, switching to slave mode");
              current_status = STATUS_CONNECTED_TO_CONTROLLER;
              
              controller_generated_id = random(20000, 65536);
              strcat(tcp_out_buffer, "CFID");
              strcat(tcp_out_buffer, controller_generated_id);
              controladora.write(tcp_out_buffer);
              
              controladora.write("CFCK");
            } else {
              controladora.write("ER01");
            }
          }
        }
      }
      break;
    }
    case STATUS_CONNECTED_TO_CONTROLLER: {
      handle_heartbeat();
      if (controladora.connected()) {
        uint8_t s = 0;
        while (controladora.available()) tcp_in_buffer[s++] = controladora.read();
        if (strcmp(tcp_in_buffer, "CFHB") == 0) {
          last_recieved_heartbeat = millis();
        }
      }
      break;
    }
    default: break;
  }
  
}
