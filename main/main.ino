#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Rotary.h>
#include <RotaryEncOverMCP.h>

// move all pin connections to seperate header file later
#include <pinConnections.h>

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
void changeLightFrequency(bool clockwise, int id);
void changeMotorSpeed(bool clockwise, int id);

/* Array of all rotary encoders and their pins */
RotaryEncOverMCP rotaryEncoders[] = {

  // instrument 1
  RotaryEncOverMCP(&mcp, 15, 14, &changeLightFrequency, 0),
  RotaryEncOverMCP(&mcp, 0, 1, &changeMotorSpeed, 0),

  // instrument 2
  RotaryEncOverMCP(&mcp, 2, 3, &changeLightFrequency, 1),
  RotaryEncOverMCP(&mcp, 4, 5, &changeMotorSpeed, 1),

  // instrument 3
  RotaryEncOverMCP(&mcp, 6, 7, &changeLightFrequency, 2),
  RotaryEncOverMCP(&mcp, 8, 9, &changeMotorSpeed, 2),

  // instrument 4
  RotaryEncOverMCP(&mcp, 10, 11, &changeLightFrequency, 3),
  RotaryEncOverMCP(&mcp, 12, 13, &changeMotorSpeed, 3)
  
};
constexpr int numEncoders = (int)(sizeof(rotaryEncoders) / sizeof(*rotaryEncoders));


// add all instruments to the array
int instruments[4] = {
	instrument1,
	instrument2,
	instrument3,
	instrument4
}

// calculate the number of instruments
int number_of_instruments[sizeof(sensor)];

// set all initial led frequencies and motor speeds
for(int i = 0; i < number_of_instruments; i++){
	instruments[i].led_hz = 1;
	instruments[i].motor_speed_pwm = 0;
}

void changeLightFrequency(bool clockwise, int id) {
  
  if(clockwise && hz[id] < 500){
    hz[id]++;
  }

  if(!clockwise && speed[id] > 1) {
    speed[id]--;
  }


}

void changeMotorSpeed(bool clockwise, int id) {
  
  if(clockwise && speed[id] < 255){

    speed[id]++;
    analogWrite(instruments[id][3], speed[id]);

  }

  if(!clockwise && speed[id] > 0) {
    speed[id]--;
    analogWrite(instruments[id][3], speed[id]);
  }

}

// constants won't change. Used here to set a pin number:
const int ledPin = LED_BUILTIN;  // the number of the LED pin

// Variables will change:
int ledState = LOW;  // ledState used to set the LED

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;  // will store last time LED was updated

// constants won't change:


void setup() {

  pinMode(ledPin, OUTPUT);


  // Serial.begin(115200);
  // Serial.println("MCP23007 Interrupt Test");

  pinMode(arduinoIntPin, INPUT);

  mcp.begin();         // use default address 0
  mcp.readINTCAPAB();  //read this so that the interrupt is cleared

  //initialize all rotary encoders

  //Setup interrupts, OR INTA, INTB together on both ports.
  //thus we will receive an interrupt if something happened on
  //port A or B with only a single INT connection.
  mcp.setupInterrupts(true, false, LOW);

  //Initialize input encoders (pin mode, interrupt)
  for (int i = 0; i < numEncoders; i++) {
    rotaryEncoders[i].init();
  }

  attachInterrupt(digitalPinToInterrupt(arduinoIntPin), intCallBack, FALLING);
}

// The int handler will just signal that the int has happened
// we will do the work from the main loop.
void INTERRUPT_FUNC_ATTRIB intCallBack() {
  awakenByInterrupt = true;
}

void checkInterrupt() {
  if (awakenByInterrupt) {
    // disable interrupts while handling them.
    detachInterrupt(digitalPinToInterrupt(arduinoIntPin));
    handleInterrupt();
    attachInterrupt(digitalPinToInterrupt(arduinoIntPin), intCallBack, FALLING);
  }
}

void handleInterrupt() {
  //Read the entire state when the interrupt occurred

  //An interrupt occurred on some MCP object.
  //since all of them are ORed together, we don't
  //know exactly which one has fired.
  //just read all of them, pre-emptively.

  for (int j = 0; j < numMCPs; j++) {
    uint16_t gpioAB = allMCPs[j]->readINTCAPAB();
    // we need to read GPIOAB to clear the interrupt actually.
    volatile uint16_t dummy = allMCPs[j]->readGPIOAB();
    for (int i = 0; i < numEncoders; i++) {
      //only feed this in the encoder if this
      //is coming from the correct MCP
      if (rotaryEncoders[i].getMCP() == allMCPs[j])
        rotaryEncoders[i].feedInput(gpioAB);
    }
  }

  cleanInterrupts();
}

// handy for interrupts triggered by buttons
// normally signal a few due to bouncing issues
void cleanInterrupts() {
#ifdef __AVR__
  EIFR = 0x01;
#endif
  awakenByInterrupt = false;
}

void loop() {


  //Check if an interrupt has occurred and act on it
  checkInterrupt();
}