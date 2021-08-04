#include "VehicleCommunication.h"
#include "Esp.h"

VehicleCommunication::VehicleCommunication(RemoteVehicle *vehicle, vehicleinfo_t cinfo) {
    current_status = STATUS_BOOTING_UP;
    remote_vehicle = vehicle;

    // setup wifi
    logger.info("Connecting to WiFi");
    WiFi.begin(cinfo.wssid, cinfo.wpass);
    // using this variable because it will have no use until later
    last_sent_broadcast = millis();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        // restart if the wifi didn't connect within 10s
        if (millis() - last_sent_broadcast >= cinfo.wtimeout) {
            logger.severe("Didn't connect to the WiFi within the specified time, restarting...");
            ESP.restart();
        }
    }
    setup_broadcast_ip(WiFi.localIP(), WiFi.subnetMask());
    logger.info("WiFi Connected!");

    // initialize udp
    logger.info("Starting UDP server");
    if (!udp_server.begin(cinfo.udp_port)) {
        logger.severe("Failed to initialize UDP, restarting...");
        ESP.restart();
    }

    // initialize tcp
    logger.info("Starting TCP servers");
    cmd_server = WiFiServer(cinfo.cmd_port);
    img_server = WiFiServer(cinfo.img_port);
    cmd_server.begin();
    img_server.begin();

    parser.register_callback(new Callback<Listener>((Listener*) this, (void (Listener::*)(Packet)) &VehicleCommunication::cb_cmd_connect), PKT_REQ_CON, 1);
    parser.register_callback(new Callback<Listener>((Listener*) this, (void (Listener::*)(Packet)) &VehicleCommunication::cb_cam_connect), PKT_REQ_CAMERA_CON, 1);
    
    parser.register_callback(new Callback<Listener>((Listener*) this, (void (Listener::*)(Packet)) &VehicleCommunication::cb_heartbeat), PKT_HEARTBEAT, 1);
    parser.register_callback(new Callback<Listener>((Listener*) this, (void (Listener::*)(Packet)) &VehicleCommunication::cb_req_cam_img), PKT_REQ_CAMERA_IMAGE, 1);
    parser.register_callback(new Callback<Listener>((Listener*) this, (void (Listener::*)(Packet)) &VehicleCommunication::cb_req_con_end), PKT_REQ_CONNECTION_END, 1);
    parser.register_callback(new Callback<Listener>((Listener*) this, (void (Listener::*)(Packet)) &VehicleCommunication::cb_req_cam_res), PKT_REQ_CAMERA_RES_CHANGE, 1);
    parser.register_callback(new Callback<Listener>((Listener*) this, (void (Listener::*)(Packet)) &VehicleCommunication::cb_req_info_cam_res), PKT_REQ_CAMERA_RES, 1);
    
    current_status = STATUS_AWAITING_TCP_CONNECTION_REQUEST;
}

void VehicleCommunication::disconnect() {
    current_status = STATUS_AWAITING_TCP_CONNECTION_REQUEST;
    last_sent_heartbeat = 0;
    last_recieved_heartbeat = 0;
    cmd_client.stop();
    img_client.stop();
    logger.info("Disconnected, returning to network broadcast");
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
    logger.info("Image sent");
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
        logger.warn("Heart failed");
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
                udp_server.beginPacket(broadcast_ip, remote_vehicle->get_cinfo().udp_port);
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
    if (s > 0) {
        parser.handle_packet(tcp_in_buffer, s);
    }
        
        
    clear_buffers();
}

/**
 *  CALLBACK FUNCTIONS
 */

void VehicleCommunication::cb_cmd_connect(Packet p) {
    client_ip = cmd_client.remoteIP();
    logger.info("Recieved TCP request");
    cmd_client.write(PKT_CF_SRV_CON_OK);
    current_status = STATUS_CONNECTING_TO_IMG;
}

void VehicleCommunication::cb_cam_connect(Packet p) {
    logger.info("Recieved TCP IMG request");
    img_client.write(PKT_CF_SRV_CAM_CON_OK);
    current_status = STATUS_CONNECTED;
    logger.info("Connected to remote controller!");
}

void VehicleCommunication::cb_heartbeat(Packet p) {
    last_recieved_heartbeat = millis();
}

void VehicleCommunication::cb_req_cam_img(Packet p) {
    remote_vehicle->take_and_send_image();
}

void VehicleCommunication::cb_req_con_end(Packet p) {
    logger.info("Connection end requested by remote host, terminating");
    cmd_client.write(PKT_CF_SRV_CON_END);
    disconnect();    
}

void VehicleCommunication::cb_req_cam_res(Packet p) {
    logger.info("Controller request camera image resolution change");
    if (remote_vehicle->change_canera_resolution(p.get_info_as_uint8())) {
        cmd_client.write(PKT_CF_SRV_CAM_RES_OK);
        logger.info("Camera resolution changed!");
    } else {
        cmd_client.write(PKT_ERR_CAM_RES);
        logger.error("Could not change camera resolution");
    }
}

void VehicleCommunication::cb_req_info_cam_res(Packet p) {
    logger.info("Controller request camera resolution");
    sprintf(tcp_out_buffer, "%s%2d", PKT_DT_SRV_CAM_RES, remote_vehicle->get_camera_resolution());
    img_client.write(tcp_out_buffer);
}
