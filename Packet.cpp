#include "Packet.h"

/* these are the packets that the server can recieve from the client, as you can see they follow
 *  
 *  PKT_XX_(CLI|ANY)_X+
 */
char *client_packet_headers[] = {
    PKT_CF_CLI_REQ_CON, // request connection
    PKT_CF_ANY_HEARTBEAT, // heartbeat
    PKT_CF_CLI_REQ_CON_END, // request connection end
    PKT_DT_CLI_REQ_CAM_IMG, // request camera image
    PKT_CF_CLI_REQ_CAM_RES_CHANGE, // requesting camera resolution change
    PKT_CF_CLI_CAM_CON_ATTEMPT
};

Packet::Packet(char *packet, uint8_t packet_size) {
    _packet_size = packet_size;
    _packet_type = -1;
    for (uint8_t i = 0; i < NUMBER_OF_CLIENT_PACKETS; i++) {
        if (strncmp(packet, client_packet_headers[i], 4) == 0)
            _packet_type = i;
    }
    if (packet_size > 4) {
        // copy everything 4-bytes-in on packet to _packet_info
        strncpy(_packet_info, packet + 4, packet_size - 4);
    }
}

int Packet::has_info() {
    return _packet_size > 4;
}

uint8_t Packet::parse_uint8(char *input, uint8_t input_size) {
    uint8_t total, subtotal;
    uint8_t current_power_of_10 = 0;
    
    for (uint8_t i = input_size - 1; i >= 0; i--) {
        subtotal = input[i] - '0';
        for (uint8_t mul = 0; mul < current_power_of_10; mul++)
            subtotal *= 10;
        total += subtotal;
        current_power_of_10++;
    }
    return total;
}

uint8_t Packet::get_info_as_uint8() {
    if (!has_info()) return 0;
    return parse_uint8(_packet_info, _packet_size - 4);
}

uint8_t Packet::get_packet_type() {
    return _packet_type;
}
