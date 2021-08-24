/*
    RemoteVehicle - library for controlling a remote vehicle using a ESP32CAM board.
    Created by Luan Negroni Sibinel, August 24, 2021.
    Released under the MIT license.
*/
#ifndef i_VehicleMotors
#define i_VehicleMotors

#include <stdint.h>
#include <Arduino.h>
#include "MovimentationStatus.h"
#include "Logger.h"

class VehicleMotors {
    private:
        uint8_t _left_a, _left_b, _right_a, _right_b;
        MovimentationStatus _currentAction;

        void updateSignals();
    public:
        VehicleMotors(uint8_t left_a, uint8_t left_b, uint8_t right_a, uint8_t right_b);
        MovimentationStatus getAction();
        void setAction(MovimentationStatus action);
};

#endif