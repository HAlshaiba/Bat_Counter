/**
/* Created on 2024-11-10 by Amin Khaled


/* This file logs data captured by a single IR sensor
  to a local SD card on an MKR Arduino Zero board


/* The current setup assumes we are only counting bat
entrances, so a counter (tally) is incremented everytime
a motion is detected by an IR sensor


/* Last Updated by: Amin Khaled on 2024-11-11
/* Added the inteface of NFC chip to allow reading .txt
files on the local SD card
**/


#include <Wire.h>
#include <Adafruit_PN532.h>
#include <SPI.h>
#include <SD.h>


const int chipSelect = SDCARD_SS_PIN;
#define SDA_PIN 11        // SDA pin for I2C on MKR Zero
#define SCL_PIN 12        // SCL pin for I2C on MKR Zero

const int trigPin = 9;
const int echoPin = 10;


Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);  // Initialize PN532 with I2C


// variables
long duration;
int distance;
int baseline = 21;
int batTally = 0;
bool batDetected = false;
File dataFile;
int incomingByte = 0 ; // to read incoming serial data




void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
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


  /* REMOVE COMMENT (NFC)
  // Initialize NFC over I2C
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Didn't find PN53x board");
    while (1);
  }
  nfc.SAMConfig();
  Serial.println("NFC initialized.");
  }
  */


}


void loop() {
  // make a string for assembling the data to log:
  String dataString = "";


  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);


  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(4);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(20);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  if (distance < baseline){
    if (!batDetected){
    batDetected = true;
    batTally += 1;
    dataString += "Bat Detected. Bat count: \n" + String(batTally);
    Serial.println(dataString);

     // if the file is available, write to it:
    if (dataFile.available()) {
      dataFile.println(dataString);
      // print to the serial port too:
      Serial.println(dataString);


      /* REMOVE COMMENT (NFC)
      if (sendNFCMessage(dataString)) {
        Serial.println("Sent: " + dataString);
      } else {
        Serial.println("Failed to send: " + dataString);
      }
      delay(500); // Adjust delay as needed
      */


    }
    // if the file isn't open, pop up an error:
    else {
    Serial.println("error opening datalog.txt");
    }
    }
  }
  else {
    batDetected = false;

  }
  Serial.print("Distance: ");
  Serial.println(distance);
  dataFile.print("Distance ");
  dataFile.println(distance);

  delay(50); // small delay for reading stability



  // close .txt file upon request
  if (Serial.available() > 0){
    // read incoming byte
    incomingByte = Serial.read();


    if (incomingByte == 7){
      dataFile.close();
    }
  }
}


/* REMOVE COMMENT (NFC)
bool sendNFCMessage(String message) {
  uint8_t ndefData[128];
  int msgLen = message.length();
  message.toCharArray((char *)ndefData, msgLen + 1); // Convert to char array


  // Prepare buffers for response data
  uint8_t response[128];
  uint8_t responseLength = sizeof(response);


  // Send NFC message over I2C
  if (nfc.inDataExchange(ndefData, msgLen, response, &responseLength)) {
    return true;
  }
  return false;
}
*/
