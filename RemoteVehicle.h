#pragma once

#include <WiFi.h>
#include <WiFiUdp.h>
#include "VehicleCamera.h"
#include "packets.h"

#define STATUS_BOOTING_UP                       0
#define STATUS_AWAITING_TCP_CONNECTION_REQUEST  1
#define STATUS_CONNECTED_TO_CONTROLLER          2

const uint16_t communication_tcp_port = 6887;
const uint16_t communication_udp_port = 6888;
const uint16_t communication_img_port = 6889;

class RemoteVehicle {
    private:
        char *_wifi_ssid, *_wifi_pass;

        WiFiUDP Udp;
        WiFiServer server, img_server;
        WiFiClient controladora, controladora_img;
        char tcp_in_buffer[20], tcp_out_buffer[20];

        VehicleCamera camera;

        uint8_t current_status;
        unsigned long last_sent_broadcast = 0;
        IPAddress broadcastIP, controllerIP;

        unsigned long last_sent_heartbeat = 0;
        unsigned long last_recieved_heartbeat = 0;

        void setup_broadcast_ip(IPAddress ip, IPAddress mask);

        void clear_buffers();

        void handle_heartbeat();

    public:
        RemoteVehicle(char *wifi_ssid, char *wifi_pass);

        void initial_setup();

        void disconnect();

        void take_and_send_image();

        void main_loop();

};
