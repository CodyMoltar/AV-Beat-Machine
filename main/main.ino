int led = 13;

int hz = 60;

int enA = 10;
int in1 = 9;
int in2 = 8;

int pot = A0;

void setup() {
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  pinMode(enA, OUTPUT);
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	
	// Turn off motors - Initial state
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);

  pinMode(A0, INPUT);
  Serial.begin(9600);

  digitalWrite(in1, LOW);
	digitalWrite(in2, HIGH);
  analogWrite(enA, 0);

}

void loop() {
  // put your main code here, to run repeatedly:

  // strobe();

  float reading = analogRead(pot);
  float value = mapfloat(reading, 0, 1023, 0, 255);
  Serial.print("original reading: "); 
  Serial.print(reading);
  Serial.print(" and mapped value: ");
  Serial.println(value);

  analogWrite(enA, value);

  digitalWrite(led, HIGH);
  delay(10);
  digitalWrite(led, LOW);
  delay(10);  


}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void directionControl() {
	// Set motors to maximum speed
	// For PWM maximum possible values are 0 to 255
	analogWrite(enA, 255);

	// Turn on motor A & B
	digitalWrite(in1, HIGH);
	digitalWrite(in2, LOW);
	delay(2000);
	
	// Now change motor directions
	digitalWrite(in1, LOW);
	digitalWrite(in2, HIGH);
	delay(2000);
	
	// Turn off motors
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
}

void speedControl() {
	// Turn on motors
	digitalWrite(in1, LOW);
	digitalWrite(in2, HIGH);
	
	// Accelerate from zero to maximum speed
	for (int i = 0; i < 256; i++) {
		analogWrite(enA, i);
		delay(20);
	}
	
	// Decelerate from maximum speed to zero
	for (int i = 255; i >= 0; --i) {
		analogWrite(enA, i);
		delay(20);
	}
	
	// Now turn off motors
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
}

void strobe(){
  if(hz > 1000){
    hz = 1;
  }

  int time = 1000/hz;

  digitalWrite(led, HIGH);
  delay(time);
  digitalWrite(led, LOW);
  delay(time);

  hz++;
}
