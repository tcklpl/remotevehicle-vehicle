#include "RemoteVehicle.h"

RemoteVehicle::RemoteVehicle(char *wifi_ssid, char *wifi_pass) {
    _wifi_ssid = wifi_ssid;
    _wifi_pass = wifi_pass;
}

void RemoteVehicle::initial_setup() {

    com = new VehicleCommunication(this, _wifi_ssid, _wifi_pass);
    
    Serial.println("setting up camera...");
    camera.setup_camera();

    Serial.println("Initialization done!, broadcasting to network...");
}

void RemoteVehicle::take_and_send_image() {
    if (!com->is_connected()) return;

    camera_fb_t *camera_image = camera.get_gamera_fb();
    Serial.print("Sending ");
    Serial.print(camera_image->len);
    Serial.print(" bytes ");
    Serial.print(camera_image->width);
    Serial.print("x");
    Serial.println(camera_image->height);

    com->send_img(PKT_DT_SRV_CAMERA_DATA, camera_image->buf, camera_image->len);

    camera.return_camera_fb();
}

void RemoteVehicle::main_loop() {
    com->loop();
}
