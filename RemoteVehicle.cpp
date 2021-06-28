#include "RemoteVehicle.h"

RemoteVehicle::RemoteVehicle(char *wifi_ssid, char *wifi_pass) {
    _wifi_ssid = wifi_ssid;
    _wifi_pass = wifi_pass;
}

void RemoteVehicle::setup_broadcast_ip(IPAddress ip, IPAddress mask) {
    for (uint8_t i = 0; i < 4; i++) {
        if (mask[i] == 255)
            broadcastIP[i] = ip[i];
        else if (mask[i] == 0)
            broadcastIP[i] = 255;
        else
            broadcastIP[i] = ~mask[i];
    }
}

void RemoteVehicle::clear_buffers() {
    tcp_in_buffer[0] = '\0';
    tcp_out_buffer[0] = '\0';
}

void RemoteVehicle::handle_heartbeat() {
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
            controladora.write(HEADER_PACKER_FORCE_CONNECTION_END);
            disconnect();
        }
    }
}

void RemoteVehicle::initial_setup() {
    current_status = STATUS_BOOTING_UP;
    WiFi.begin(_wifi_ssid, _wifi_pass);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    setup_broadcast_ip(WiFi.localIP(), WiFi.subnetMask());

    Serial.println("initializing udp server...");
    if (!Udp.begin(communication_udp_port)) {
        Serial.println("failed to initialize udp.");
        while (1);
    }

    Serial.println("initializing tcp server...");
    server = WiFiServer(communication_tcp_port);
    img_server = WiFiServer(communication_img_port);
    server.begin();
    img_server.begin();
    
    Serial.println("setting up camera...");
    camera.setup_camera();
            
    current_status = STATUS_AWAITING_TCP_CONNECTION_REQUEST;
    Serial.println("Initialization done!, broadcasting to network...");
}

void RemoteVehicle::disconnect() {
    current_status = STATUS_AWAITING_TCP_CONNECTION_REQUEST;
    last_sent_heartbeat = 0;
    last_recieved_heartbeat = 0;
    controladora.stop();
    controladora_img.stop();
    Serial.println("Disconnected, broadcasting to network...");
}

void RemoteVehicle::take_and_send_image() {
    if (current_status != STATUS_CONNECTED_TO_CONTROLLER) return;

    camera_fb_t *camera_image = camera.get_gamera_fb();
    Serial.print("Sending ");
    Serial.print(camera_image->len);
    Serial.print(" bytes ");
    Serial.print(camera_image->width);
    Serial.print("x");
    Serial.println(camera_image->height);

    sprintf(tcp_out_buffer, "%s%6d", HEADER_PACKET_CAMERA_DATA, camera_image->len);
    controladora_img.write(tcp_out_buffer);
    controladora_img.write(camera_image->buf, camera_image->len);

    camera.return_camera_fb();
}

void RemoteVehicle::main_loop() {
    switch (current_status) {
        case STATUS_AWAITING_TCP_CONNECTION_REQUEST: {

            // send broadcast every 3s
            if (millis() - last_sent_broadcast >= 3000) {
                last_sent_broadcast = millis();
                Udp.beginPacket(broadcastIP, communication_udp_port);
                Udp.print(HEADER_PACKET_BROADCAST);
                Udp.endPacket();
            }

            controladora = server.available();
            controladora_img = img_server.available();

            if (controladora) {
                if (controladora.connected()) {
            
                    uint8_t s = 0;
                    while (controladora.available()) tcp_in_buffer[s++] = controladora.read();
                    Packet p = parse_packet(tcp_in_buffer, s);
            
                    if (p.packet_type == PACKET_REQUESTING_CONNECTION) {
                        controllerIP = controladora.remoteIP();
                        Serial.print("Recieved TCP request from ");
                        Serial.print(controllerIP);
                        Serial.println(", switching to slave mode");
                        current_status = STATUS_CONNECTED_TO_CONTROLLER;
                        controladora.write(HEADER_PACKET_CONNECTION_ACCEPTED);
                    } else {
                        Serial.print("unrecognized packet: ");
                        Serial.println(s);
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

                if (s == 0) return;
        
                Packet p = parse_packet(tcp_in_buffer, s);

                switch (p.packet_type) {
                    case PACKET_HEARTBEAT:
                        last_recieved_heartbeat = millis();
                        break;
                    case PACKET_REQUEST_CAMERA_IMAGE:
                        take_and_send_image();
                        break;
                    case PACKET_REQUEST_CONNECTION_END:
                        Serial.println("connection end requested by remote host, terminating...");
                        controladora.write(HEADER_PACKET_CONNECTION_ENDED);
                        disconnect();
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
