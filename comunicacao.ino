#include "wifilogin.h"
#include "RemoteVehicle.h"

vehicleinfo_t cinfo; 
RemoteVehicle *vehicle;

void setup() {
    Serial.begin(115200);
    cinfo.wssid = WIFI_LOGIN_SSID;
    cinfo.wpass = WIFI_LOGIN_PASS;
    cinfo.should_log = true;
    cinfo.log_severity = 0;
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
