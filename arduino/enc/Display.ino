#include <M5StickC.h>
#include "encrypt.h"
#include "my_utils.h"

#define KEY_LENGTH 32
#define BUF_SIZE 256

String send_msgs[] = {
  "The one with the stars on it?",
  "It’s over there.",
  "Three years.",
  "Sendai and Nagoya for a year each, and Osaka for ten months.",
  "There was a traffic jam, and it took twice as long as usual to get here.",
  "Twenty minutes.",
  "Do we have to pay the full 100 dollars?",
  "We’re lucky. It’s usually at least 50%.",
  "No, it was delayed an hour and a half.",
  "1:30.",
  "The USB ports are on the left, so the mouse cable’s in my way.",
  "No, it’s too short."};

int index_send_msgs;

int rand_num;
int rand_num_index;
char *buf_char;

void setup() {
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(1.2);
  M5.Lcd.println("Started...");

  index_send_msgs = 0;

  rand_num_index = KEY_LENGTH - 1;
  init_seed(&rand_num);
  update_key(&rand_num);
  buf_char = (char *)malloc(BUF_SIZE);
  
  Serial.begin(9600, SERIAL_8N1, 0, 26);
}

void loop() {
   if (Serial.available()) {
    String string = Serial.readString();
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 10);
    string.toCharArray(buf_char, BUF_SIZE);

    encrypt(buf_char, &rand_num_index, KEY_LENGTH, &rand_num, string.length());
    
    M5.Lcd.println("Received: " + String(buf_char) + "\n");
    free(buf_char);
    
    M5.Lcd.println("Write:    " + send_msgs[index_send_msgs%12] + "\n");

    send_msgs[index_send_msgs%12].toCharArray(buf_char, send_msgs[index_send_msgs%12].length());
    
    encrypt(buf_char, &rand_num_index, KEY_LENGTH, &rand_num, send_msgs[index_send_msgs%12].length());
    Serial.print(String(buf_char));
    index_send_msgs = index_send_msgs + 1;
  }
}
