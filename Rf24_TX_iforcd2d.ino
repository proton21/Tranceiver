#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const uint64_t pipeOut = 0xE8E8F0F0E1LL;

struct MyData {
  byte axis1;
  byte axis2;
  byte axis3;
  byte axis4;
  unsigned long _micros;
};

MyData data ;

RF24 radio(7, 8); //google 코드랑 비교확인.

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
  radio.begin(); 
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);

  radio.openWritingPipe(pipeOut);


  data.axis1 = 50;
  data.axis2 = 50;
  data.axis3 = 50;
  data.axis4 = 50;
  radio.write(&data, sizeof(MyData));

  delay(750);

}

void loop() {
 
  //data.axis1 = map( analogRead(3), 0,1023,0,100);  //yaw
   data._micros = micros();
  data.axis2 = map( analogRead(0), 0, 1023, 100, 0); //throttle
  data.axis1 = map( analogRead(1), 0, 1023, 0, 100); //roll

  //data.axis2 = map( analogRead(2), 0,1023,100,0);  //pitch

  radio.write(&data, sizeof(MyData));
 
}
