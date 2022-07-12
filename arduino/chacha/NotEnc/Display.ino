#include <M5StickC.h>
#include "my_utils.h"
#include "run_def.h"

#define BUF_SIZE 256
#define KEY_LENGTH 32

#define KEY_SIZE 32
#define NONSE_SIZE 12

#define PREAMBLE_BYTE 3
#define ERROR_INDEX 3
#define DATA_LENGTH_INDEX 4
#define MINIMUM_DATA_FRAME 6
#define EXCEPT_DATA_LEN 5
#define START_DATA_INDEX 5

int rand_num;
int rand_num_index;
char *buf_char;
unsigned char *nonse;
unsigned char *key;
struct chacha20_context *ctx;
uint8_t preamble[2] = {0xAA, 0xA5};

void setup() {
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(1);
  M5.Lcd.println("Started...");

  //key = (unsigned char *)malloc(KEY_SIZE);
  //for (int i =0; i < KEY_SIZE; i++){
    //key[i] = 0x7d;
  //}
  
  //nonse = (uint8_t *)malloc(NONSE_SIZE * sizeof(uint8_t));
  //for (int i = 0; i < NONSE_SIZE; i++){
    //nonse[i] = 0xFD;
  //}
  

  //ctx = (struct chacha20_context *)malloc(sizeof(struct chacha20_context));
  //buf_char = (char *)malloc(BUF_SIZE * sizeof(uint8_t));
    
  //chacha20_init_context(ctx, key, nonse, 0);
  
  Serial.begin(9600, SERIAL_8N1, 0, 26);
}

void loop() {
  if (Serial.available()) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 10);
    M5.Lcd.printf("Received data\n");
    memset(buf_char, '\0', BUF_SIZE);

    for (int i = 0; i < PREAMBLE_BYTE; i++){
      int read_byte = Serial.read() & 0xFF;
      buf_char[i] = read_byte;
      if ((read_byte & 0xFF) != preamble[i%2]){
        goto loopend;
      }
      M5.Lcd.printf("%x ", buf_char[i]);
    }

    int read_byte = Serial.read() & 0xFF;
    buf_char[ERROR_INDEX] = read_byte;
    M5.Lcd.printf("%x ", buf_char[ERROR_INDEX]);
    read_byte = Serial.read() & 0xFF;
    buf_char[DATA_LENGTH_INDEX] = read_byte;
    M5.Lcd.printf("%x ", buf_char[DATA_LENGTH_INDEX]);

    for (int i = 0; i < buf_char[DATA_LENGTH_INDEX]; i++){
      int read_byte = Serial.read() & 0xFF;
      buf_char[i + START_DATA_INDEX] = read_byte;
      M5.Lcd.printf("%x ", buf_char[i + START_DATA_INDEX]);
    }
    
    char *data_section = (char *)malloc(buf_char[DATA_LENGTH_INDEX] * sizeof(char));
    memset(data_section, '\0', buf_char[DATA_LENGTH_INDEX]);
    
    //if (is_errcode(buf_char)) {
    if (0) {
      M5.Lcd.printf("ERROR detected\n");
    //} else if (!check_sum(buf_char, buf_char[START_DATA_INDEX + buf_char[DATA_LENGTH_INDEX]])){
      //M5.Lcd.printf("Invalid check_sum\n");
      //delay(10000);
    } else {
      memset(data_section, 0, buf_char[DATA_LENGTH_INDEX]);
      strncpy(data_section, &buf_char[START_DATA_INDEX], buf_char[DATA_LENGTH_INDEX]);

      //chacha20_xor(ctx, (uint8_t *)data_section, buf_char[DATA_LENGTH_INDEX]);

      M5.Lcd.printf("\n\nDecrypted data\n");
      for (int i = 0; i <  buf_char[DATA_LENGTH_INDEX]; i++) {
        M5.Lcd.printf("%x ", data_section[i]);
      }

      //chacha20_xor(ctx, (uint8_t *)data_section, buf_char[DATA_LENGTH_INDEX]);
     
    }

    M5.Lcd.printf("\n\nWrite data\n");
    for (int i = 5; i <  buf_char[DATA_LENGTH_INDEX] + 5; i++) {
      //M5.Lcd.printf("%x ", data_section[i]);
      Serial.print(buf_char[i]);
    }
    free(data_section);
  }
loopend: 
  delay(1);
}
