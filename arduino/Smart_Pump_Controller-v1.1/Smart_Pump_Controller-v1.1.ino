/**
 * Smart Controller for Pump.
 * version:1.1
 * author:Debajyoti Bagchi
 * Description: This sketch is for Digispark (Atmel Attiny85 chip).
 *              Pump can be controlled by both IR remote control and manual push button.
 *              ANY button of ANY remote can be used by pressing once to ON/OFF.
 *              TSOP1738 receiver signal is received using interrupt service routine (ISR). Pin - P0
 *              Push button is connected for manual on/off signal. Pin - P1
 *              A relay and a Led is connected in parallel to each other. Pin - P2
 *              A pizzo speaker for Relay On/Off Indication is connected. Pin - P3.
 *              2 Pins are unused and are set to OUTPUT pin with state LOW. Pin - P4, P5
 * 
 */
# include "avr/interrupt.h"

// P0 is used for IR receive using interrupt. Hence not declared here.
const byte pushButtonPin = 1;
const byte ledRelayPin = 2;
const byte relayStatusIndicatorPin = 3;
const byte unusedPin4GND = 4;//unused pin P4--made it to Ground as per forum discussion knowledge
const byte unusedPin5GND = 5;//unused pin P5--made it to Ground as per forum discussion knowledge

volatile boolean relayStatus = false;//as it is used in ISR and loop(), so its volatile.

byte manualButtonState;             // the current reading from the input pin
byte lastManualButtonState = LOW;   // the previous reading from the input pin
// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastManualDebounceTime = 0;  // the last time the output pin was toggled
long manualDebounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned int relayIndSoundDelay = 100; //100 ms

//volatile boolean lastRelayStatus = false;
//volatile long lastRelayToggleTime = 0;
volatile boolean isPlaySound = false;



void setup()
{
  GIMSK = 0b00100000; // turns on pin change interrupts
  PCMSK = 0b00000001; // turn on interrupts on pins PB0
  sei(); // enables interrupts
  pinMode(pushButtonPin, INPUT);
  pinMode(ledRelayPin, OUTPUT);
  pinMode(relayStatusIndicatorPin, OUTPUT);
  pinMode(unusedPin4GND, OUTPUT);
  pinMode(unusedPin5GND, OUTPUT);
  
  digitalWrite(ledRelayPin, HIGH);
  digitalWrite(relayStatusIndicatorPin, HIGH);
  digitalWrite(unusedPin4GND, LOW);//unused pin P4--made it to Ground as per forum discussion knowledge
  digitalWrite(unusedPin5GND, LOW);//unused pin P5--made it to Ground as per forum discussion knowledge


  relayStatus = false;//initialize relay state
  //Sound 3 times to show that system is going to be ready
  for (int i = 0; i < 3;) {
    digitalWrite(relayStatusIndicatorPin, LOW);
    delay(relayIndSoundDelay);
    digitalWrite(relayStatusIndicatorPin, HIGH);
    delay(relayIndSoundDelay);
    i++;
  }

  //now the system is ready
}

void loop()
{
  doManualOnOff();

  //if (((millis() - lastRelayToggleTime) > 5000)) {
    if (isPlaySound) {
      digitalWrite(relayStatusIndicatorPin, LOW);
      delay(relayIndSoundDelay);
      digitalWrite(relayStatusIndicatorPin, HIGH);
      isPlaySound = false;
    }

    if (relayStatus) {
      digitalWrite(ledRelayPin, LOW);
    } else {      
      digitalWrite(ledRelayPin, HIGH);
    }

 // }

}

ISR(PCINT0_vect)
{
  if (PINB & (1 << PB0)) {//if pin state change detected for IR pin
    //    digitalWrite(relayStatusIndicatorPin, LOW);
    //    delayMicroseconds(relayIndSoundDelay * 1000);//100 ms
    //    digitalWrite(relayStatusIndicatorPin, HIGH);

    //lastRelayStatus = relayStatus;
    relayStatus = !relayStatus;
    //lastRelayToggleTime = millis();
    isPlaySound = true;

  }

}

void doManualOnOff() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(pushButtonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastManualButtonState) {
    // reset the debouncing timer
    lastManualDebounceTime = millis();
  }

  if ((millis() - lastManualDebounceTime) > manualDebounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != manualButtonState) {
      manualButtonState = reading;
      if (manualButtonState == HIGH) {
        //        digitalWrite(relayStatusIndicatorPin, LOW);
        //        delay(relayIndSoundDelay);
        //        digitalWrite(relayStatusIndicatorPin, HIGH);

        //lastRelayStatus = relayStatus;
        relayStatus = !relayStatus;//just toggle the state
        //lastRelayToggleTime = millis();
        isPlaySound = true;
      }//end of -- if (manualButtonState == HIGH)
    }//end of -- if (reading != manualButtonState)
  }//end of -- if ((millis() - lastManualDebounceTime) > manualDebounceDelay)

  // save the reading.  Next time through the loop,
  // it'll be the lastModeButtonState:
  lastManualButtonState = reading;
}//end of -- doManualOnOff()
