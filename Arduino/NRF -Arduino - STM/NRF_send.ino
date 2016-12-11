#include <SPI.h>
#include "RF24.h"

RF24 myRadio (9,10); // (ce_pin, cs_pin)

//byte addresses[][6] = {"1Node"};
uint8_t addresses[][6] = {
   0x7E,
  0x7E,
  0x7E,
  0x7E,
  0x7E
 //"1Node"
 };
int data = 6;

void setup() {
  Serial.begin(9600);
  //delay(1000);
  myRadio.begin();
  myRadio.setChannel(108);
  myRadio.setPALevel(RF24_PA_MIN);
  myRadio.setDataRate(RF24_250KBPS);
  myRadio.openWritingPipe(addresses[0]);
  myRadio.setPayloadSize(32);
  myRadio.setCRCLength(RF24_CRC_8);
  delay(500);  
}

void loop() {
  myRadio.write(&data, sizeof(data));
//data = data +1 ;
  Serial.print("\nPackage ");
  Serial.print(data);
  Serial.print("\n");
  
  delay(500);
  }
