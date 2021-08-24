/*
    RemoteVehicle - library for controlling a remote vehicle using a ESP32CAM board.
    Created by Luan Negroni Sibinel, August 24, 2021.
    Released under the MIT license.
*/
#ifndef i_RemoteVehicle
#define i_RemoteVehicle

#include <stdint.h>
#include "VehicleMovimentation.h"
#include "VehicleCamera.h"

class RemoteVehicle;

/**
 * This struct is meant to be used to configure the vehicle and/or to change
 * default behaviour, the configurations are:
 * 
 * wssid            REQUIRED    The WiFi network's SSID.
 * wpass            REQUIRED    The WiFi network's password.
 * 
 * wtimeout                     Time in which the vehicle will try to connect to
 *                              the WiFi netowrk, if the connection time exceeds
 *                              this value, the vehicle will restart.
 * 
 * should_log                   0 for false or 1 for true, if the vehicle should
 *                              log to the Serial.
 * 
 * log_severity                 Which log severity will be shown. Possible values
 *                              are defined in "Logger.h"
 *                          
 * cmd_port                     TCP Port for sending and recieving commands.
 * udp_port                     UDP Port for bradcasting.
 * img_port                     TCP Port for sending images to the controller.
 * 
 * ir_enable                    To enable the use of an infrared sensor. You can
 *                              enable this and specify just the analog or the digital
 *                              pins, there's no need to specify both (unless of course
 *                              you're gonna use both).
 * ir_analog                    The pin that connects to the IR analog output.
 * ir_digital                   The pin that connects to the IR digital output.
 * 
 * motor_la         REQUIRED    The A pin to the left wheel motor.
 * motor_lb         REQUIRED    The B pin to the left wheel motor.
 * motor_ra         REQUIRED    The A pin to the right wheel motor.
 * motor_rb         REQUIRED    The B pin to the right wheel motor.
 * motor_default                Default stance for the motor.
 * 
 * blockVideoStream             If the vehicle should NOT ALLOW any requests for a video
 *                              stream, responding to all with ERROR 06.
 * videoStreamMaxFPS            The maximum FPS that the vehicle will allow a video stream
 *                              to be initialized.
 * videoStreamMaxResolution     The maximum frame size that the vehicle will allow a video
 *                              stream to be initialized.
 *                              
 */
struct vehicleinfo_t {
    // WiFi related configurations
    char *wssid, *wpass;
    uint16_t wtimeout = 10000;

    // Logging related configurations
    bool should_log = false;
    uint8_t log_severity = 3;

    // TCP/UDP related configurations
    uint16_t cmd_port = 6887, udp_port = 6888, img_port = 6889;

    // IR Sensor
    bool ir_enable = false;
    int8_t ir_analog = -1, ir_digital = -1;

    // Motor
    int8_t motor_la = -1, motor_lb = -1, motor_ra = -1, motor_rb = -1;
    MovimentationStatus motor_default = MovimentationStatus::NEUTRAL;

    // Video Stream
    bool blockVideoStream = false;
    uint8_t videoStreamMaxFPS = 30;
    uint8_t videoStreamMaxResolution = CAMERA_RESOLUTION_640_480;
};

#include "VehicleCommunication.h"
#include "PacketParser.h"
#include "Logger.h"
#include "VehicleIR.h"
#include "Listener.h"
#include "Callback.h"
#include <Arduino.h>

class RemoteVehicle {
    private:
        vehicleinfo_t cinfo;

        VehicleCamera camera;
        PacketParser parser;
        VehicleCommunication *com;

        VehicleIR *ir;
        VehicleMovimentation *mov;

        bool isBroadcastingVideo = false;
        unsigned long lastBroadcastedFrameTime = 0;
        uint16_t frameTime = 0;

    public:
        RemoteVehicle(vehicleinfo_t c_info);
        
        /**
         *  This function sets up the vehicle, setting up everything in the following order:
         * 
         *  ► Logger.
         *  ► Camera¹.
         *  ► Motors.
         *  ► IR Sensor (if requested).
         *  ► Communication
         *      ∟ WiFi².
         *      ∟ UDP Server.
         *      ∟ TCP Servers.
         *      ∟ Internal callbacks.
         * 
         *  (1) - The camera setup can fail sometimes, it's probably something with the ESP32.
         *  If this happens, you'll have to manually press the reset button on the ESP.
         * 
         *  (2) - I believe I don't have to say that the WiFi will only connect if you enter
         *  the correct SSID/Password, but sometimes the WiFi will take too long to connect, in this
         *  case, if the sime is greater than the specified threshold (wtimeout, defaults to 10s), the
         *  ESP will restart.
         */
        void initialSetup();

        /**
         *  Takes and sends an image using the communication IMG TCP socket. Is meant to be used by the
         *  communication, however you can call this, just be sure that the other side is expecting an
         *  image at the time.
         * 
         *  If the vehicle is not yet connected to a controller this function will simply do nothing.
         */
        void takeAndSendImage();

        /**
         *  Changes the camera resolution, all possible values are specified inside 'VehicleCamera.h', returns:
         *  true    if the operation is successful.
         *  false   if supplied with an invalid camera resolution code.
         */
        bool changeCameraResolution(uint8_t new_resolution);

        /**
         *  Returns the code of the current camera resolution, all codes are specified inside 'VehicleCamera.h'.
         */
        uint8_t getCameraResolution();

        /**
         *  Runs everything that needs to run every tick, you need to call this function inside your loop() function
         *  in order for everything to work.
         */
        void mainLoop();

        /**
         *  Returns the vehicle creation info.
         */
        vehicleinfo_t getCInfo();

        /**
         *  Changes the current movimentation instruction that the vehicle is following. All possible values are defined inside
         *  'MovimentationStatus.h'. 
         * 
         *  PLEASE DO NOT INSTANTIATE 'MovimentationStatus' !!!, EVERY VALUE YOU'LL NEED TO USE IS ALREADY DEFINED AS A PUBLIC STATIC
         *  VARIABLE INSIDE 'MovimentationStatus'. For further reading check out 'MovimentationStatus.h'
         */
        void move(MovimentationStatus status);

        /**
         *  Returns the Vehicle IR Sensor, this only returns the memory space.
         *  !!! THIS FUNCTION DOESN'T CHECK IF IR IS PRESENT, ALL IMPLEMENTATIONS NEED TO CHECK THIS !!!
         */
        VehicleIR* getIR();

        /**
         *  Returns the camera, this is mainly used in VehicleCommunication to retrieve the camera during the camera callbacks.
         */
        VehicleCamera* getCamera();

        /**
         *  Initiates or ends a video stream with the desired FPS (that need to be between the previously specified bounds).
         */
        void setVideoBroadcast(bool status, uint8_t fps);

        /**
         *  Returns if the vehicle is currently broadcasting a video stream.
         */ 
        bool isCurrentlyBroadcastingVideo();

        VehicleCommunication* getCommunication();

};

#endif
