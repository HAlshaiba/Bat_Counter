#define LEDPIN 13
  // Pin 13: Arduino has an LED connected on pin 13
#define SENSORPIN 4

// variables will change:
int sensorState = 0, lastState=0;         // variable for reading the pushbutton status
int batTally = 0;
bool batDetected = false;

void setup() {
  // initialize the LED pin as an output:
  pinMode(LEDPIN, OUTPUT);
  // initialize the sensor pin as an input:
  pinMode(SENSORPIN, INPUT);
  digitalWrite(SENSORPIN, HIGH); // turn on the pullup

  Serial.begin(9600);
}

void loop(){
  // read the state of the pushbutton value:
  sensorState = digitalRead(SENSORPIN);

  // check if the sensor beam is broken
  // if it is, the sensorState is LOW:
  if (sensorState == LOW) {
    // turn LED on:
    digitalWrite(LEDPIN, HIGH);
  } 
  else {
    // turn LED off:
    digitalWrite(LEDPIN, LOW); 
  }

  if (sensorState && !lastState) {
    batDetected = false;
  } 
  if (!sensorState && lastState) {
    if (!batDetected){
    batDetected = true;
    batTally += 1;
    Serial.println("Bat Detected. Bat count: ");
    Serial.println(batTally);
    }
  }
  lastState = sensorState;
  delay(80);
}