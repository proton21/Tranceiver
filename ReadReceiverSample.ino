/*
  Copyright 2011 Lex Talionis (Lex.V.Talionis at gmail)
  This program is free software: you can redistribute it
  and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation,
  either version 3 of the License, or (at your option) any
  later version.

  This code uses pin change interrupts and timer 1 to measure the
  time between the rise and fall of 3 channels of PPM
  (Though often called PWM, see http://forum.arduino.cc/index.php/topic,14146.html)
  on a typical RC car receiver.  It could be extended to as
  many channels as you like.  It uses the PinChangeInt library
  to notice when the signal pin goes high and low, and the
  Timer1 library to record the time between.


*/
#include <PinChangeInt.h>    // http://playground.arduino.cc/Main/PinChangeInt
#include <PinChangeIntConfig.h>
#include <TimerOne.h>        // http://playground.arduino.cc/Code/Timer1

#define NO_PORTB_PINCHANGES //PinChangeInt setup
#define NO_PORTC_PINCHANGES    //only port D pinchanges (see: http://playground.arduino.cc/Learning/Pins)
#define PIN_COUNT 3    //number of channels attached to the receiver
//#define MAX_PIN_CHANGE_PINS PIN_COUNT

#define RC_TURN 3    //Arduino pins attached to the receiver
#define RC_FWD 2
#define RC_FIRE 4
byte pin[] = {RC_FWD, RC_TURN, RC_FIRE};    //for maximum efficiency these pins should be attached
unsigned int time[] = {0, 0, 0};              // to the receiver's channels in the order listed here

byte state = 0;
byte burp = 0;  // a counter to see how many times the int has executed
byte cmd = 0;   // a place to put our serial data
byte i = 0;     // global counter for tracking what pin we are on

//serial monitor에 시간이 안 찍힌다. pinchange프록그램에서는 제대로 인터럽트가 동작했다. 
//PWM발생은 우노에서 하고 READRECEIVER은 NANO에 uploading하고 신호를 받아서 출력한다. 
//GND을 연결않했다. 연결한 후에는 인터럽터가 1번 발생했다. 이후는 반응이 없다. 시간도 물론 않찍힌다. 
//다시 반응이 없다. 인터럽터도 없다. 
//nano를 교체 후 인터럽터는 잡힌다. 아무래도 nano가 문제였던 것 같다. 
//왜 타임이 않 찍히는지 찾아야 한다. 이제부터~~

void setup() { 
  Serial.begin(9600);
  Serial.print("PinChangeInt ReciverReading test");
  Serial.println();            //warm up the serial port

  Timer1.initialize(2200);    //longest pulse in PPM is usually 2.1 milliseconds,
  //pick a period that gives you a little headroom.
  Timer1.stop();                //stop the counter
  Timer1.restart();            //set the clock to zero

  for (byte i = 0; i < 3; i++)
  {
    pinMode(pin[i], INPUT);     //set the pin to input
    digitalWrite(pin[i], HIGH); //use the internal pullup resistor
  
  }
    PCintPort::attachInterrupt(pin[i], rise, RISING);
  // attach a PinChange Interrupt to our first pin
     
}                         


void loop() {
  cmd = Serial.read();      //while you got some time gimme a systems report
  if (cmd == 'p')
  {
    Serial.print("time:\t");
    for (byte i = 0; i < PIN_COUNT; i++) //PIN_COUNT == 3;
    {
      
      Serial.print(i, DEC);
      Serial.print(":");
      Serial.print(time[i], DEC);
      Serial.print("\t");
    }
    Serial.print(burp, DEC);
    Serial.println();
    /*      Serial.print("\t");
            Serial.print(clockCyclesToMicroseconds(Timer1.pwmPeriod), DEC);
            Serial.print("\t");
            Serial.print(Timer1.clockSelectBits, BIN);
            Serial.print("\t");
            Serial.println(ICR1, DEC);*/
  }
  cmd = 0;
  switch (state)
  {
    case RISING: //we have just seen a rising edge
      PCintPort::detachInterrupt(pin[i]);
      PCintPort::attachInterrupt(pin[i], fall, FALLING); //attach the falling end
      state = 255;
      break;
    case FALLING: //we just saw a falling edge
      PCintPort::detachInterrupt(pin[i]);
      i++;                //move to the next pin
      i = i % PIN_COUNT;  //i ranges from 0 to PIN_COUNT
      PCintPort::attachInterrupt(pin[i], rise, RISING);
      state = 255;
      break;
      /*default:
          //do nothing
          break;*/
  }
   //Serial.print("T");
}

void rise()        //on the rising edge of the currently interesting pin
{
  Timer1.restart();        //set our stopwatch to 0
  Timer1.start();            //and start it up
  //Serial.println("Rising");
  state = RISING;
  //Serial.print('r');
  burp++;
}

void fall()        //on the falling edge of the signal
{
  state = FALLING;
      time[i] = readTimer1();  // read the time since timer1 was restarted 
      //그냥 시간을 기록하는 아두이노 기본 함수를 써서 테스트해본다. readtimer1함수는 제대로 작동되는가?
      Serial.println(Timer1.clockSelectBits);                    
    //time[i]=Timer1.read();    // The function below has been ported into the
  // the latest TimerOne class, if you have the
  // new Timer1 lib you can use this line instead
  Timer1.stop();
   // Serial.print('f');
}

unsigned long readTimer1()        //returns the value of the timer in microseconds
{ //remember! phase and freq correct mode counts
  //up to ICR1 then down again
  unsigned int tmp = TCNT1;
  char scale = 0;
  switch (Timer1.clockSelectBits) 
       {    //TCCR1B의 [2:0]bits(Clock Select 1 [n = 0..2])의 값을 읽어서 switch문을 대체할 수 있나?
    case 1:// no prescale  //cs비트를 구하는 함수는 어디 있는가? 여기서 프리스케일러가 왜 필요하지? 
      scale = 0;            //cs비트는 자동으로 구해지나?
      break;
    case 2:// x8 prescale
      scale = 3;
      break;
    case 3:// x64
      scale = 6;
      break;
    case 4:// x256
      scale = 8;
      break;
    case 5:// x1024
      scale = 10;
      break;
  }
  
  while (TCNT1 == tmp) //if the timer has not ticked yet
  {
    //do nothing -- max delay here is ~1023 cycles
  }
  tmp = (  (TCNT1 > tmp) ? (tmp) : (ICR1 - TCNT1) + ICR1  ); //if we are counting down add the top value
  //to how far we have counted down
  return ((tmp * 1000L) / (F_CPU / 1000L)) << scale;
}

