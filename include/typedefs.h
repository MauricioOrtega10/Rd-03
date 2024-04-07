#pragma once

#define LD2461_DEFAULT_BAUD 115200

enum Constants 
{
  FH_0 = 0x00,
  FH_1 = 0x01,
  FH_2 = 0x02,
  FH_3 = 0x03,
  SOMEONE = 0x06,
  TARGET_RANGE_0 = 0x07,
  TARGET_RANGE_1 = 0x08,
  PARAMETER_VALUE = 0x0A,
  FH_LAST = 0xF1,
  FE_LAST = 0xF5
}; 

typedef struct
{
    uint8_t buffer[512];
    uint8_t size;
    uint8_t frame_start;
}serial_buffer_struct;


//Not Used
typedef struct
{
    uint8_t frame_header[4];
    uint8_t data_length_0;
    uint8_t data_length_1;
    uint8_t somenone;
    uint8_t target_range[2];
    uint16_t range_energy[16];
    uint8_t frame_end[4];
}data_packet_struct;
