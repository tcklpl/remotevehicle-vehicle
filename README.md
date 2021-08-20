
# REMOTE VEHICLE: Vehicle

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

## CInfo (vehicleinfo_t)

CInfo (Creation Info) is how you configure the library, it contains all the parameters you can edit about the vehicle (without editing its source of course). I chose to do this because otherwise there would be multiple long ass constructors.
Please note that some of the parameters are obrigatory, those need to be filled in order to everything to work.

Field        | Obrigatory | Default Value | Description | Type | Values Accepted
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

then you'll create your callbacks (name them whaterver you want), just be sure that your callback function has one Packet parameter:
    
    #include "Listener.h"
    #include "Packet.h"

    class MyListener : public Listener {
        public:
            void callback_connection_attempt(Packet p);
    }