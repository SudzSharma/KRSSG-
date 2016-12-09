#include <SPI.h>
#include "RF24.h"

RF24 myRadio (9,10); // (ce_pin, cs_pin)

byte addresses[][6] = {"1Node"};
int data = 5;

void setup() {
  Serial.begin(9600);
  delay(1000);
  myRadio.begin();
  myRadio.setChannel(108);
  myRadio.setPALevel(RF24_PA_MIN);
  myRadio.setDataRate(RF24_250KBPS);
  myRadio.openWritingPipe(addresses[0]);
  delay(500);  
}

void loop() {
  myRadio.write(&data, sizeof(data));

  Serial.print("\nPackage ");
  Serial.print(data);
  Serial.print("\n");
  delay(500);
  }
