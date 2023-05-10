// // pin connected to the mosfet controlling LED
// #define instrument_1_LED 0

// // de gpios used on the mcp23017 for the encoders controlling the light frequency
// #define instrument_1_LED_rotary_A 0
// #define instrument_1_LED_rotary_B 1

// // pins connected to the L298N motor controller
// #define instrument_1_motor_A 2
// #define instrument_1_motor_B 4
// #define instrument_1_motor_enable A0

// // de gpios used on the mcp23017 for the encoders controlling the motor speed
// #define instrument_1_motor_rotary_A 2
// #define instrument_1_motor_rotary_B 3

// // pins connected to the 7segment display to show both the motor speed and light frequency
// #define instrument_1_display_din 5
// #define instrument_1_display_clock 6
// #define instrument_1_display_cs 7

struct instrument{
    int LED_pin;
    int LED_rotary_A;
    int LED_rotary_B;

    int LED_state;
    unsigned long LED_previousMillis;
    int LED_hz;

    int motor_A;
    int motor_B;
    int motor_enable;

    int motor_speed_pwm;

    int motor_rotary_A;
    int motor_rotary_B;

    int display_din;
    int display_clock;
    int display_cs; 
}

instrument instrument1 = {
    13,  // LED
    6,  // LED encoder A
    7,  // LED encoder B
    1,  // motor controller A
    2,  // motor controller B
    5, // motor controller enable
    4,  // motor encoder A
    5,  // motor encoder B
    6,  // 7-segment-display din
    7,  // 7-segment-display clock
    8   // 7-segment-display cs
}

instrument instrument2 = {
    7,  // LED
    4,  // LED encoder A
    5,  // LED encoder B
    8,  // motor controller A
    9,  // motor controller B
    A1, // motor controller enable
    6,  // motor encoder A
    7,  // motor encoder B
    10, // 7-segment-display din
    11, // 7-segment-display clock
    12  // 7-segment-display cs
}

instrument instrument3 = {
    13, // LED
    8,  // LED encoder A
    9,  // LED encoder B
    14, // motor controller A
    15, // motor controller B
    A2, // motor controller enable
    10, // motor encoder A
    11, // motor encoder B
    16, // 7-segment-display din
    17, // 7-segment-display clock
    18  // 7-segment-display cs
}

instrument instrument4 = {
    19, // LED
    12, // LED encoder A
    13, // LED encoder B
    20, // motor controller A
    21, // motor controller B
    A3, // motor controller enable
    14, // motor encoder A
    15, // motor encoder B
    22, // 7-segment-display din
    23, // 7-segment-display clock
    24  // 7-segment-display cs
}