#ifndef i_RemoteVehicle
#define i_RemoteVehicle

class RemoteVehicle;

#include "VehicleCamera.h"
#include "VehicleCommunication.h"
#include "PacketParser.h"

class RemoteVehicle {
    private:
        char *_wifi_ssid, *_wifi_pass;

        VehicleCamera camera;
        PacketParser parser;
        VehicleCommunication *com;

    public:
        RemoteVehicle(char *wifi_ssid, char *wifi_pass);

        void initial_setup();
        void take_and_send_image();
        void main_loop();

};

#endif
