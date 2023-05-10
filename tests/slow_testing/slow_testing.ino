#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Rotary.h>
#include <RotaryEncOverMCP.h>
#include "LedControl.h"

int number_of_instruments = 4;

int lights[number_of_instruments] = {
  4, // instrument
  5,
  6,
  7
};

int light_frequencies[number_of_instruments] = {
  1,
  1,
  1,
  1
};

int motor_speeds[number_of_instruments] = {
  0,
  0,
  0,
  0
};

int motors[number_of_instruments][3] = {
  {46, 48, 10},
  {50, 52, 11},
  {47, 49, 9},
  {51, 53, 8}
};

LedControl displays[number_of_instruments] = {
  LedControl(22,26,24,1), // instrument 1
  LedControl(4,6,5,1), // instrument 2
  LedControl(23,27,25,1), // instrument 3
  LedControl(29,33,31,1) // instrument 4
};

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
    // instrument 1
    RotaryEncOverMCP(&mcp, 1, 0, &changeLight, 0),
    RotaryEncOverMCP(&mcp, 2, 3, &changeMotor, 0),

    // instrument 2
    RotaryEncOverMCP(&mcp, 4, 5, &changeLight, 1),
    RotaryEncOverMCP(&mcp, 6, 7, &changeMotor, 1),

    // instrument 3
    RotaryEncOverMCP(&mcp, 8, 9, &changeLight, 2),
    RotaryEncOverMCP(&mcp, 10, 11, &changeMotor, 2),

    // instrument 4
    RotaryEncOverMCP(&mcp, 12, 13, &changeLight, 3),
    RotaryEncOverMCP(&mcp, 14, 15, &changeMotor, 3)
};
constexpr int numEncoders = (int)(sizeof(rotaryEncoders) / sizeof(*rotaryEncoders));

void RotaryEncoderChanged(bool clockwise, int id) {
    Serial.println("Encoder " + String(id) + ": "
            + (clockwise ? String("clockwise") : String("counter-clock-wise")));
}

void changeLight(bool clockwise, int id) {
    if(clockwise){
      if(light_frequencies[i] < 999){
        light_frequencies[i]++;
        update_displays();
      }
    }
    else{
      if(light_frequencies[i] > 0){
        light_frequencies[i--;
        update_displays();
      }
    }
}

void changeMotor(bool clockwise, int id) {
  if(clockwise){
    if(motor_speeds[id] < 255){
      motor_speeds[id]++;
      update_displays();
    }
  }
  else{
    if(motor_speeds[id] > 0){
      motor_speeds[id]--;
      update_displays(id);
    }
  }
   
}

void update_displays(int id){

  int value = motor_speeds[id];

  displays[id].setDigit(0,7,j,false);
  displays[id].setDigit(0,6,j+1,false);
  displays[id].setDigit(0,5,j+2,false);
  displays[id].setDigit(0,4,j+3,false);
  displays[id].setDigit(0,3,j+4,false);
  displays[id].setDigit(0,2,j+5,false);
  displays[id].setDigit(0,1,j+6,false);
  displays[id].setDigit(0,0,j+7,false);
}

void setup() {

  Serial.begin(115200);
  Serial.println("MCP23007 Interrupt Test");

  pinMode(arduinoIntPin,INPUT);

  mcp.begin();      // use default address 0
  mcp.readINTCAPAB(); //read this so that the interrupt is cleared

  //initialize all rotary encoders

  //Setup interrupts, OR INTA, INTB together on both ports.
  //thus we will receive an interrupt if something happened on
  //port A or B with only a single INT connection.
  mcp.setupInterrupts(true,false,LOW);

  //Initialize input encoders (pin mode, interrupt)
  for(int i=0; i < numEncoders; i++) {
      rotaryEncoders[i].init();
  }

  attachInterrupt(digitalPinToInterrupt(arduinoIntPin), intCallBack, FALLING);
  // put your setup code here, to run once:
  
  for(int i = 0; i < number_of_motors; i++){
    pinMode(motors[i][0], OUTPUT);
    pinMode(motors[i][1], OUTPUT);
    pinMode(motors[i][2], OUTPUT);

    digitalWrite(motors[i][0], LOW);
    digitalWrite(motors[i][1], HIGH);

    analogWrite(motors[i][2], 60);


  }

  

  for(int i = 0; i < number_of_displays; i++){

    displays[i].shutdown(0,false);
    /* Set the brightness to a medium values */
    displays[i].setIntensity(0,2);
    /* and clear the display */
    displays[i].clearDisplay(0);


    int j = 0;

    displays[i].setDigit(0,7,j,false);
    displays[i].setDigit(0,6,j+1,false);
    displays[i].setDigit(0,5,j+2,false);
    displays[i].setDigit(0,4,j+3,false);
    displays[i].setDigit(0,3,j+4,false);
    displays[i].setDigit(0,2,j+5,false);
    displays[i].setDigit(0,1,j+6,false);
    displays[i].setDigit(0,0,j+7,false);

  }


  // for(int i = 0; i < 4; i++){

        // instrument[i][3] = 1000000 / instrument[i][0];
        // instrument[i][4] = instrument[i][3] - instrument[i][2];

        // pinMode(instrument[i][5], OUTPUT);
        // digitalWrite(instrument[i][5], instrument[i][1]);

        // Serial.println(instrument[i][3]);

        // Serial.println(String(i) + " --> hz = " + String(instrument[i][0]));
        // Serial.println(String(i) + " --> total_period = " + String(instrument[i][3]));
        // Serial.println(String(i) + " --> light_on_time_ms = " + String(instrument[i][2]));
        // Serial.println(String(i) + " --> light_off_time_ms = " + String(instrument[i][4]));
        // Serial.println("------------------------------------------------------------");

    // }

  

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


  // for(int i = 0; i < number_of_displays; i++){

  //   displays[i].clearDisplay(0);

  //   int j = 0;

  //   displays[i].setDigit(0,7,j,false);
  //   displays[i].setDigit(0,6,j+1,false);
  //   displays[i].setDigit(0,5,j+2,false);
  //   displays[i].setDigit(0,4,j+3,false);
  //   displays[i].setDigit(0,3,j+4,false);
  //   displays[i].setDigit(0,2,j+5,false);
  //   displays[i].setDigit(0,1,j+6,false);
  //   displays[i].setDigit(0,0,j+7,false);

  //   delay(100);

  // }
    //Check if an interrupt has occurred and act on it
    checkInterrupt();


  // for(int i = 0; i <0; i++){
  //   pinMode(lights[i], OUTPUT);
  //   tone(lights[i], 40);
  //   noTone(lights[i]);
  // }



  // fo
  
  // unsigned long currentMicros = micros();

  // for(int i = 0; i < 0; i++){
      
  //     if(instrument[i][1]){

  //         if (currentMicros - instrument[i][6] >= instrument[i][2]) {

  //             instrument[i][6] = currentMicros;

  //             instrument[i][1] = false;

  //             digitalWrite(instrument[i][5], instrument[i][1]);

  //             // Serial.print(i);
  //             // Serial.println("turning  off");

  //         }
  //     } 

  //     else {

  //         if (currentMicros - instrument[i][6] >= instrument[i][4]) {

  //             instrument[i][6] = currentMicros;

  //             instrument[i][1] = true;

  //             digitalWrite(instrument[i][5], instrument[i][1]);

  //             // Serial.print(i);
  //             // Serial.println("turning on ");

  //         }
  //     }
  // }


}



