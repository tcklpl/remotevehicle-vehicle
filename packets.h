typedef struct sPacket {
    uint8_t packet_type, packet_size;
    char *packet;
} Packet;

#define NUMBER_OF_CLIENT_PACKETS     3

#define PACKET_UNKNOWN               -1
#define PACKET_REQUESTING_CONNECTION 0
#define PACKET_HEARTBEAT             1
#define PACKET_REQUEST_CAMERA_IMAGE  2

char *packet_headers[] = {
    "CFRC",
    "CFHB",
    "DTRC"
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
