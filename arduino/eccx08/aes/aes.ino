
/*
  Using the SparkFun Cryptographic Co-processor Breakout ATECC508a (Qwiic)
  By: Pete Lewis
  SparkFun Electronics
  Date: August 5th, 2019
  License: This code is public domain but you can buy me a beer if you use this and we meet someday (Beerware license).

  Feel like supporting our work? Please buy a board from SparkFun!
  https://www.sparkfun.com/products/15573

  This example shows how to setup your Cryptographic Co-processor with SparkFun's standard settings.
  ***Configurations settings are PERMENANT***
  We highly encourage advanced users to do their own configuration settings.

  Hardware Connections and initial setup:
  Install artemis in boards manager: http://boardsmanager/All#Sparkfun_artemis
  Plug in your controller board (e.g. Artemis Redboard, Nano, ATP) into your computer with USB cable.
  Connect your Cryptographic Co-processor to your controller board via a qwiic cable.
  Select TOOLS>>BOARD>>"SparkFun Redboard Artemis"
  Select TOOLS>>PORT>> "COM 3" (note, yours may be different)
  Click upload, and follow configuration prompt on serial monitor at 115200.

*/

//#include <SparkFun_ATECCX08a_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_ATECCX08a
#include <Wire.h>
#include <M5StickC.h>
#include "Sparkfun.h"

#define SLOT 9

ATECCX08A atecc;

// array to hold our 32 bytes of message. Note, it must be 32 bytes, no more or less.
uint8_t message[16] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

uint8_t key_write[32] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

// array to hold our 32 bytes of message. Note, it must be 32 bytes, no more or less.
uint8_t encrypt[16] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// array to hold our 32 bytes of message. Note, it must be 32 bytes, no more or less.
uint8_t decrypt[16] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint8_t signature[64] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void setup() {
  M5.begin();
  Wire.begin(0, 26);
  Serial.begin(115200);
  
  if (atecc.begin(0x60, Wire, Serial) == true) {
    Serial.println("Successful wakeUp(). I2C connections are good.");
  } else {
    Serial.println("Device not found. Check wiring.");
    while (1); // stall out forever
  }
  Serial.print("-------------------------\n\n\n\n");

  printInfo(); // see function below for library calls and data handling

  // check for configuration
  if (!(atecc.configLockStatus && atecc.dataOTPLockStatus && atecc.slot0LockStatus)) {
    Serial.print("Device not configured. Please use the configuration sketch.");
    //while (1); // stall out forever.
  }

  if (!atecc.write(ZONE_DATA, ADDRESS_DATA_WRITE_BLOCK_0 | DATA_SLOT_9, key_write, 32)){
    Serial.print("Could not write data_9");
  }
  if (!atecc.write(ZONE_DATA, ADDRESS_DATA_WRITE_BLOCK_0 | DATA_SLOT_9, key_write, 32)){
    Serial.print("Could not write data_9");
  }
  if (!atecc.write(ZONE_DATA, ADDRESS_DATA_WRITE_BLOCK_0 | DATA_SLOT_9, key_write, 32)){
    Serial.print("Could not write data_9");
  }

  //Serial.print("-------------------------\n");
  //printMessage(message, 16);
  //printMessage(encrypt, 16);
  //printMessage(decrypt, 16);
  //Serial.print("-------------------------\n");

  //if (!atecc.createNewKeyPair(SLOT)) {
  //  Serial.print("Couldnt createNewKeyPair\n");
  //}
  if (!atecc.aesEncryptEBC(DATA_SLOT_9, message, encrypt)){
    Serial.print("AES encrypt ERROR\n");
  }
  //Serial.print("-------------------------\n");
  //printMessage(message, 16);
  //printMessage(encrypt, 16);
  //printMessage(decrypt, 16);
  //Serial.print("-------------------------\n");

  if (!atecc.aesDecryptEBC(SLOT, encrypt, decrypt)){
    Serial.print("AES decrypt ERROR\n");
  }
  //Serial.print("-------------------------\n");
  //printMessage(message, 16);
  //printMessage(encrypt, 16);
  //printMessage(decrypt, 16);
  //Serial.print("-------------------------\n");
  
}

void loop()
{
  // do nothing.
}

void printInfo()
{
  // Read all 128 bytes of Configuration Zone
  // These will be stored in an array within the instance named: atecc.configZone[128]
  atecc.readConfigZone(false); // Debug argument false (OFF)

  // Print useful information from configuration zone data
  Serial.println();

  Serial.print("Serial Number: \t");
  for (int i = 0 ; i < 9 ; i++)
  {
    if ((atecc.serialNumber[i] >> 4) == 0) Serial.print("0"); // print preceeding high nibble if it's zero
    Serial.print(atecc.serialNumber[i], HEX);
  }
  Serial.println();

  Serial.print("Rev Number: \t");
  for (int i = 0 ; i < 4 ; i++)
  {
    if ((atecc.revisionNumber[i] >> 4) == 0) Serial.print("0"); // print preceeding high nibble if it's zero
    Serial.print(atecc.revisionNumber[i], HEX);
  }
  Serial.println();

  Serial.print("Config Zone: \t");
  if (atecc.configLockStatus) Serial.println("Locked");
  else Serial.println("NOT Locked");

  Serial.print("Data/OTP Zone: \t");
  if (atecc.dataOTPLockStatus) Serial.println("Locked");
  else Serial.println("NOT Locked");

  Serial.print("Data Slot 0: \t");
  if (atecc.slot0LockStatus) Serial.println("Locked");
  else Serial.println("NOT Locked");

  Serial.println();

  // if everything is locked up, then configuration is complete, so let's print the public key
  if (atecc.configLockStatus && atecc.dataOTPLockStatus && atecc.slot0LockStatus) 
  {
    if(atecc.generatePublicKey() == false)
    {
      Serial.println("Failure to generate This device's Public Key");
      Serial.println();
    }
  }
}

void printMessage(uint8_t *mes, uint8_t len)
{
  Serial.println("{");
  for (int i = 0; i < len ; i++)
  {
    Serial.print("0x");
    if ((mes[i] >> 4) == 0) Serial.print("0"); // print preceeding high nibble if it's zero
    Serial.print(mes[i], HEX);
    if (i != 31) Serial.print(", ");
    if ((31 - i) % 16 == 0) Serial.println();
  }
  Serial.println("};");
}
