#include <RTCZero.h>
#include <SD.h>
#include <SPI.h>
#define SENSOR1 4
#define SENSOR2 5
RTCZero rtc;

const byte seconds = 0;
const byte minutes = 33;
const byte hours = 12;
const byte day = 13;
const byte month = 11;
const byte year = 24;
const int chipSelect = SDCARD_SS_PIN;
const int BUTTON_PIN = 2;               // Pin where the button is connected


int batTally = 0;
int entries = 0;
int exits = 0;
String currTime = "";


void setup() {
  pinMode(SENSOR1, INPUT);
  pinMode(SENSOR2, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  // turn on the pullups
  digitalWrite(SENSOR1, HIGH); 
  digitalWrite(SENSOR2, HIGH); 
  rtc.begin();
  rtc.setHours(hours);
  rtc.setMinutes(minutes);
  rtc.setSeconds(seconds);
  rtc.setDay(day);
  rtc.setMonth(month);
  rtc.setYear(year);
  Serial.begin(9600);
  Serial1.begin(9600);                   // Bluetooth communication
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }




  Serial.print("Initializing SD card...");


  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
}

void loop() {

  if (!digitalRead(SENSOR1)){
    delay(50); // delay is for me to type
    if (!digitalRead(SENSOR2)){
      batTally++;
      entries++;
    }
  }

  if (!digitalRead(SENSOR2)){
    delay(50);
    if (!digitalRead(SENSOR1)){
      batTally--;
      exits++;
    }
  }
  currTime += print2digits(rtc.getDay());
  currTime += ("/");
  currTime += (rtc.getMonth());
  currTime += ("/");
  currTime += (rtc.getYear());
  currTime += (" ");
  currTime += (rtc.getHours());
  currTime += (":");
  currTime += (rtc.getMinutes());
  currTime += (":");
  currTime += (rtc.getSeconds());

  if (digitalRead(BUTTON_PIN) == LOW) {
    File dataFile = SD.open("Nov14.txt", FILE_WRITE);
  // if the file is available, write to it:
      if (dataFile) {
        Serial.println(currTime);
        dataFile.println(currTime);
        currTime = "";
        Serial.println("bats inside: " + (String) batTally);
        Serial.println("entries: " + (String) entries);
        Serial.println("exits: " + (String)exits);
        dataFile.println("bats inside: " + (String) batTally);
        dataFile.println("entries: " + (String) entries);
        dataFile.println("exits: " + (String)exits);
        Serial1.println("bats inside: " + (String) batTally);
        Serial1.println("entries: " + (String) entries);
        Serial1.println("exits: " + (String)exits);
        dataFile.close();
      }
      // if the file isn't open, pop up an error:
      else {
      Serial.println("error opening datalog.txt");
      }
      while (digitalRead(BUTTON_PIN) == LOW);
  }
}

String print2digits(int number) {
  String num = "";
  if (number < 10) {
    num += "0"; // add a 0 before if the number is < than 10
  }
  num += (String) number;
  return (num);
}