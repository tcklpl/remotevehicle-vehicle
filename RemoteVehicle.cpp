#include "RemoteVehicle.h"

RemoteVehicle::RemoteVehicle(vehicleinfo_t c_info) {
    cinfo = c_info;
}

void RemoteVehicle::initial_setup() {

    logger = Logger(cinfo.should_log, cinfo.log_severity);

    com = new VehicleCommunication(this, cinfo);
    
    logger.info("Setting up camera");
    camera.setup_camera();

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

void RemoteVehicle::main_loop() {
    com->loop();
}
