/*
    RemoteVehicle - library for controlling a remote vehicle using a ESP32CAM board.
    Created by Luan Negroni Sibinel, August 24, 2021.
    Released under the MIT license.
*/
#include "VehicleMovimentation.h"

VehicleMovimentation::VehicleMovimentation(vehiclemovinfo_t info) {
    _motors = new VehicleMotors(info.left_a, info.left_b, info.right_a, info.right_b);
}

void VehicleMovimentation::setMove(MovimentationStatus move) {
    _motors->setAction(move);
}