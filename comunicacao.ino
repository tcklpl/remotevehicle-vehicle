#include "wifilogin.h"
#include "RemoteVehicle.h"

vehicleinfo_t cinfo; 
RemoteVehicle *vehicle;

void setup() {
    Serial.begin(115200);
    
    cinfo.wssid = WIFI_LOGIN_SSID;
    cinfo.wpass = WIFI_LOGIN_PASS;
    cinfo.should_log = 1;
    cinfo.log_severity = LOG_LVL_INFO;

    vehicle = new RemoteVehicle(cinfo);
    
    vehicle->initial_setup();
}

void loop() {
    vehicle->main_loop();
}
