#ifndef i_Callback
#define i_Callback

#include "Packet.h"

template <typename T>
class Callback {
    private:
        T* instance;
        void (T::*member_function)(Packet);
    public:
        Callback(T* who, void (T::*mf)(Packet)) :  instance(who), member_function(mf){
        }
        
        void call(Packet p) {
            (instance->*member_function)(p);
        }
};

#endif
