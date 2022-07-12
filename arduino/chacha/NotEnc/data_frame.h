#ifndef DATA_FRAME
#define DATA_FRAME

#define PREAMBLE_BYTE 3
#define ERROR_INDEX 3
#define DATA_LENGTH_INDEX 4
#define MINIMUM_DATA_FRAME 6
#define EXCEPT_DATA_LEN 5
#define START_DATA_INDEX 5

#include <string.h>
#include "run_def.h"

typedef struct data_frame {
  char running_status;
  char running_mode;
  char running_power;
  char running_vector;
  char running_nanoe;
  char running_temp;
}DATA_FRAME_AIRCON;

bool check_preamble(char *buf) {
  uint8_t preamble[PREAMBLE_BYTE] = {0x99, 0x99, 0x99};
  for (int i = 0; i < PREAMBLE_BYTE; i++){
    if (buf[i] != 0x99) {
      return false;
    }
  }
  return true;
}

bool is_errcode(char *buf) {
  if (buf[ERROR_INDEX] == 0x01) {
    return true;
  }

  return false;
}

uint8_t get_data_len(char *buf){
  return (uint8_t)buf[DATA_LENGTH_INDEX];
}

bool check_sum(char *buf, uint8_t data_len){
  int sum = 0;
  for(int i = 0; i < data_len + EXCEPT_DATA_LEN; i++){
    sum += (uint8_t)(buf[i]);
    sum &= 0xFF;
  }

  if (sum == 0){
    return true;
  } else {
    return false;
  }
}

DATA_FRAME_AIRCON read_data(char *buf){
  DATA_FRAME_AIRCON data;

  data.running_status = buf[RUNNING_STATUS];
  data.running_mode   = buf[RUNNING_MODE];
  data.running_power  = buf[RUNNING_POWER];
  data.running_vector = buf[RUNNING_VECTOR];
  data.running_nanoe  = buf[RUNNING_NANOE];
  data.running_temp   = buf[RUNNING_TEMP];

  return data;
}



#endif
