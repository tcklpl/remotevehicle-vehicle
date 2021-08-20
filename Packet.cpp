#include "Packet.h"

/* these are the packets that the server can recieve from the client, as you can see they follow
 *  
 *  PKT_XX_(CLI|ANY)_X+
 */
char *client_packet_headers[] = {
    PKT_CF_CLI_REQ_CON,
    PKT_CF_ANY_HEARTBEAT,
    PKT_CF_CLI_REQ_CON_END,
    PKT_DT_CLI_REQ_CAM_IMG,
    PKT_CF_CLI_REQ_CAM_RES_CHANGE,
    PKT_CF_CLI_CAM_CON_ATTEMPT,
    PKT_DT_CLI_REQ_CAM_RES,
    PKT_MV_STOP,
    PKT_MV_FORWARD,
    PKT_MV_LEFT,
    PKT_MV_RIGHT,
    PKT_MV_BACKWARD,
    PKT_MV_NEUTRAL,
    PKT_DT_IR_REQ_DIGITAL,
    PKT_DT_IR_REQ_ANALOG,
    PKT_CF_CHG_FLASH,
    PKT_DT_REQ_FLASH,
    PKT_DT_REQ_VIDEO_STREAM,
    PKT_DT_END_VIDEO_STREAM
};

Packet::Packet(char *packet, uint8_t packet_size) {
    _packet_size = packet_size;
    _packet_type = -1;
    for (uint8_t i = 0; i < NUMBER_OF_CLIENT_PACKETS; i++) {
        if (strncmp(packet, client_packet_headers[i], 4) == 0)
            _packet_type = i;
    }
    if (packet_size > 4) {
        _packet_info = new char[packet_size - 4];
        // copy everything 4-bytes-in on packet to _packet_info
        strncpy(_packet_info, packet + 4, packet_size - 4);
    }
}

void Packet::free_info() {
    if (_packet_info) {
        delete _packet_info;
    }   
}

int Packet::has_info() {
    return _packet_size > 4;
}

uint8_t Packet::parse_uint8(char *input, uint8_t input_size) {
    uint8_t total = 0, subtotal;
    uint8_t current_power_of_10 = 0;
    
    for (int i = input_size - 1; i >= 0; i--) {
        subtotal = input[i] - '0';
        // prevent other characters from being calaculated that are not between 0-9
        // don't have to check if < 0, because uint8_t is unsigned.
        if (subtotal > 9) subtotal = 0;
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
