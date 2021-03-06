/*
    RemoteVehicle - library for controlling a remote vehicle using a ESP32CAM board.
    Created by Luan Negroni Sibinel, August 24, 2021.
    Released under the MIT license.
*/
#ifndef i_MovimentationStatus
#define i_MovimentationStatus

#include "MotorStatus.h"
#include <stdint.h>

class MovimentationStatus {
    public:
        // are all the possible movimentation values
        static const MovimentationStatus FORWARD;
        static const MovimentationStatus BACKWARD;
        static const MovimentationStatus TURN_L;
        static const MovimentationStatus TURN_R;
        static const MovimentationStatus BRAKE;
        static const MovimentationStatus NEUTRAL;
    private:
        MotorStatus _l_status, _r_status;
        uint8_t _priority;
    public:
        MovimentationStatus(MotorStatus l_status, MotorStatus r_status, uint8_t priority);
        MotorStatus get_rStatus();
        MotorStatus get_lStatus();

        int operator >(const MovimentationStatus& o);
};

#endif