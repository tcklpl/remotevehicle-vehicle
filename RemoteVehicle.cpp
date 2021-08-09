#include "RemoteVehicle.h"

RemoteVehicle::RemoteVehicle(vehicleinfo_t c_info) {
    cinfo = c_info;
}

void RemoteVehicle::initial_setup() {

    logger = Logger(cinfo.should_log, cinfo.log_severity);

    logger.info("Setting up camera");
    camera.setup_camera();

    logger.info("Setting up motors");
    vehiclemovinfo_t movinfo;
    movinfo.left_a = cinfo.motor_la;
    movinfo.left_b = cinfo.motor_lb;
    movinfo.right_a = cinfo.motor_ra;
    movinfo.right_b = cinfo.motor_rb;
    movinfo.default_state = cinfo.motor_default;
    mov = new VehicleMovimentation(movinfo);

    if (cinfo.ir_enable) {
        logger.info("Setting up IR Sensor");
        ir = new VehicleIR(cinfo.ir_analog, cinfo.ir_digital);
    }
    
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

bool RemoteVehicle::change_canera_resolution(uint8_t new_resolution) {
    return camera.change_camera_resolution(new_resolution);
}

uint8_t RemoteVehicle::get_camera_resolution() {
    return camera.get_camera_resolution();
}

void RemoteVehicle::move(MovimentationStatus status) {
    mov->setMove(status);
}

void RemoteVehicle::main_loop() {
    com->loop();
}
