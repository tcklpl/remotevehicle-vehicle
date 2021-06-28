#include "Packets.h"

char *packet_headers[] = {
    "CFRC", // request connection
    "CFHB", // heartbeat
    "CFRE", // request connection end
    "DTRC"  // request camera image
};

Packet parse_packet(char *packet_content, uint8_t size) {
    Packet p;
    p.packet_type = -1;
    p.packet_size = size;
    p.packet = packet_content;
    for (uint8_t i = 0; i < NUMBER_OF_CLIENT_PACKETS; i++) {
        if (strncmp(packet_content, packet_headers[i], 4) == 0)
            p.packet_type = i;
    }
    return p;
}
