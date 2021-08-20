#include "RemoteVehicle.h"

RemoteVehicle::RemoteVehicle(vehicleinfo_t c_info) {
    cinfo = c_info;
}

void RemoteVehicle::initialSetup() {

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

vehicleinfo_t RemoteVehicle::getCInfo() {
    return cinfo;
}

void RemoteVehicle::takeAndSendImage() {
    if (!com->is_connected()) return;
    camera_fb_t *camera_image = camera.get_gamera_fb();
    com->sendImg(PKT_DT_SRV_CAMERA_DATA, camera_image->buf, camera_image->len);
    camera.return_camera_fb();
}

bool RemoteVehicle::changeCameraResolution(uint8_t new_resolution) {
    return camera.change_camera_resolution(new_resolution);
}

uint8_t RemoteVehicle::getCameraResolution() {
    return camera.get_camera_resolution();
}

void RemoteVehicle::move(MovimentationStatus status) {
    mov->setMove(status);
}

void RemoteVehicle::mainLoop() {
    com->loop();
    if (isBroadcastingVideo) {
        if (com->is_connected()) {
            if (millis() - lastBroadcastedFrameTime >= frameTime) {
                camera_fb_t *camera_image = camera.get_gamera_fb();
                com->sendImg(PKT_DT_VIDEO_STREAM, camera_image->buf, camera_image->len);
                camera.return_camera_fb();
            }
        }
    }
}

VehicleIR* RemoteVehicle::getIR() {
    return ir;
}

VehicleCamera* RemoteVehicle::getCamera() {
    return &camera;
}

void RemoteVehicle::setVideoBroadcast(bool status, uint8_t fps) {
    if (cinfo.blockVideoStream) return;
    if (cinfo.videoStreamMaxFPS < fps) return;
    if (cinfo.videoStreamMaxResolution < camera.get_camera_resolution()) return;

    isBroadcastingVideo = status;
    if (status) {
        frameTime = 1000 / fps;
        lastBroadcastedFrameTime = millis() - frameTime;
    }
}

bool RemoteVehicle::isCurrentlyBroadcastingVideo() {
    return isBroadcastingVideo;
}
