/*
    RemoteVehicle - library for controlling a remote vehicle using a ESP32CAM board.
    Created by Luan Negroni Sibinel, August 24, 2021.
    Released under the MIT license.
*/
#ifndef i_VehicleIR
#define i_VehicleIR

#include <Wire.h>
#include <stdint.h>

class VehicleIR {
    private:
        int8_t _panalog, _pdigital;
    public:
          VehicleIR(int8_t panalog, int8_t pdigital);

          int digital();
          int analog();
};

#endif
