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

// add all instruments to the array
int instruments[4] = {
	instrument1,
	instrument2,
	instrument3,
	instrument4
}

/* Array of all rotary encoders and their pins */
RotaryEncOverMCP rotaryEncoders[] = {

  // instrument 1
  RotaryEncOverMCP(
    &mcp,
    instrument1.motor_rotary_A,
    instrument1.motor_rotary_B
    , &changeMotorSpeed,
    0
  ),
  RotaryEncOverMCP(
    &mcp,
    instrument1.LED_rotary_A,
    instrument1.LED_rotary_B,
    &changeLightFrequency,
    0
  ),

  // instrument 2
  RotaryEncOverMCP(
    &mcp,
    instrument2.motor_rotary_A,
    instrument2.motor_rotary_B
    , &changeMotorSpeed,
    1
  ),
  RotaryEncOverMCP(
    &mcp,
    instrument2.LED_rotary_A,
    instrument2.LED_rotary_B,
    &changeLightFrequency,
    1
  ),

  // instrument 3
  RotaryEncOverMCP(
    &mcp,
    instrument3.motor_rotary_A,
    instrument3.motor_rotary_B
    , &changeMotorSpeed,
    2
  ),
  RotaryEncOverMCP(
    &mcp,
    instrument3.LED_rotary_A,
    instrument3.LED_rotary_B,
    &changeLightFrequency,
    2
  ),

  // instrument 4
  RotaryEncOverMCP(
    &mcp,
    instrument4.motor_rotary_A,
    instrument4.motor_rotary_B
    , &changeMotorSpeed,
    1
  ),
  RotaryEncOverMCP(
    &mcp,
    instrument4.LED_rotary_A,
    instrument4.LED_rotary_B,
    &changeLightFrequency,
    3
  )
  
};

constexpr int numEncoders = (int)(sizeof(rotaryEncoders) / sizeof(*rotaryEncoders));




// calculate the number of instruments
int number_of_instruments[sizeof(sensor)];

// set all initial led frequencies and motor speeds
for(int i = 0; i < number_of_instruments; i++){
	instruments[i].led_hz = 1;
	instruments[i].motor_speed_pwm = 0;
}

void changeLightFrequency(bool clockwise, int id) {
  
  if(clockwise && instruments[id].led_hz < 500){
    instruments[id].led_hz++;
  }
  if(!clockwise && instruments[id].led_hz > 1){
    instruments[id].led_hz--
  }

}

void changeMotorSpeed(bool clockwise, int id) {
  
  if(clockwise && instruments[id].motor_speed_pwm < 255){

    instruments[id].motor_speed_pwm++;
    analogWrite(instruments[id].motor_enable, instruments[id].motor_speed_pwm);

  }

  if(!clockwise && instruments[id].motor_speed_pwm > 0){

    instruments[id].motor_speed_pwm--;
    analogWrite(instruments[id].motor_enable, instruments[id].motor_speed_pwm);

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

  for(int i = 0; i < 4; i++){

    // LED
    pinMode(instrument[i].LED, OUTPUT);
    digitalWrite(instrument[i].LED, LOW);

    // MOTOR A
    pinMode(instrument[i].motor_A, OUTPUT);
    digitalWrite(instrument[i].motor_A, LOW);

    // MOTOR B
    pinMode(instrument[i].motor_B, OUTPUT);
    digitalWrite(instrument[i].motor_B, LOW);

    // MOTOR E
    pinMode(instrument[i].motor_enable, OUTPUT);
    analogWrite(instrument[i].motor_enable, 0);

    // DISPLAY din
    pinMode(instrument[i].display_din, OUTPUT);
    digitalWrite(instrument[i].display_din, LOW);

    // DISPLAY clock
    pinMode(instrument[i].display_clock, OUTPUT);
    digitalWrite(instrument[i].display_clock, LOW);

    // DISPLAY cs
    pinMode(instrument[i].display_cs, OUTPUT);
    digitalWrite(instrument[i].display_cs, LOW);

  }

  

  // set up the mcp for the rotary encoders
  Serial.begin(115200);
  Serial.println("MCP23007 Interrupt Test");
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
  pinMode(ledPin, OUTPUT);
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