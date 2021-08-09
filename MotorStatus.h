#ifndef i_MotorStatus
#define i_MotorStatus

#include <stdint.h>
#include <Arduino.h>

class MotorStatus {
    public:
        static const MotorStatus& CLOCKWISE();
        static const MotorStatus& COUNTERCLOCKWISE();
        static const MotorStatus& BRAKE();
        static const MotorStatus& NEUTRAL();
    private:
        uint8_t _a, _b;
    public:
        MotorStatus(uint8_t a, uint8_t b);
        const uint8_t getA();
        const uint8_t getB();
};

#endif