

// const int ledPin = LED_BUILTIN;  // the number of the LED pin

// int hz = 100;

// bool light_on = false;  // ledState used to set the LED

// int light_on_time_ms = 500;
// int total_period = 1000000/hz;
// int light_off_time_ms = total_period - light_on_time_ms;



int instrument[3][7] = {
//    hz  -  light_on  -  light_on_time  -  total_period  -  light_off_time  -  ledpin
    {100, false, 500, 0, 0, 2, 0},
    {60,  false, 500, 0, 0, 3, 0},
    {20,  false, 500, 0, 0, 4, 0}
};



unsigned long previousMicros = 0;  // will store last time LED was updated


void setup() {

    Serial.begin(9600);

    for(int i = 0; i < 1; i++){

        instrument[i][3] = 1000000 / instrument[i][0];
        instrument[i][4] = instrument[i][3] - instrument[i][2];

        pinMode(instrument[i][5], OUTPUT);
        digitalWrite(instrument[i][5], instrument[i][1]);

        Serial.println(instrument[i][3]);

        // Serial.println(String(i) + " --> hz = " + String(instrument[i][0]));
        // Serial.println(String(i) + " --> total_period = " + String(instrument[i][3]));
        // Serial.println(String(i) + " --> light_on_time_ms = " + String(instrument[i][2]));
        // Serial.println(String(i) + " --> light_off_time_ms = " + String(instrument[i][4]));
        // Serial.println("------------------------------------------------------------");

    }

}

void loop() {

    // unsigned long currentMicros = micros();

    // for(int i = 0; i < 3; i++){
        
    //     if(instrument[i][1]){

    //         if (currentMicros - instrument[i][6] >= instrument[i][2]) {

    //             instrument[i][6] = currentMicros;

    //             instrument[i][1] = false;

    //             digitalWrite(instrument[i][5], instrument[i][1]);

    //         }
    //     } 

    //     else {

    //         if (currentMicros - instrument[i][6] >= instrument[i][4]) {

    //             instrument[i][6] = currentMicros;

    //             instrument[i][1] = true;

    //             digitalWrite(instrument[i][5], instrument[i][1]);

    //         }
    //     }
    // }

    

    // unsigned long currentMicros = micros();

    // if(light_on){

    //     if (currentMicros - previousMicros >= light_on_time_ms) {

    //         previousMicros = currentMicros;

    //         light_on = false;

    //         digitalWrite(ledPin, light_on);

    //     }
    // } 

    // else {

    //     if (currentMicros - previousMicros >= light_off_time_ms) {

    //         previousMicros = currentMicros;

    //         light_on = true;

    //         digitalWrite(ledPin, light_on);

    //     }
    // }

}