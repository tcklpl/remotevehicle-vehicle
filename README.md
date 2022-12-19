
# REMOTE VEHICLE: Vehicle

This is a project in 2 parts, this is the vehicle part and there's a controller part, wich you can find [here](https://github.com/tcklpl/remotevehicle-controller).
This part was developed using C++ and the libraries from Arduino and ESP, using as hardware the ESP32CAM AI-Thinker model.
> If you are using other ESP32CAM models, you'll have to edit the pins inside [VehicleCamera.h](VehicleCamera.h).

## Summary

1. [Downloading and setting up](#downloading-and-setting-up)
2. [Basic Project](#basic-project)
3. [Basic Project with Logging](#basic-project-with-logging)
4. [Vehicle Creation Info (CInfo)](#cinfo)
5. [Registering custom callbacks](#registering-custom-callbacks)


## Downloading and setting up

You can download the library on the releases page of this repository, after downloading, just go into the Arduino IDE (assuming you're using it), `Sketch > Include Library > Add .ZIP Library...` and select the file you've just downloaded. After this you can follow the examples below.

## Basic Project

Below is an example of what would be a basic implementation of the library:
    
    #include "RemoteVehicle.h"

    vehicleinfo_t cinfo; 
    RemoteVehicle *vehicle;

    void setup() {
        cinfo.wssid = <YOUR WIFI LOGIN SSID>;
        cinfo.wpass = <YOUR WIFI LOGIN PASSWORD>;
        cinfo.motor_la = 12;
        cinfo.motor_lb = 13;
        cinfo.motor_ra = 15;
        cinfo.motor_rb = 14;

        vehicle = new RemoteVehicle(cinfo);
        
        vehicle->initialSetup();
    }

    void loop() {
        vehicle->mainLoop();
    }

**PLEASE NOTE** that cinfo.motor_* referes to the pins that YOU connected them to on your board.

## Basic Project WITH LOGGING

    #include "RemoteVehicle.h"

    vehicleinfo_t cinfo; 
    RemoteVehicle *vehicle;

    void setup() {
        Serial.begin(115200);
        cinfo.wssid = <YOUR WIFI LOGIN SSID>;
        cinfo.wpass = <YOUR WIFI LOGIN PASSWORD>;
        cinfo.should_log = true;
        cinfo.log_severity = 0;
        cinfo.motor_la = 12;
        cinfo.motor_lb = 13;
        cinfo.motor_ra = 15;
        cinfo.motor_rb = 14;

        vehicle = new RemoteVehicle(cinfo);
        
        vehicle->initialSetup();
    }

    void loop() {
        vehicle->mainLoop();
    }

**PLEASE NOTE** that in order to see the logging you'll need to start the serial using `Serial.begin(BAUD)`, also, of course, you'll need to set `cinfo.should_log`. Setting up `cinfo.log_severity` is optional, it'll default to SEVERE.

## CInfo

CInfo (Creation Info or `vehicleinfo_t`) is how you configure the library, it contains all the parameters you can edit about the vehicle (without editing its source of course). I chose to do this because otherwise there would be multiple long ass constructors.
Please note that some of the parameters are obrigatory, those need to be filled in order to everything to work.

Field        | Required | Default Value | Description | Type | Values Accepted
-------------|------------|-------------|-|------|----------------
wssid        | ✔      |  | The WiFi network SSID | char* | array of characters
wpass        | ✔      |  | The WiFi network Password | char * | array of characters
wtimeout     | ❌ | 10 000| The time that the vehicle will wait for the WiFi to connect, elapsed this time it'll restart. **NOTE:** the time is measured in **milliseconds**, that is, 1s = 1000ms. | uint16_t | 0 - 65535
should_log   | ❌ | false | If the vehicle should log the the serial output | bool | true or false
log_severity | ❌ | SEVERE (3) | The minimum severity of the log messages that will be shown | uint8_t | 0 - 3
cmd_port     | ❌ | 6887 | The port that the vehicle will use to connect to the controller to send and receive commands | uint16_t | 0 - 65535
udp_port     | ❌ | 6888 | The port that the vehicle will use to broadcast to the network | uint16_t | 0 - 65535
img_port     | ❌ | 6889 | The port that the vehicle will use to connect to the controller to send images and videos | uint16_t | 0 - 65535
ir_enable    | ❌ | false | To enable the use of an infrared sensor | bool | true or false
ir_analog    | ❌ | | Analog pin to read the IR Sensor | uint8_t | 0 - 255
ir_digital   | ❌ | | Digital pin to read the IR Sensor | uint8_t | 0 - 255
motor_la     | ✔  | | Pin that connects to the left motor - A | uint8_t | 0 - 255
motor_lb     | ✔  | | Pin that connects to the left motor - B | uint8_t | 0 - 255
motor_ra     | ✔  | | Pin that connects to the right motor - A | uint8_t | 0 - 255
motor_rb     | ✔  | | Pin that connects to the tight motor - B | uint8_t | 0 - 255
motor_default| ❌ | MovimentationStatus::NEUTRAL | Default stance for the motors when the vehicle turns on | MovimentationStatus | MovimentationStatus::NEUTRAL, MovimentationStatus::FORWARD, MovimentationStatus::BACKWARD, MovimentationStatus::TURN_L, MovimentationStatus::TURN_R, MovimentationStatus::BRAKE
blockVideoStream | ❌ | false | If the vehicle shouldn't allow a video stream to be created | bool | true or false
videoStreamMaxFPS | ❌ | 30 | The maximum FPS that the vehicle will allow a video stream to be initialized | uint8_t | 0 - 255
videoStreamMaxResolution | ❌ | CAMERA_RESOLUTION_640_480 | The maximum frame size that the vehicle will allow a video stream to be initialized | uint8_t | Values are defined in `VehicleCamera.h`

## Registering custom callbacks

It's kinda tricky, first of all, in this implementation you can only register **one** custom callback per packet, but to do so you'll need to:

First extend `Listener`:
    
    #include "Listener.h"

    class MyListener : public Listener {

    }

Then you'll create your callbacks (name them whaterver you want), just be sure that your callback function has only one parameter and it's `Packet`:
    
    #include "Listener.h"
    #include "Packet.h"

    class MyListener : public Listener {
        public:
            void callback_connection_attempt(Packet p);
    }

After this, you'll need to register the callback, this can be done by doing so:

    vehicle->getCommunication()->registerCustomCallback(...);

The parameters for `registerCustomCallback` are `Callback<Listener>` and `int`, that represent in order, the callback that you previously created and the packet type that you'll be listening. These values are defined in [`Packet.h`](Packet.h#L66) (after the comment `packets that can be received`).

To create a `Callback<Listener>`, you'll need to cast your class and function the the parent for some reason, I don't know why but it doesn't accept a child on a parent parameter. So you can do this by doing so:

    #include "Callback.h"

    MyListener ml;
    Callback *cb = new Callback<Listener>((Listener*) &ml, (void (Listener::*)(Packet)) &MyListener::callback_connection_attempt);

Ugly, I know. Contact me if you have a better way of doing this please. But now you can register the final callback:

    vehicle->getCommunication()->registerCustomCallback(cb, PKT_REQ_CON);

And now `callback_connection_attempt(Packet p)` will be called when the controller requests the start of a connection.
