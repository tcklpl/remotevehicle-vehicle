#pragma once
#include <stdint.h>
#include <string.h>

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

extern char *packet_headers[];

Packet parse_packet(char *packet_content, uint8_t size);
