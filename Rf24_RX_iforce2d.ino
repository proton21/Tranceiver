#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

const uint64_t pipeIn = 0xE8E8F0F0E1LL;

struct MyData {
 byte axis1;
 byte axis2;
 byte axis3;
 byte axis4;
 unsigned long _micros;
};

MyData data;

RF24 radio(7,8); //google 코드랑 비교확인.
Servo throttleServo;
Servo steeringServo;

void setup() {
  
  Serial.begin(115200);
 radio.begin(); 
 radio.setAutoAck(false);
 radio.setDataRate(RF24_250KBPS);

 radio.openReadingPipe(1,pipeIn);
 radio.startListening();

 throttleServo.attach(3);
 steeringServo.attach(5);

 data.axis1 = 50;
 data.axis2 = 50;
 data.axis3 = 50;
 data.axis4 = 50;

 delay(750);
}

unsigned long lastRecvTime = 0;

void loop() {
 
while (radio.available()){
  radio.read(&data, sizeof(MyData));
  lastRecvTime = millis();
}
 
  unsigned long now  = millis();
  if( now - lastRecvTime > 1000 ) {
    data.axis2 = 50 ;
    data.axis3 = 50 ;
  }
  Serial.println(data._micros);
byte s= map(data.axis3, 0, 100, 40, 140);
byte t= map(data.axis2, 0, 100, 140, 40);

steeringServo.write(s);
throttleServo.write(t);

}
