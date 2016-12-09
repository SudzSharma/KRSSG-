#include <SPI.h>
#include "RF24.h"

RF24 myRadio (9,10);

byte addresses[][6] = {"1Node"};
int data;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  myRadio.begin();
  myRadio.setChannel(108);
  myRadio.setPALevel(RF24_PA_MIN);
  myRadio.setDataRate(RF24_250KBPS);
  myRadio.openReadingPipe(1,addresses[0]);
  myRadio.startListening();

}

void loop() {
  // put your main code here, to run repeatedly:
  if(myRadio.available())
  {
    while(myRadio.available())
    {
      myRadio.read( &data, sizeof(data));
    }
    Serial.print("\nPackage R ");
    Serial.print(data);
    Serial.print("\n");
  }
}
