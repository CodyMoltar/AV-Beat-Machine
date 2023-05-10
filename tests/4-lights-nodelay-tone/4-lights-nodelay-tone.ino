
// constants won't change. Used here to set a pin number:
const int ledPin1 = 4;  
const int ledPin2 = 5;
const int ledPin3 = 6;
const int ledPin4 = 46;// the number of the LED pin

// Variables will change:
int hz1 = 1;
int ledState1 = LOW;  // ledState used to set the LED
unsigned long previousMillis1 = 0;  // will store last time LED was updated
const long interval1 = 1000000 / hz1;  // interval at which to blink (milliseconds)

int hz2 = 2;
int ledState2 = LOW;  // ledState used to set the LED
unsigned long previousMillis2 = 0;  // will store last time LED was updated
const long interval2 = 1000000 / hz2;

int hz3 = 3;
int ledState3 = LOW;  // ledState used to set the LED
unsigned long previousMillis3 = 0;  // will store last time LED was updated
const long interval3 = 1000000 / hz3;

int hz4 = 30;
int ledState4 = LOW;  // ledState used to set the LED
unsigned long previousMillis4 = 0;  // will store last time LED was updated
const long interval4 = 1000000 / hz4 / 2;

void setup() {
  // set the digital pin as output:
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);

}



void loop() {

  unsigned long currentMillis = micros();

  if (currentMillis - previousMillis1 >= interval1) {
    // save the last time you blinked the LED
    previousMillis1 = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState1 == LOW) {
      ledState1 = HIGH;
    } else {
      ledState1 = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin1, ledState1);
  }

  if (currentMillis - previousMillis2 >= interval2) {
    // save the last time you blinked the LED
    previousMillis2 = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState2 == LOW) {
      ledState2 = HIGH;
    } else {
      ledState2 = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin2, ledState2);
  }

  if (currentMillis - previousMillis3 >= interval3) {
    // save the last time you blinked the LED
    previousMillis3 = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState3 == LOW) {
      ledState3 = HIGH;
    } else {
      ledState3 = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin3, ledState3);
  }

  if (currentMillis - previousMillis4 >= interval4) {
    // save the last time you blinked the LED
    previousMillis4 = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState4 == LOW) {
      ledState4 = HIGH;
    } else {
      ledState4 = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin4, ledState4);
  }

  
}
