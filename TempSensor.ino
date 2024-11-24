#include <DHT.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>

File dataFile;
int incomingByte = 0 ; // to read incoming serial data

const int chipSelect = SDCARD_SS_PIN;


#define DHTPIN A0

#define DHTTYPE DHT11 // DHT 11

DHT dht(DHTPIN, DHTTYPE);


void setup(){
  Serial.begin(9600);

  dht.begin();

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




  





void loop(){


  // make a string for assembling the data to log:
  String dataString = "";
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  
  File dataFile = SD.open("datalog.txt", FILE_WRITE);


    



    float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
  Serial.println("Failed to read from DHT sensor!");
  return;
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!


  dataString += "Humidity: "+ String(h) + "%\t" ;
  dataString += "Temperature: " + String(t) + "°C\n";

  

// if the file is available, write to it:
      if (dataFile) {
        dataFile.println(dataString);
        dataFile.close();
        // print to the serial port too:
        Serial.println(dataString);
      }
      // if the file isn't open, pop up an error:
      else {
      Serial.println("error opening datalog.txt");
      }


  delay(21600000);
  
}