/*
    RemoteVehicle - library for controlling a remote vehicle using a ESP32CAM board.
    Created by Luan Negroni Sibinel, August 24, 2021.
    Released under the MIT license.
*/
#include "VehicleIR.h"

VehicleIR::VehicleIR(int8_t panalog, int8_t pdigital) {
    _panalog = panalog;
    _pdigital = pdigital;
    if (_pdigital != -1)
        pinMode(_pdigital, INPUT);
    if (_panalog != -1)
        pinMode(_panalog, INPUT);
}

int VehicleIR::digital() {
    return _pdigital == -1 ? -1 : digitalRead(_pdigital);
}

int VehicleIR::analog() {
    return _panalog == -1 ? -1 : analogRead(_panalog);
}
