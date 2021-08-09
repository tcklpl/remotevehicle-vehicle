#ifndef i_VehicleMovimentation
#define i_VehicleMovimentation

class VehicleMovimentation;
struct vehiclemovinfo_t;

#include <stdint.h>
#include <Arduino.h>
#include "VehicleMotors.h"
#include "MovimentationStatus.h"

struct vehiclemovinfo_t {
    // motor pins
    uint8_t left_a, left_b, right_a, right_b;
    MovimentationStatus default_state = MovimentationStatus::NEUTRAL;
};

class VehicleMovimentation {
    private:
        VehicleMotors *_motors;
        uint16_t _min_move_time;
        unsigned long _last_move;
    public:
        VehicleMovimentation(vehiclemovinfo_t info);
        void setMove(MovimentationStatus move);
};


#endif