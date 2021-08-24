/*
    RemoteVehicle - library for controlling a remote vehicle using a ESP32CAM board.
    Created by Luan Negroni Sibinel, August 24, 2021.
    Released under the MIT license.
*/
#include "VehicleMotors.h"

VehicleMotors::VehicleMotors(uint8_t left_a, uint8_t left_b, uint8_t right_a, uint8_t right_b) 
    : _left_a(left_a), _left_b(left_b), _right_a(right_a), _right_b(right_b), _currentAction(MovimentationStatus::NEUTRAL) {
        pinMode(_left_a, OUTPUT);
        pinMode(_left_b, OUTPUT);
        pinMode(_right_a, OUTPUT);
        pinMode(_right_b, OUTPUT);
}

void VehicleMotors::updateSignals() {
    digitalWrite(_left_a, _currentAction.get_lStatus().getA());
    digitalWrite(_left_b, _currentAction.get_lStatus().getB());
    digitalWrite(_right_a, _currentAction.get_rStatus().getA());
    digitalWrite(_right_b, _currentAction.get_rStatus().getB());
}

MovimentationStatus VehicleMotors::getAction() {
    return _currentAction;
}

void VehicleMotors::setAction(MovimentationStatus action) {
    _currentAction = action;
    updateSignals();
}
