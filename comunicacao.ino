#include <WiFi.h>
#include <WiFiUdp.h>
#include "wifilogin.h"
#include "camera.h"
#include "packets.h"

const char* ssid = WIFI_LOGIN_SSID;
const char* password = WIFI_LOGIN_PASS;

const uint16_t communication_tcp_port = 6887;
const uint16_t communication_udp_port = 6888;
WiFiUDP Udp;
WiFiServer server(communication_tcp_port);
WiFiClient controladora;
char tcp_in_buffer[20], tcp_out_buffer[20];

#define STATUS_BOOTING_UP                       0
#define STATUS_AWAITING_TCP_CONNECTION_REQUEST  1
#define STATUS_CONNECTED_TO_CONTROLLER          2

uint8_t current_status;
unsigned long last_sent_broadcast = 0;
IPAddress broadcastIP;

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
    if (!Udp.begin(communication_udp_port)) {
        Serial.println("failed to initialize udp.");
        while (1);
    }

    Serial.println("initializing tcp server...");
    server.begin();

    Serial.println("setting up camera...");
    setup_camera(FRAMESIZE_HD);

    current_status = STATUS_AWAITING_TCP_CONNECTION_REQUEST;
    randomSeed(millis());
    Serial.println("Initialization done!, broadcasting to network...");
}

void disconnect() {
    current_status = STATUS_AWAITING_TCP_CONNECTION_REQUEST;
    last_sent_heartbeat = 0;
    last_recieved_heartbeat = 0;
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

void take_and_send_image() {
    if (current_status != STATUS_CONNECTED_TO_CONTROLLER) return;

    take_photo();
    Serial.print("Sending ");
    Serial.print(camera_image->len);
    Serial.print(" bytes ");
    Serial.print(camera_image->width);
    Serial.print("x");
    Serial.println(camera_image->height);

    sprintf(tcp_out_buffer, "DTDC%6d", camera_image->len);
    controladora.write(tcp_out_buffer);
    controladora.write(camera_image->buf, camera_image->len);

    esp_camera_fb_return(camera_image);
}

void loop() {
    switch (current_status) {
        case STATUS_AWAITING_TCP_CONNECTION_REQUEST: {

            // send broadcast every 3s
            if (millis() - last_sent_broadcast >= 3000) {
                last_sent_broadcast = millis();
                Udp.beginPacket(broadcastIP, communication_udp_port);
                Udp.print("CFBC");
                Udp.endPacket();
            }

            controladora = server.available();

            if (controladora) {
                if (controladora.connected()) {
                    
                    uint8_t s = 0;
                    while (controladora.available()) tcp_in_buffer[s++] = controladora.read();
                    Packet p = parse_packet(tcp_in_buffer, s);
                    
                    if (p.packet_type == PACKET_REQUESTING_CONNECTION) {
                        Serial.println("Recieved TCP request, switching to slave mode");
                        current_status = STATUS_CONNECTED_TO_CONTROLLER;
                        controladora.write("CFCK");
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
                Packet p = parse_packet(tcp_in_buffer, s);

                switch (p.packet_type) {
                    case PACKET_HEARTBEAT:
                        last_recieved_heartbeat = millis();
                        break;
                    case PACKET_REQUEST_CAMERA_IMAGE:
                        take_and_send_image();
                        break;
                    default:
                        break;
                }
            }
            break;
        }
        default: break;
    }

    clear_buffers();
}
