typedef struct sPacket {
    uint8_t packet_type, packet_size;
    char *packet;
} Packet;

#define NUMBER_OF_CLIENT_PACKETS     4

// packets that can be sent
#define HEADER_PACKET_BROADCAST             "CFBC"
#define HEADER_PACKET_CONNECTION_ACCEPTED   "CFCK"
#define HEADER_PACKET_CONNECTION_ENDED      "CFCE"
#define HEADER_PACKER_FORCE_CONNECTION_END  "DTXC"
#define HEADER_PACKET_CAMERA_DATA           "DTDC"

// packets that can be received
#define PACKET_UNKNOWN                  -1
#define PACKET_REQUESTING_CONNECTION    0
#define PACKET_HEARTBEAT                1
#define PACKET_REQUEST_CONNECTION_END   2
#define PACKET_REQUEST_CAMERA_IMAGE     3

char *packet_headers[] = {
    "CFRC",
    "CFHB",
    "CFRE",
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
