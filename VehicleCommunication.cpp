#include "VehicleCommunication.h"

VehicleCommunication::VehicleCommunication(RemoteVehicle *vehicle, char *wifi_ssid, char *wifi_pass) {
    current_status = STATUS_BOOTING_UP;
    remote_vehicle = vehicle;

    // setup wifi
    WiFi.begin(wifi_ssid, wifi_pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    setup_broadcast_ip(WiFi.localIP(), WiFi.subnetMask());

    // initialize udp
    Serial.println("initializing udp server...");
    if (!udp_server.begin(UDP_PORT)) {
        Serial.println("failed to initialize udp.");
        while (1);
    }

    // initialize tcp
    Serial.println("initializing tcp servers...");
    cmd_server = WiFiServer(TCP_PORT);
    img_server = WiFiServer(IMG_PORT);
    cmd_server.begin();
    img_server.begin();

    parser.register_callback(new Callback<Listener>((Listener*) this, (void (Listener::*)(Packet)) &VehicleCommunication::cb_cmd_connect), PKT_REQ_CON, 1);
    parser.register_callback(new Callback<Listener>((Listener*) this, (void (Listener::*)(Packet)) &VehicleCommunication::cb_cam_connect), PKT_REQ_CAMERA_CON, 1);
    
    parser.register_callback(new Callback<Listener>((Listener*) this, (void (Listener::*)(Packet)) &VehicleCommunication::cb_heartbeat), PKT_HEARTBEAT, 1);
    parser.register_callback(new Callback<Listener>((Listener*) this, (void (Listener::*)(Packet)) &VehicleCommunication::cb_req_cam_img), PKT_REQ_CAMERA_IMAGE, 1);
    parser.register_callback(new Callback<Listener>((Listener*) this, (void (Listener::*)(Packet)) &VehicleCommunication::cb_req_con_end), PKT_REQ_CONNECTION_END, 1);
    parser.register_callback(new Callback<Listener>((Listener*) this, (void (Listener::*)(Packet)) &VehicleCommunication::cb_req_cam_res), PKT_REQ_CAMERA_RES_CHANGE, 1);
    
    current_status = STATUS_AWAITING_TCP_CONNECTION_REQUEST;
}

void VehicleCommunication::disconnect() {
    current_status = STATUS_AWAITING_TCP_CONNECTION_REQUEST;
    last_sent_heartbeat = 0;
    last_recieved_heartbeat = 0;
    cmd_client.stop();
    img_client.stop();
    Serial.println("Disconnected, broadcasting to network...");
}

uint8_t VehicleCommunication::is_connected() {
    return current_status == STATUS_CONNECTED;
}

void VehicleCommunication::setup_broadcast_ip(IPAddress ip, IPAddress mask) {
    for (uint8_t i = 0; i < 4; i++) {
        if (mask[i] == 255)
            broadcast_ip[i] = ip[i];
        else if (mask[i] == 0)
            broadcast_ip[i] = 255;
        else
            broadcast_ip[i] = ~mask[i];
    }
}

void VehicleCommunication::clear_buffers() {
    tcp_in_buffer[0] = '\0';
    tcp_out_buffer[0] = '\0';
}

void VehicleCommunication::send_cmd(char *bytes) {
    cmd_client.write(bytes);
}

void VehicleCommunication::send_img(char *header, uint8_t *bytes, int len) {
    sprintf(tcp_out_buffer, "%s%6d", header, len);
    img_client.write(tcp_out_buffer);
    img_client.write(bytes, len);
}

void VehicleCommunication::handle_heartbeat() {
    if (last_recieved_heartbeat == 0 || last_sent_heartbeat == 0) {
        last_recieved_heartbeat = millis();
        last_sent_heartbeat = millis();
    }
    if (millis() - last_sent_heartbeat >= 1000) {
        last_sent_heartbeat = millis();
        cmd_client.write(PKT_CF_ANY_HEARTBEAT);
    }
    if (millis() - last_recieved_heartbeat >= 5000) {
        Serial.println("Heart failed");
        disconnect();
    }
}

void VehicleCommunication::loop() {
    uint8_t s = 0;
    switch (current_status) {
        case STATUS_AWAITING_TCP_CONNECTION_REQUEST:
            // send broadcast every 3s
            if (millis() - last_sent_broadcast >= 3000) {
                last_sent_broadcast = millis();
                udp_server.beginPacket(broadcast_ip, UDP_PORT);
                udp_server.print(PKT_CF_SRV_BROADCAST);
                udp_server.endPacket();
            }
            cmd_client = cmd_server.available();
            break;
        case STATUS_CONNECTING_TO_IMG:
            // request camera connection every 1s
            if (millis() - last_sent_broadcast >= 1000) {
                last_sent_broadcast = millis();
                cmd_client.write(PKT_CF_SRV_REQ_CAM_CON_ATTEMPT);
            }
            img_client = img_server.available();
            if (img_client && img_client.connected()) {
                while (img_client.available()) tcp_in_buffer[s++] = img_client.read();
            }
        case STATUS_CONNECTED:
            handle_heartbeat();        
    }
    if (cmd_client) {
        if (cmd_client.connected()) {
            while (cmd_client.available()) tcp_in_buffer[s++] = cmd_client.read();
        }
    }
    // parse any recieved packet
    if (s > 0)
        parser.handle_packet(tcp_in_buffer, s);
    clear_buffers();
}

/**
 *  CALLBACK FUNCTIONS
 */

void VehicleCommunication::cb_cmd_connect(Packet p) {
    client_ip = cmd_client.remoteIP();
    Serial.print("Recieved TCP request from ");
    Serial.println(client_ip);
    cmd_client.write(PKT_CF_SRV_CON_OK);
    current_status = STATUS_CONNECTING_TO_IMG;
}

void VehicleCommunication::cb_cam_connect(Packet p) {
    Serial.println("Recieved TCP IMG request");
    img_client.write(PKT_CF_SRV_CAM_CON_OK);
    current_status = STATUS_CONNECTED;
}

void VehicleCommunication::cb_heartbeat(Packet p) {
    last_recieved_heartbeat = millis();
}

void VehicleCommunication::cb_req_cam_img(Packet p) {
    remote_vehicle->take_and_send_image();
}

void VehicleCommunication::cb_req_con_end(Packet p) {
    Serial.println("connection end requested by remote host, terminating...");
    cmd_client.write(PKT_CF_SRV_CON_END);
    disconnect();    
}

void VehicleCommunication::cb_req_cam_res(Packet p) {
    Serial.println("controller request camera image resolution change.");
}
