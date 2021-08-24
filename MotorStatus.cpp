/*
    RemoteVehicle - library for controlling a remote vehicle using a ESP32CAM board.
    Created by Luan Negroni Sibinel, August 24, 2021.
    Released under the MIT license.
*/
#include "MotorStatus.h"

MotorStatus::MotorStatus(uint8_t a, uint8_t b) {
    _a = a;
    _b = b;
}

const uint8_t MotorStatus::getA() {
    return _a;
}

const uint8_t MotorStatus::getB() {
    return _b;
}

const MotorStatus& MotorStatus::CLOCKWISE() { static const MotorStatus _clockwise(1, 0); return _clockwise; }
const MotorStatus& MotorStatus::COUNTERCLOCKWISE() { static const MotorStatus _counterclockwiwse(0, 1); return _counterclockwiwse; }
const MotorStatus& MotorStatus::BRAKE() { static const MotorStatus _brake(1, 1); return _brake; }
const MotorStatus& MotorStatus::NEUTRAL() { static const MotorStatus _neutral(0, 0); return _neutral; }