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
