#include "esp_camera.h"
// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define CAMERA_RESOLUTION_96_96 1
#define CAMERA_RESOLUTION_160_120 2
#define CAMERA_RESOLUTION_176_144 3
#define CAMERA_RESOLUTION_240_176 4
#define CAMERA_RESOLUTION_240_240 5
#define CAMERA_RESOLUTION_320_240 6
#define CAMERA_RESOLUTION_400_296 7
#define CAMERA_RESOLUTION_480_320 8
#define CAMERA_RESOLUTION_640_480 9
#define CAMERA_RESOLUTION_800_600 10
#define CAMERA_RESOLUTION_1024_768 11
#define CAMERA_RESOLUTION_1280_720 12
#define CAMERA_RESOLUTION_1280_1024 13
#define CAMERA_RESOLUTION_1600_1200 14

camera_fb_t *camera_image = NULL;
sensor_t *camera_sensor = NULL;

void setup_camera(framesize_t frame_size) {
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
        config.frame_size = frame_size; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
        config.jpeg_quality = 10;
        config.fb_count = 2;
    } else {
        config.frame_size = frame_size;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    // Init Camera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        while(1);
    }

    camera_sensor = esp_camera_sensor_get();
}

void set_camera_resolution(int resolution_type) {
    framesize_t frame_size;
    switch (resolution_type) {
        case 1:
            frame_size = FRAMESIZE_96X96;
            break;
        case 2:
            frame_size = FRAMESIZE_QQVGA;
            break;
        case 3:
            frame_size = FRAMESIZE_QCIF;
            break;
        case 4:
            frame_size = FRAMESIZE_HQVGA;
            break;
        case 5:
            frame_size = FRAMESIZE_240X240;
            break;
        case 6:
            frame_size = FRAMESIZE_QVGA;
            break;
        case 7:
            frame_size = FRAMESIZE_CIF;
            break;
        case 8:
            frame_size = FRAMESIZE_HVGA;
            break;
        case 9:
            frame_size = FRAMESIZE_VGA;
            break;
        case 10:
            frame_size = FRAMESIZE_SVGA;
            break;
        case 11:
            frame_size = FRAMESIZE_XGA;
            break;
        case 12:
            frame_size = FRAMESIZE_HD;
            break;
        case 13:
            frame_size = FRAMESIZE_SXGA;
            break;
        case 14:
            frame_size = FRAMESIZE_UXGA;
            break;
        default:
            frame_size = FRAMESIZE_HD;
            break;
    }
    setup_camera(frame_size);
}

void take_photo() {
    camera_image = esp_camera_fb_get();  
    if(!camera_image) {
        Serial.println("Camera capture failed");
        return;
    }
}
