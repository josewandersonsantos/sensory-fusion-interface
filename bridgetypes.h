#ifndef BRIDGETYPES_H
#define BRIDGETYPES_H

/*
 * INCLUDES
 */
#include <inttypes.h>
/*
 * DEFINES
 */
#define SOF              0x7E
#define PAYLOAD_SIZE_MAX 128

/*
 * ENUMS
 */
typedef enum __frame_type_t
{
    FR_TYPE_VERSION = 0,
    FR_TYPE_CFG,
    FR_TYPE_GPS_DATA,
    FR_TYPE_COORDS,
    FR_TYPE_ACC_DATA,
    FR_TYPE_GYR_DATA,
    FR_TYPE_MPU_DATA,

} frame_type_t;

/*
 * STRUCTS
 */

typedef struct __frame_header_t
{
    uint16_t len;
    uint8_t type;
    uint8_t frameId;

} frame_header_t;

typedef struct __frame_t
{
    frame_header_t header;
    char* payload;
    uint16_t crc;
    
} frame_t;


#endif // BRIDGETYPES_H
