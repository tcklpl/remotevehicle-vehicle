#include "RemoteVehicle.h"

vehicleinfo_t cinfo; 
RemoteVehicle *vehicle;

void setup() {
    cinfo.wssid = "Your WiFi SSID";
    cinfo.wpass = "Your WiFi Password";
    cinfo.motor_la = 12;
    cinfo.motor_lb = 13;
    cinfo.motor_ra = 15;
    cinfo.motor_rb = 14;

    vehicle = new RemoteVehicle(cinfo);
    
    vehicle->initialSetup();
}

void loop() {
    vehicle->mainLoop();
}
