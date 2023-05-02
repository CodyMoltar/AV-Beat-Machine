#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Rotary.h>
#include <RotaryEncOverMCP.h>

#include "LedControl.h"

LedControl lc=LedControl(8,9,10,1);

int hz = 35;
int motorspeed = 0;

int motorA = 22;
int motorB = 24;
int motorE = 5;

const int ledPin = 12;  // the number of the LED pin

// Variables will change:
int ledState = LOW;  // ledState used to set the LED

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;  // will store last time LED was updated

#if defined(ESP32) || defined(ESP8266)
#define INTERRUPT_FUNC_ATTRIB IRAM_ATTR
#else
#define INTERRUPT_FUNC_ATTRIB  
#endif

/* Our I2C MCP23017 GPIO expanders */
Adafruit_MCP23017 mcp;

//Array of pointers of all MCPs if there is more than one
Adafruit_MCP23017* allMCPs[] = { &mcp };
constexpr int numMCPs = (int)(sizeof(allMCPs) / sizeof(*allMCPs));

/* the INT pin of the MCP can only be connected to
 * an interrupt capable pin on the Arduino, either
 * D3 or D2.
 * */
byte arduinoIntPin = 3;

/* variable to indicate that an interrupt has occured */
volatile boolean awakenByInterrupt = false;

/* function prototypes */
void intCallBack();
void cleanInterrupts();
void handleInterrupt();
void RotaryEncoderChanged(bool clockwise, int id);
void changeLight(bool clockwise, int id);
void changeMotor(bool clockwise, int id);

/* Array of all rotary encoders and their pins */
RotaryEncOverMCP rotaryEncoders[] = {
    // outputA,B on GPB7,GPB6, register with callback and ID=1 (GPB0=7 .. GPB7=15)
    RotaryEncOverMCP(&mcp, 7, 6, &changeMotor, 0),
    RotaryEncOverMCP(&mcp, 5, 4, &changeLight, 0)
};
constexpr int numEncoders = (int)(sizeof(rotaryEncoders) / sizeof(*rotaryEncoders));

void RotaryEncoderChanged(bool clockwise, int id) {
    Serial.println("Encoder " + String(id) + ": "
            + (clockwise ? String("clockwise") : String("counter-clock-wise")));
}

int updateLightNumber(int value){

  int thousands = value%10000;
  int hundreds = value%1000;
  int tens = value%100;
  int ones = value%10;

  int seperatedValues[4] = {thousands, hundreds, tens, ones};

  return seperatedValues;
}

void changeLight(bool clockwise, int id) {

  if(clockwise){
    if(hz < 1000){
      hz++;
      lc.clearDisplay(0);
      lc.setDigit(0,3,(hz/1000)%10,false);
      lc.setDigit(0,2,(hz/100)%10,false);
      lc.setDigit(0,1,(hz/10)%10,false);
      lc.setDigit(0,0,hz%10,false);
      lc.setDigit(0,3,(hz/1000)%10,false);
      lc.setDigit(0,2,(hz/100)%10,false);
      lc.setDigit(0,1,(hz/10)%10,false);
      lc.setDigit(0,0,hz%10,false);
      lc.setDigit(0,7,(motorspeed/1000)%10,false);
      lc.setDigit(0,6,(motorspeed/100)%10,false);
      lc.setDigit(0,5,(motorspeed/10)%10,false);
      lc.setDigit(0,4,motorspeed%10,false);
      tone(ledPin, hz);
    }
  }
  else{
    if(hz > 0){
      hz--;
      lc.clearDisplay(0);
      lc.setDigit(0,3,(hz/1000)%10,false);
      lc.setDigit(0,2,(hz/100)%10,false);
      lc.setDigit(0,1,(hz/10)%10,false);
      lc.setDigit(0,0,hz%10,false);
      lc.setDigit(0,7,(motorspeed/1000)%10,false);
      lc.setDigit(0,6,(motorspeed/100)%10,false);
      lc.setDigit(0,5,(motorspeed/10)%10,false);
      lc.setDigit(0,4,motorspeed%10,false);
      tone(ledPin, hz);
    }
  }

  Serial.println("Light Hz: " + String(hz) + "     Motorspeed: " + String(motorspeed));
  
}

void changeMotor(bool clockwise, int id) {

  if(clockwise){
    if(motorspeed < 255){
      motorspeed++;
      lc.clearDisplay(0);
      lc.setDigit(0,3,(hz/1000)%10,false);
      lc.setDigit(0,2,(hz/100)%10,false);
      lc.setDigit(0,1,(hz/10)%10,false);
      lc.setDigit(0,0,hz%10,false);
      lc.setDigit(0,7,(motorspeed/1000)%10,false);
      lc.setDigit(0,6,(motorspeed/100)%10,false);
      lc.setDigit(0,5,(motorspeed/10)%10,false);
      lc.setDigit(0,4,motorspeed%10,false);
      analogWrite(motorE, motorspeed);
    }
  }
  else{
    if(motorspeed > 0){
      motorspeed--;
      lc.clearDisplay(0);
      lc.setDigit(0,3,(hz/1000)%10,false);
      lc.setDigit(0,2,(hz/100)%10,false);
      lc.setDigit(0,1,(hz/10)%10,false);
      lc.setDigit(0,0,hz%10,false);
      lc.setDigit(0,7,(motorspeed/1000)%10,false);
      lc.setDigit(0,6,(motorspeed/100)%10,false);
      lc.setDigit(0,5,(motorspeed/10)%10,false);
      lc.setDigit(0,4,motorspeed%10,false);
      analogWrite(motorE, motorspeed);
    }
  }

  

  Serial.println("Light Hz: " + String(hz) + "     Motorspeed: " + String(motorspeed));
  
}



void setup(){

    Serial.begin(115200);
    Serial.println("Starting Audio Visual Beat machine");

    lc.shutdown(0,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(0,8);
    /* and clear the display */
    lc.clearDisplay(0);

    lc.setDigit(0,3,(hz/1000)%10,false);
    lc.setDigit(0,2,(hz/100)%10,false);
    lc.setDigit(0,1,(hz/10)%10,false);
    lc.setDigit(0,0,hz%10,false);
    lc.setDigit(0,7,(motorspeed/1000)%10,false);
    lc.setDigit(0,6,(motorspeed/100)%10,false);
    lc.setDigit(0,5,(motorspeed/10)%10,false);
    lc.setDigit(0,4,motorspeed%10,false);

    pinMode(arduinoIntPin,INPUT);

    pinMode(ledPin, OUTPUT);

    mcp.begin();      // use default address 0
    mcp.readINTCAPAB(); //read this so that the interrupt is cleared

    //initialize all rotary encoders

    pinMode(motorA, OUTPUT);
    pinMode(motorB, OUTPUT);
    digitalWrite(motorA, HIGH);
    digitalWrite(motorB, LOW);
    analogWrite(motorE, motorspeed);

    tone(ledPin, hz); 

    //Setup interrupts, OR INTA, INTB together on both ports.
    //thus we will receive an interrupt if something happened on
    //port A or B with only a single INT connection.
    mcp.setupInterrupts(true,false,LOW);

    //Initialize input encoders (pin mode, interrupt)
    for(int i=0; i < numEncoders; i++) {
        rotaryEncoders[i].init();
    }

    attachInterrupt(digitalPinToInterrupt(arduinoIntPin), intCallBack, FALLING);
}

// The int handler will just signal that the int has happened
// we will do the work from the main loop.
void INTERRUPT_FUNC_ATTRIB intCallBack() {
    awakenByInterrupt=true;
}

void checkInterrupt() {
    if(awakenByInterrupt) {
        // disable interrupts while handling them.
        detachInterrupt(digitalPinToInterrupt(arduinoIntPin));
        handleInterrupt();
        attachInterrupt(digitalPinToInterrupt(arduinoIntPin),intCallBack,FALLING);
    }
}

void handleInterrupt(){
    //Read the entire state when the interrupt occurred

    //An interrupt occurred on some MCP object.
    //since all of them are ORed together, we don't
    //know exactly which one has fired.
    //just read all of them, pre-emptively.

    for(int j = 0; j < numMCPs; j++) {
        uint16_t gpioAB = allMCPs[j]->readINTCAPAB();
        // we need to read GPIOAB to clear the interrupt actually.
        volatile uint16_t dummy = allMCPs[j]->readGPIOAB();
        for (int i=0; i < numEncoders; i++) {
            //only feed this in the encoder if this
            //is coming from the correct MCP
            if(rotaryEncoders[i].getMCP() == allMCPs[j])
                rotaryEncoders[i].feedInput(gpioAB);
        }
    }

    cleanInterrupts();
}

// handy for interrupts triggered by buttons
// normally signal a few due to bouncing issues
void cleanInterrupts(){
#ifdef __AVR__
    EIFR=0x01;
#endif
    awakenByInterrupt=false;
}

void loop() {
  //Check if an interrupt has occurred and act on it
  checkInterrupt();

}