#include "PinChangeInterrupt.h"
#define CLK 2
#define DT 3
#define SW 4

int led = 7;

int hz = 1;
int rpm = 0;

int enA = 10;
int in1 = 9;
int in2 = 8;

String mode = "light";

int pot = A0;

int currentState;
int initState;

unsigned long debounceDelay = 0;

int ledState = LOW;  // ledState used to set the LED
unsigned long previousMillis = 0;

void setup() {

  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);

  // led
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  // motor
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  // Turn off motors - Initial state
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  // turn on motors - with 0 speed
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, 0);

  // Setup Serial Monitor
  Serial.begin(9600);

  // Read the initial state of CLK
  initState = digitalRead(CLK);

  // Call encoder_value() when any high/low changed seen
  // on interrupt 0 (pin 2), or interrupt 1 (pin 3)

  attachInterrupt(0, encoder_value, CHANGE);
  attachInterrupt(1, encoder_value, CHANGE);
  attachPCINT(digitalPinToPCINT(SW), button_press, CHANGE);

  if (mode == "motor") {
    mode = "light";
    Serial.println("Now controlling LED frequency...");
  } else {
    mode = "motor";
    Serial.println("Now controlling motor speed...");
  }

}


void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= (1000/hz)) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(led, ledState);
  }
}


void button_press(){

  int buttonVal = digitalRead(SW);

  //If we detect LOW signal, button is pressed
  if (buttonVal == LOW) {

    if (millis() - debounceDelay > 200) {

      if (mode == "motor") {
        mode = "light";
        Serial.println("Now controlling LED frequency...");
        
      } else {
        mode = "motor";
        Serial.println("Now controlling motor speed...");
      }
    }


    debounceDelay = millis();
  }
}


void encoder_value() {

  // Read the current state of CLK
  currentState = digitalRead(CLK);

  // If last and current state of CLK are different, then we can be sure that the pulse occurred
  if (currentState != initState && currentState == 1) {

    // Encoder is rotating counterclockwise so we decrement the counter
    if (digitalRead(DT) != currentState) {

      if (mode == "motor") {
        if (rpm < 255) {
          rpm++;
          analogWrite(enA, rpm);
        }
      } else {
        if (hz < 1000) {
          hz++;
        }
      }

    } else {
      // Encoder is rotating clockwise so we increment the counter
      if (mode == "motor") {
        if (rpm > 0) {
          rpm--;
          analogWrite(enA, rpm);
        }
      } else {
        if (hz > 1) {
          hz--;
        }
      }

    }

    // print the value in the serial monitor window
    if (mode == "motor") {
      Serial.print("Motor PWM value: ");
      Serial.println(rpm);
    } else {
      Serial.print("LED frequency: ");
      Serial.print(hz);
      Serial.println("hz");
    }
  }

    // Remember last CLK state for next cycle
    initState = currentState;
  
}
