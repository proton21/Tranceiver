#define THROTTLE_SIGNAL_IN 0 // INTERRUPT 0 = DIGITAL PIN 2 - use the interrupt number in attachInterrupt
#define THROTTLE_SIGNAL_IN_PIN 2 
#define NEUTRAL_THROTTLE 1500

volatile int nThrottleIn = NEUTRAL_THROTTLE;
volatile unsigned long ulStartPeriod = 0;
volatile boolean bNewThrottleSignal = false;

void setup() {
  // put your setup code here, to run once:
attachInterrupt(THROTTLE_SIGNAL_IN, calcInput, CHANGE);
Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(bNewThrottleSignal)
  { 
    Serial.println(nThrottleIn);

    bNewThrottleSignal = false;
  }
  //other processing ...
}

void calcInput()
{
  if(digitalRead(THROTTLE_SIGNAL_IN_PIN) == HIGH)
  {
    ulStartPeriod = micros();
  }
  else
  { 
    if(ulStartPeriod && (bNewThrottleSignal == false ))
    {
      nThrottleIn = (int)(micros() - ulStartPeriod);
      ulStartPeriod = 0;
      bNewThrottleSignal = true;
      
    }
  }
}




