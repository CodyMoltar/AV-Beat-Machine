
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Rotary.h>
#include <RotaryEncOverMCP.h>
#include "LedControl.h"
#include <TonePlayer.h>

const int number_of_instruments = 4;

int lights[number_of_instruments] = {
  5, // instrument
  6,
  46,
  4
};

int light_frequencies[number_of_instruments] = {
  0,
  0,
  0,
  0
};

int ledStates[number_of_instruments] = {
  LOW, LOW, LOW, LOW
};

long previousMicros[number_of_instruments] = {
  0, 0, 0, 0
};

int motor_speeds[number_of_instruments] = {
  0,
  0,
  0,
  0
};

int motors[number_of_instruments][3] = {
  {44, 48, 10},
  {50, 52, 11},
  {47, 49, 9},
  {51, 53, 8}
};

TonePlayer tone1(TCCR3A, TCCR3B, OCR3AH, OCR3AL, TCNT3H, TCNT3L); // MUST BE pin D5 
TonePlayer tone2(TCCR4A, TCCR4B, OCR4AH, OCR4AL, TCNT4H, TCNT4L); // MUST BE pin D6
TonePlayer tone3(TCCR5A, TCCR5B, OCR5AH, OCR5AL, TCNT5H, TCNT5L); // MUST BE pin D46

LedControl displays[number_of_instruments] = {
  LedControl(23,27,25,1), // instrument 1
  LedControl(29,33,31,1), // instrument 2
  LedControl(22,26,24,1), // instrument 3
  LedControl(28,32,30,1) // instrument 4
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
    RotaryEncOverMCP(&mcp, 0, 1, &changeLight, 0),
    RotaryEncOverMCP(&mcp, 2, 3, &changeMotor, 0),
    // instrument 2
    RotaryEncOverMCP(&mcp, 4, 5, &changeLight, 1),
    RotaryEncOverMCP(&mcp, 6, 7, &changeMotor, 1),
    // instrument 3
    RotaryEncOverMCP(&mcp, 12, 13, &changeLight, 2),
    RotaryEncOverMCP(&mcp, 14, 15, &changeMotor, 2),
    // instrument 4
    RotaryEncOverMCP(&mcp, 11, 10, &changeLight, 3),
    RotaryEncOverMCP(&mcp, 9, 8, &changeMotor, 3)
};
constexpr int numEncoders = (int)(sizeof(rotaryEncoders) / sizeof(*rotaryEncoders));

void changeLight(bool clockwise, int id) {
  if(clockwise){
    if(light_frequencies[id] < 999){
      light_frequencies[id]++;
      update_displays(id);
    }
  }
  else{
    if(light_frequencies[id] > 0){
      light_frequencies[id]--;
      update_displays(id);
    }
  }
}

void changeMotor(bool clockwise, int id) {
  if(clockwise){
    if(motor_speeds[id] < 255){
      motor_speeds[id]++;
      analogWrite(motors[id][2], motor_speeds[id]);
      update_displays(id);
    }
  }
  else{
    if(motor_speeds[id] > 0){
      motor_speeds[id]--;
      analogWrite(motors[id][2], motor_speeds[id]);
      update_displays(id);
    }
  } 
}

void update_displays(int id){
  // motor speeds
  int value = motor_speeds[id];
  displays[id].setDigit(0,3,value/1000%10,false);
  displays[id].setDigit(0,2,value/100%10,false);
  displays[id].setDigit(0,1,value/10%10,false);
  displays[id].setDigit(0,0,value%10,false);
  // light frequencies
  value = light_frequencies[id];
  displays[id].setDigit(0,7,value/1000%10,false);
  displays[id].setDigit(0,6,value/100%10,false);
  displays[id].setDigit(0,5,value/10%10,false);
  displays[id].setDigit(0,4,value%10,false);
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
  
  // set up motor pins and start init speed
  for(int i = 0; i < number_of_instruments; i++){

    pinMode(lights[i], OUTPUT);
    digitalWrite(lights[i], ledStates[i]);

    pinMode(motors[i][0], OUTPUT);
    pinMode(motors[i][1], OUTPUT);
    pinMode(motors[i][2], OUTPUT);
    digitalWrite(motors[i][0], LOW);
    digitalWrite(motors[i][1], HIGH);
    analogWrite(motors[i][2], motor_speeds[i]);

    displays[i].shutdown(0,false);
    displays[i].setIntensity(0,2);
    displays[i].clearDisplay(0);
    update_displays(i);
  }

  for(int i = 0; i < number_of_instruments; i++){
    
  }

  

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

  

  unsigned long currentMicros = micros();

  for(int i = 0; i < 4; i++){

    if(light_frequencies[i] == 0){
      if(ledStates[i] == LOW){
        ledStates[i] = HIGH;
        digitalWrite(lights[i], ledStates[i]);
      }
    }

    if (currentMicros - previousMicros[i] >= 500000 / light_frequencies[i]) {
      // save the last time you blinked the LED
      previousMicros[i] = currentMicros;

      // if the LED is off turn it on and vice-versa:
      if (ledStates[i] == LOW) {
        ledStates[i] = HIGH;
      } else {
        ledStates[i] = LOW;
      }

      // Serial.print(lights[i]);
      // Serial.println(ledStates[i]);
      // Serial.println("INTERVAL REACHED");

      // set the LED with the ledState of the variable:
      digitalWrite(lights[i], ledStates[i]);
    }
  }


  checkInterrupt();

  

  // digitalWrite(46, HIGH);

  

}



// old stuff

// void changeLight(bool clockwise, int id) {

//   switch (id){
//     // instrument 1 (min hz = 31)
//     case 3:
//       if(clockwise){
//         if(light_frequencies[id] < 999){
//           light_frequencies[id]++;
//           tone(lights[id], light_frequencies[id]);
//           update_displays(id);
//         }
//       }
//       else{
//         if(light_frequencies[id] > 31){
//           light_frequencies[id]--;
//           tone(lights[id], light_frequencies[id]);
//           update_displays(id);
//         }
//       }
//       break;
//     case 0:
//       if(clockwise){
//         if(light_frequencies[id] < 999){
//           light_frequencies[id]++;
//           tone1.tone(light_frequencies[id]);
//           update_displays(id);
//         }
//       }
//       else{
//         if(light_frequencies[id] > 1){
//           light_frequencies[id]--;
//           tone1.tone(light_frequencies[id]);
//           update_displays(id);
//         }
//       }
//       break;
//     case 1:
//       if(clockwise){
//         if(light_frequencies[id] < 999){
//           light_frequencies[id]++;
//           tone2.tone(light_frequencies[id]);
//           update_displays(id);
//         }
//       }
//       else{
//         if(light_frequencies[id] > 1){
//           light_frequencies[id]--;
//           tone2.tone(light_frequencies[id]);
//           update_displays(id);
//         }
//       }
//       break;
//       case 2:
//       if(clockwise){
//         if(light_frequencies[id] < 999){
//           light_frequencies[id]++;
//           tone3.tone(light_frequencies[id]);
//           update_displays(id);
//         }
//       }
//       else{
//         if(light_frequencies[id] > 1){
//           light_frequencies[id]--;
//           tone3.tone(light_frequencies[id]);
//           update_displays(id);
//         }
//       }
//       break;
//   }
  
// }


