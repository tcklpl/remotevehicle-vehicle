/*
    RemoteVehicle - library for controlling a remote vehicle using a ESP32CAM board.
    Created by Luan Negroni Sibinel, August 24, 2021.
    Released under the MIT license.
*/
#ifndef i_VehicleCamera
#define i_VehicleCamera

#include "esp_camera.h"
#include <Arduino.h>
#include "Logger.h"

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

// onboard led flash
#define FLASH_GPIO_NUM     4

// Possible camera resolutions
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

extern framesize_t camera_frame_sizes[];

class VehicleCamera {
    private:
        camera_fb_t *_camera_framebuffer;
        sensor_t    *_camera_sensor;
        uint8_t      _current_image_size_code;
        bool         _camera_flash_status;
    public:
        VehicleCamera();
        VehicleCamera(uint8_t camera_frame_size);

        void setup_camera();
        
        camera_fb_t* get_gamera_fb();
        void return_camera_fb();

        bool change_camera_resolution(uint8_t new_resolution);
        uint8_t get_camera_resolution();

        void set_flash_state(uint8_t state);
        bool is_flash_on();
};

#endif
