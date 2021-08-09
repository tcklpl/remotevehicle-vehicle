#include "MovimentationStatus.h"

const MovimentationStatus MovimentationStatus::FORWARD = MovimentationStatus(MotorStatus::CLOCKWISE(), MotorStatus::COUNTERCLOCKWISE(), 1);
const MovimentationStatus MovimentationStatus::BACKWARD = MovimentationStatus(MotorStatus::COUNTERCLOCKWISE(), MotorStatus::CLOCKWISE(), 1);
const MovimentationStatus MovimentationStatus::TURN_L = MovimentationStatus(MotorStatus::NEUTRAL(), MotorStatus::COUNTERCLOCKWISE(), 2);
const MovimentationStatus MovimentationStatus::TURN_R = MovimentationStatus(MotorStatus::CLOCKWISE(), MotorStatus::NEUTRAL(), 2);
const MovimentationStatus MovimentationStatus::BRAKE = MovimentationStatus(MotorStatus::BRAKE(), MotorStatus::BRAKE(), 3);
const MovimentationStatus MovimentationStatus::NEUTRAL = MovimentationStatus(MotorStatus::NEUTRAL(), MotorStatus::NEUTRAL(), 0);

MovimentationStatus::MovimentationStatus(MotorStatus l_status, MotorStatus r_status, uint8_t priority) : _r_status(r_status), _l_status(l_status), _priority(priority) {}
MotorStatus MovimentationStatus::get_rStatus() {
    return _r_status;
}

MotorStatus MovimentationStatus::get_lStatus() {
    return _l_status;
}

int MovimentationStatus::operator >(const MovimentationStatus& o) {
    return _priority > o._priority;
}
