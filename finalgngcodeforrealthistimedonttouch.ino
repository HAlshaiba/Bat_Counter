// The following are installed libraries, except for <Wire.h>
#include <DHT.h>
#include <RTCZero.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>

#define OUTER 5
#define INNER 4
#define EXIT 3 
#define BUTTON_PIN 2
#define DHTPIN A0
#define DHTTYPE DHT11 // defines the type of tmp sensort (DHT 11)
#define CHIP_SELECT SDCARD_SS_PIN

RTCZero rtc;
DHT dht(DHTPIN, DHTTYPE);

// Initial settings
const byte seconds = 0;
const byte minutes = 0;
const byte hours = 0;
const byte day = 0;
const byte month = 0;
const byte year = 0;

int batTally = 0;
int entries = 0;
int exits = 0;
bool batDetectedS3 = false;
String dataString = ""; // record of temp and humidity

File dataFile;
int incomingByte = 0 ; // to read incoming serial data

// State variables
enum State { IDLE, OUTER_TRIGGERED, INNER_TRIGGERED };
State currentState = IDLE;

void setup() {
  pinMode(OUTER, INPUT_PULLUP);
  pinMode(INNER, INPUT_PULLUP);
  pinMode(EXIT, INPUT_PULLUP);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  rtc.begin();
  rtc.setHours(hours);
  rtc.setMinutes(minutes);
  rtc.setSeconds(seconds);
  rtc.setDay(day);
  rtc.setMonth(month);
  rtc.setYear(year);

  Serial.begin(9600);
  Serial1.begin(9600); // Bluetooth communication
  
  dht.begin();
  
  //while (!Serial) {
    //; // Wait for Serial port to connect (Native USB)
  //}
  

  Serial.print("Initializing SD card...");
  if (!SD.begin(CHIP_SELECT)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  Serial.println("Card initialized.");
}

void loop() {
  checkSensors();
  readTemp();
  if (digitalRead(BUTTON_PIN) == LOW) {
    // log the data every 3 mins (every multiple of 3 minutes)
    if (seconds == 0 && minutes % 3 == 0){
      logData();
    }
    // if you wanted to change the logging period to 6 hours, uncomment the following:
    /*if (seconds  == 0 && minutes == 0 && hours % 6 = 0){
      logData();
    }*/
    while (digitalRead(BUTTON_PIN) == LOW); // Wait until button is released
  }
}

bool prevOUTERState = false; // Previous state of OUTER sensor
bool prevINNERState = false; // Previous state of INNER sensor
bool prevEXITState = false;  // Previous state of EXIT sensor

void checkSensors() {
  // Read current sensor states
  bool OUTERState = digitalRead(OUTER) == LOW;
  bool INNERState = digitalRead(INNER) == LOW;
  bool EXITState = digitalRead(EXIT) == LOW;

  // OUTER sensor logic
  if (OUTERState && !prevOUTERState) { // Triggered only on state change
    Serial.println("Sensor 1 triggered");
    currentState = OUTER_TRIGGERED;
  }

  // INNER sensor logic
  if (INNERState && !prevINNERState) { // Triggered only on state change
    Serial.println("Sensor 2 triggered");
    currentState = INNER_TRIGGERED;
  }

  // EXIT sensor logic
  if (EXITState && !prevEXITState) { // Triggered only on state change
    if (!batDetectedS3) {
      batDetectedS3 = true;
      batTally--;
      exits++;
      Serial.println("Bat exited");
    }
  } else if (!EXITState) {
    batDetectedS3 = false; // Reset when sensor is untriggered
  }

  // Update previous states
  prevOUTERState = OUTERState;
  prevINNERState = INNERState;
  prevEXITState = EXITState;

  // FSM logic for OUTER and INNER
  switch (currentState) {
    case OUTER_TRIGGERED:
      if (INNERState) {
        batTally++;
        entries++;
        Serial.println("Bat entered");
        currentState = IDLE; // Reset to IDLE after valid entry
      } else if (!OUTERState) {
        currentState = IDLE; // Reset if OUTER is no longer blocked
      }
      break;

    case INNER_TRIGGERED:
      if (OUTERState) {
        batTally--;
        exits++;
        Serial.println("Bat exited");
        currentState = IDLE; // Reset to IDLE after valid exit
      } else if (!INNERState) {
        currentState = IDLE; // Reset if INNER is no longer blocked
      }
      break;

    default:
      break;
  }
}


void logData() {
  File dataFile = SD.open("bat_data.txt", FILE_WRITE);
  if (dataFile) {
    String timestamp = getTimestamp();
    dataFile.println(timestamp);
    dataFile.println("Bats inside: " + String(batTally));
    dataFile.println("Entries: " + String(entries));
    dataFile.println("Exits: " + String(exits));
    dataFile.println("");
    dataFile.println(dataString);
    dataFile.close();

    // Feedback to Serial and Bluetooth
    Serial.println(timestamp);
    Serial.println("Bats inside: " + String(batTally));
    Serial.println("Entries: " + String(entries));
    Serial.println("Exits: " + String(exits));
    Serial.println("");
    Serial.println(dataString);
    Serial1.println("Bats inside: " + String(batTally));
    Serial1.println("Entries: " + String(entries));
    Serial1.println("Exits: " + String(exits));
    Serial1.println("");
    Serial1.println(dataString);
  } else {
    Serial.println("Error opening bat_data.txt");
  }
}

String getTimestamp() {
  String timestamp = "";
  timestamp += String(rtc.getDay()) + "/";
  timestamp += String(rtc.getMonth()) + "/";
  timestamp += String(rtc.getYear()) + " ";
  timestamp += String(rtc.getHours()) + ":";
  timestamp += String(rtc.getMinutes()) + ":";
  timestamp += String(rtc.getSeconds());
  return timestamp;
}

/* 
This method reads the temperature and humidity from the DHT11 sensor, and
records the data in a string format (dataString)
*/
void readTemp(){
  dataString = "";
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // in Celsius
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
  Serial.println("Failed to read from DHT sensor!");
  return;
  }
  dataString += "Humidity: "+ String(h) + "%\t" ;
  dataString += "Temperature: " + String(t) + " Degrees Celsius\n";
  
}

