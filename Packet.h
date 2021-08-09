#ifndef i_Packet
#define i_Packet

#include <stdint.h>
#include <string.h>
#include <Arduino.h>

// packets that can be sent

// connection packets
#define PKT_CF_SRV_BROADCAST                "CFBC"
#define PKT_CF_CLI_REQ_CON                  "CFRC"
#define PKT_CF_SRV_CON_OK                   "CFCK"

#define PKT_CF_SRV_REQ_CAM_CON_ATTEMPT      "CFBA"
#define PKT_CF_CLI_CAM_CON_ATTEMPT          "CFRA"
#define PKT_CF_SRV_CAM_CON_OK               "CFCC"

#define PKT_CF_ANY_HEARTBEAT                "CFHB"

#define PKT_CF_CLI_REQ_CON_END              "CFRE"
#define PKT_CF_SRV_CON_END                  "CFCE"
#define PKT_CF_SRV_FORCE_CON_END            "DTXC"

// data packets - camera
#define PKT_DT_CLI_REQ_CAM_IMG              "DTRC"
#define PKT_DT_SRV_CAMERA_DATA              "DTDC"
#define PKT_CF_CLI_REQ_CAM_RES_CHANGE       "CFCS"
#define PKT_CF_SRV_CAM_RES_OK               "CFCO"
#define PKT_DT_CLI_REQ_CAM_RES              "DTRR"
#define PKT_DT_SRV_CAM_RES                  "DTCR"

// movement packets
#define PKT_MV_STOP                         "MVST"
#define PKT_MV_FORWARD                      "MVMW"
#define PKT_MV_LEFT                         "MVMA"
#define PKT_MV_RIGHT                        "MVMD"
#define PKT_MV_BACKWARD                     "MVMS"
#define PKT_MV_NEUTRAL                      "MVNT"

// error packets
#define PKT_ERR_CAM_RES                     "ER02"
#define PKT_ERR_NO_IR                       "ER03"

// packets that can be received
#define PKT_UNKNOWN                 -1
#define PKT_REQ_CON                  0
#define PKT_HEARTBEAT                1
#define PKT_REQ_CONNECTION_END       2
#define PKT_REQ_CAMERA_IMAGE         3
#define PKT_REQ_CAMERA_RES_CHANGE    4
#define PKT_REQ_CAMERA_CON           5
#define PKT_REQ_CAMERA_RES           6
#define PKT_STOP                     7
#define PKT_FORWARD                  8
#define PKT_LEFT                     9
#define PKT_RIGHT                   10
#define PKT_BACKWARD                11
#define PKT_NEUTRAL                 12

#define NUMBER_OF_CLIENT_PACKETS    13

extern char *client_packet_headers[];

class Packet {
    private:
        uint8_t _packet_type, _packet_size;
        char *_packet_info;

        uint8_t parse_uint8(char *input, uint8_t input_size);
    public:
        Packet(char *packet, uint8_t packet_size);
        
        uint8_t get_packet_type();
        int has_info();
        void free_info();
        uint8_t get_info_as_uint8();
};

#endif
