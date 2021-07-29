#include "PacketParser.h"
#include <stdlib.h>

PacketParser::PacketParser() {
    for (int i = 0; i < NUMBER_OF_CLIENT_PACKETS; i++) {
        default_callbacks[i] = NULL;
        custom_callbacks[i]  = NULL;
    }
}

/**
 * Triggers both default and custom callbacks for any given packet.
 * PRIVATE FUNCTION, Only meant to be used by the function PacketParser::handle_packet.
 * 
 * packet           packet to trigger the callbacks.
 */
void PacketParser::trigger_callbacks(Packet packet) {
    uint8_t type = packet.get_packet_type();
    if (default_callbacks[type] != NULL)
        default_callbacks[type]->call(packet);
    if (custom_callbacks[type] != NULL)
        custom_callbacks[type]->call(packet);
}

/**
 * Creates a Packet object from the given parameters and triggers it's callbacks.
 * This function is only meant to be used by the core code.
 * 
 * packet           chars that were read on the TCP connection.
 * packet_size      length of the previous item.
 */
Packet PacketParser::handle_packet(char *packet, uint8_t packet_size) {
    Packet p(packet, packet_size);
    trigger_callbacks(p);
}

/**
 * Registers an specified callback function to an specified packet type, is meant
 * to be used by both core and user. 
 * 
 * CAUTION: IF THE CALLBACK IS ALREADY DEFINED, THIS FUNCTION _WILL_ OVERWRITE IT.
 * 
 * cb_fn            function pointer for the callback.
 * packet_type      packet to which this function will register the callback,
 *                  packet types are specified in Packet.h.
 * is_default       to be used when registering the vehicle default callbacks,
 *                  to any user this will always be 0.
 */
void PacketParser::register_callback(Callback<Listener> *cb, int packet_type, int is_default) {
    // check for valid packet identification
    if (packet_type < 0 || packet_type > (NUMBER_OF_CLIENT_PACKETS - 1)) return;
    if (is_default) {
        default_callbacks[packet_type] = cb;
    } else {
        custom_callbacks[packet_type] = cb;
    }
}
