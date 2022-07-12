#include <M5StickC.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include "my_utils.h"
#include "chacha20.h"
#include "data_frame.h"
#include "run_def.h"

#define BUF_SIZE 512
#define TOKEN_SIZE 26

#define KEY_SIZE 32
#define NONSE_SIZE 12

char *buf_char;
unsigned char *nonse;
unsigned char *key;
struct chacha20_context *ctx;
uint8_t preamble[2] = {0xAA, 0xA5};

const char *ssid = "cisco(2.4GHz)";
const char *pass = "$CysecAP$";
//const char *pass = "asdkfk-0:s";
char *api = "http://192.168.100.36:8000/?token="; //XVcqI7z22VCGp9z3tsuA9xg06Q
const char *bad_request = "Bad Request";

void setup() {
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(1);
  M5.Lcd.println("Started...");

  WiFiMulti WiFiMulti;
  WiFiMulti.addAP(ssid, pass);

  M5.Lcd.print("Connecting");

  while(WiFiMulti.run() != WL_CONNECTED) {
    M5.Lcd.print(".");
    delay(1000);
  }
  M5.Lcd.println("");
  M5.Lcd.println("Connected to");
  M5.Lcd.println(WiFi.localIP());

  key = (unsigned char *)malloc(KEY_SIZE);
  for (int i =0; i < KEY_SIZE; i++){
    key[i] = 0x7d;
  }
  
  nonse = (uint8_t *)malloc(NONSE_SIZE * sizeof(uint8_t));
  for (int i = 0; i < NONSE_SIZE; i++){
    nonse[i] = 0xFD;
  }
  

  ctx = (struct chacha20_context *)malloc(sizeof(struct chacha20_context));
  
  buf_char = (char *)malloc(BUF_SIZE * sizeof(uint8_t));
  memset(buf_char, 0, BUF_SIZE);
    
  chacha20_init_context(ctx, key, nonse, 0);
  
  Serial.begin(9600, SERIAL_8N1, 0, 26);
  Serial.flush();
  delay(1000);
  M5.Lcd.print("EOS");
}

// warikomi 8 kaimade?

void loop() {
  if (Serial.available()) {

    /*
     * PREAMBLE RECV
     */
    for (int i = 0; i < PREAMBLE_BYTE; i++){
      buf_char[i] = Serial.read(); // 0
      if (buf_char[i] != preamble[i%2]){
        goto loopend;
      }
    }

    /*
     * ERROR RECV
     */
    buf_char[ERROR_INDEX] = Serial.read();

    /*
     * LENGTH RECV
     */
    buf_char[DATA_LENGTH_INDEX] = Serial.read();
    int data_index_max = buf_char[DATA_LENGTH_INDEX] + START_DATA_INDEX;
    

    /*
     * DATA RECV
     */
    for (int i = START_DATA_INDEX; i < data_index_max; i++){
      buf_char[i] = Serial.read();
    }

    /* 
     *  PRINT RECV DATA
     */
    int data_length;
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 10);
    M5.Lcd.printf("Received data\n");
    data_length = buf_char[DATA_LENGTH_INDEX];
    data_length = 5;
    //for (int i = 0; i < START_DATA_INDEX + buf_char[DATA_LENGTH_INDEX]; i++) {
      //M5.Lcd.printf("%x ", buf_char[i]);
    //}
    if (data_length < 1) {
      goto loopend;
    }

    
    if (is_errcode(buf_char)) {
      M5.Lcd.printf("ERROR detected\n");
    } else {
      char *data_section = (char *)malloc(buf_char[DATA_LENGTH_INDEX] * sizeof(char));
      memset(data_section, '\0', buf_char[DATA_LENGTH_INDEX]);
      strncpy(data_section, &buf_char[START_DATA_INDEX], buf_char[DATA_LENGTH_INDEX]);

      /*
       * Decrypt section
       */
      chacha20_xor(ctx, (uint8_t *)data_section, buf_char[DATA_LENGTH_INDEX]);

      //M5.Lcd.printf("\n\nDecrypted :%s\n", data_section);

      char *response;
      int res_len = 0;
      
      if ((WiFi.status() == WL_CONNECTED)) { 
        HTTPClient http;
        char *url = (char *)malloc((strlen(api) + buf_char[DATA_LENGTH_INDEX]) * sizeof(char));
        memset(url, 0, (strlen(api) + buf_char[DATA_LENGTH_INDEX]));
        strncpy(url, api, strlen(api));
        strncpy(&url[strlen(api)], data_section, buf_char[DATA_LENGTH_INDEX]);
        
        http.begin(url);
        int httpCode = http.GET();
        
        String payload = http.getString(); 
        res_len = payload.length();
        if (res_len > 0xFF) {
          res_len = 0xFF;
        }
        response = (char *)malloc(res_len * sizeof(char));

        memset(response, 0, res_len * sizeof(char));
        payload.toCharArray(response, res_len);
        M5.Lcd.printf("\nResponse[%d] %s\n", payload.length(), response);
        http.end();
        free(url);
        
        chacha20_xor(ctx, (uint8_t *)response, res_len);
      }
      
      //M5.Lcd.printf("\n\nWrite data\n");
      /*
       * PREAMBLE Write
       */
      for (int i = 0; i < PREAMBLE_BYTE; i++){
        Serial.write(preamble[i%2]);
      }
      /*
       * ERROR WRITE
       */
      Serial.write('\0');
  
      /*
       * LENGTH WRITE
       */
      Serial.write(res_len);
      /*
       * DATA WRITE
       */
      for (int i = 0; i < res_len; i++) {
        Serial.write(response[i]);
      }
      //for (int i = 0; i < START_DATA_INDEX + res_len; i++) {
        //M5.Lcd.printf(" %x", write_data[i]);
      //}
      free(data_section);
      free(response);
    }
    memset(buf_char, 0, BUF_SIZE);
  }
loopend:
  delay(10);
}
