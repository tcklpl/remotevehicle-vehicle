#include "wifilogin.h"
#include "RemoteVehicle.h"

RemoteVehicle vehicle(WIFI_LOGIN_SSID, WIFI_LOGIN_PASS);

void setup() {
    Serial.begin(115200);
    Serial.println();

    vehicle.initial_setup();
}

void loop() {
    vehicle.main_loop();
}
