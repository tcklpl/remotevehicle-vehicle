#ifndef i_PacketParser
#define i_PacketParser

#include "Packet.h"
#include "Listener.h"
#include "Callback.h"

class PacketParser {
    private:
        Callback<Listener> *default_callbacks[NUMBER_OF_CLIENT_PACKETS];
        Callback<Listener>  *custom_callbacks[NUMBER_OF_CLIENT_PACKETS];

        void trigger_callbacks(Packet p);
    public:
        PacketParser();
        Packet handle_packet(char *packet, uint8_t packet_size);
        void register_callback(Callback<Listener> *cb, int packet_type, int is_default);
};

#endif
