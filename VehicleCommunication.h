#ifndef i_VehicleCommunication
#define i_VehicleCommunication

class VehicleCommunication;

#include <WiFi.h>
#include <WiFiUdp.h>
#include "Logger.h"
#include "RemoteVehicle.h"
#include "Packet.h"
#include "PacketParser.h"
#include "Listener.h"
#include "Callback.h"
#include "esp_camera.h"

#define STATUS_BOOTING_UP                       0
#define STATUS_AWAITING_TCP_CONNECTION_REQUEST  1
#define STATUS_CONNECTING_TO_IMG                2
#define STATUS_CONNECTED                        3

class VehicleCommunication : public Listener {
    private:
        RemoteVehicle *remote_vehicle;
        
        WiFiUDP udp_server;
        WiFiServer cmd_server, img_server;
        WiFiClient cmd_client, img_client;
        IPAddress broadcast_ip, client_ip;
        
        char tcp_in_buffer[20], tcp_out_buffer[20];

        uint8_t current_status;
        unsigned long last_sent_broadcast = 0;

        unsigned long last_sent_heartbeat = 0;
        unsigned long last_recieved_heartbeat = 0;

        PacketParser parser;

        void setup_broadcast_ip(IPAddress ip, IPAddress mask);
        void clear_buffers();
        void handle_heartbeat();
        void disconnect();

    public:
        VehicleCommunication(RemoteVehicle *vehicle, vehicleinfo_t cinfo);
        
        void loop();
        void send_cmd(char *bytes);
        void send_img(char *header, uint8_t *bytes, int len);
        uint8_t is_connected();
        
        // callbacks
        void cb_cmd_connect(Packet p);
        void cb_cam_connect(Packet p);
        void cb_heartbeat(Packet p);
        void cb_req_cam_img(Packet p);
        void cb_req_con_end(Packet p);
        void cb_req_cam_res(Packet p);
        void cb_req_info_cam_res(Packet p);
};

#endif
