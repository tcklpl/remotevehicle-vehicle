#include "RemoteVehicle.h"

RemoteVehicle::RemoteVehicle(vehicleinfo_t c_info) {
    cinfo = c_info;
}

void RemoteVehicle::initial_setup() {

    logger = Logger(cinfo.should_log, cinfo.log_severity);

    logger.info("Setting up camera");
    camera.setup_camera();
    
    com = new VehicleCommunication(this, cinfo);

    logger.info("Initialization done!, broadcasting to network");
}

vehicleinfo_t RemoteVehicle::get_cinfo() {
    return cinfo;
}

void RemoteVehicle::take_and_send_image() {
    if (!com->is_connected()) return;
    camera_fb_t *camera_image = camera.get_gamera_fb();
    com->send_img(PKT_DT_SRV_CAMERA_DATA, camera_image->buf, camera_image->len);
    camera.return_camera_fb();
}

int RemoteVehicle::change_canera_resolution(uint8_t new_resolution) {
    if (!com->is_connected()) return 0;
    return camera.change_camera_resolution(new_resolution);
}

void RemoteVehicle::main_loop() {
    com->loop();
}
