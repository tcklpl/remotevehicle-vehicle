#ifndef i_RemoteVehicle
#define i_RemoteVehicle

#include <stdint.h>

class RemoteVehicle;

/**
 * This struct is meant to be used to configure the vehicle and/or to change
 * default behaviour, the configurations are:
 * 
 * wssid        REQUIRED    The WiFi network's SSID.
 * wpass        REQUIRED    The WiFi network's password.
 * 
 * wtimeout                 Time in which the vehicle will try to connect to
 *                          the WiFi netowrk, if the connection time exceeds
 *                          this value, the vehicle will restart.
 * 
 * should_log               0 for false or 1 for true, if the vehicle should
 *                          log to the Serial.
 * 
 * log_severity             Which log severity will be shown. Possible values
 *                          are defined in "Logger.h"
 *                          
 * cmd_port                 TCP Port for sending and recieving commands.
 * udp_port                 UDP Port for bradcasting.
 * img_port                 TCP Port for sending images to the controller.
 */
struct vehicleinfo_t {
    // WiFi related configurations
    char *wssid, *wpass;
    uint16_t wtimeout = 10000;
    // Logging related configurations
    uint8_t should_log = 0, log_severity = 3;
    // TCP/UDP related configurations
    uint16_t cmd_port = 6887, udp_port = 6888, img_port = 6889;
};


#include "VehicleCamera.h"
#include "VehicleCommunication.h"
#include "PacketParser.h"
#include "Logger.h"

class RemoteVehicle {
    private:
        vehicleinfo_t cinfo;

        VehicleCamera camera;
        PacketParser parser;
        VehicleCommunication *com;

    public:
        RemoteVehicle(vehicleinfo_t c_info);

        void initial_setup();
        void take_and_send_image();
        void main_loop();

        vehicleinfo_t get_cinfo();

};

#endif
