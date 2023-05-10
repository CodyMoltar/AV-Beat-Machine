#include "LedControl.h"

int number_of_displays = 4;

LedControl displays[4] = {
  LedControl(8,9,10,1),
  LedControl(11,13,12,1),
  LedControl(5,7,6,1),
  LedControl(32,34,36,1)
};

long value = 12345678;

unsigned long delaytime=250;

void setup(){

  for(int i = 0; i < number_of_displays; i++){
    displays[i].shutdown(0,false);
    displays[i].setIntensity(0,4);
    displays[i].clearDisplay(0);
  }

  // lc.shutdown(0,false);
  // lc.setIntensity(0,4);
  // lc.clearDisplay(0);

  // lc2.shutdown(0,false);
  // lc2.setIntensity(0,4);
  // lc2.clearDisplay(0);

  // lc3.shutdown(0,false);
  // lc3.setIntensity(0,4);
  // lc3.clearDisplay(0);

  // lc4.shutdown(0,false);
  // lc4.setIntensity(0,4);
  // lc4.clearDisplay(0);

}

void loop() { 

  for(int i = 0; i < number_of_displays; i++){

    displays[i].setDigit(0,7,(value/10000000)%10,false);
    displays[i].setDigit(0,6,(value/1000000)%10,false);
    displays[i].setDigit(0,5,(value/100000)%10,false);
    displays[i].setDigit(0,4,(value/10000)%10,false);
    displays[i].setDigit(0,3,(value/1000)%10,false);
    displays[i].setDigit(0,2,(value/100)%10,false);
    displays[i].setDigit(0,1,(value/10)%10,false);
    displays[i].setDigit(0,0,(value)%10,false);
  }

  value++;

  delay(delaytime);
  
}