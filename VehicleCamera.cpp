#include "VehicleCamera.h"

VehicleCamera::VehicleCamera() {
    _current_image_size_code = CAMERA_RESOLUTION_1280_720;
}

VehicleCamera::VehicleCamera(uint8_t camera_frame_size) {
    _current_image_size_code = camera_frame_size;
}

void VehicleCamera::setup_camera() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    if(psramFound()){
        config.frame_size = camera_frame_sizes[_current_image_size_code - 1]; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
        config.jpeg_quality = 10;
        config.fb_count = 2;
    } else {
        config.frame_size = camera_frame_sizes[_current_image_size_code - 1];
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    // Init Camera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        logger.severe("Camera setup failed, restarting...");
        ESP.restart();
    }

    _camera_sensor = esp_camera_sensor_get();
}

camera_fb_t* VehicleCamera::get_gamera_fb() {
    _camera_framebuffer = esp_camera_fb_get();
    return _camera_framebuffer;
}

void VehicleCamera::return_camera_fb() {
    esp_camera_fb_return(_camera_framebuffer);
}

int VehicleCamera::change_camera_resolution(uint8_t new_resolution) {
    if (new_resolution < 1 || new_resolution > 14) return 0;
    _current_image_size_code = new_resolution;
    _camera_sensor->set_framesize(_camera_sensor, camera_frame_sizes[new_resolution - 1]);
    return 1;
}

uint8_t VehicleCamera::get_camera_resolution() {
    return _current_image_size_code;
}

framesize_t camera_frame_sizes[] = {
    FRAMESIZE_96X96,    // 96x96
    FRAMESIZE_QQVGA,    // 160x120
    FRAMESIZE_QCIF,     // 176x144
    FRAMESIZE_HQVGA,    // 240x176
    FRAMESIZE_240X240,  // 240x240
    FRAMESIZE_QVGA,     // 320x240
    FRAMESIZE_CIF,      // 400x296
    FRAMESIZE_HVGA,     // 480x320
    FRAMESIZE_VGA,      // 640x480
    FRAMESIZE_SVGA,     // 800x600
    FRAMESIZE_XGA,      // 1024x768
    FRAMESIZE_HD,       // 1280x720
    FRAMESIZE_SXGA,     // 1280x1024
    FRAMESIZE_UXGA      // 1600x1200
};
