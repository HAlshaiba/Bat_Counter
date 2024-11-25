#include <SD.h>
#include <SPI.h>

const int chipSelect = SDCARD_SS_PIN;   // Use MKR Zero's SD card pin
const int BUTTON_PIN = 2;               // Pin where the button is connected

void setup() {
  // Initialize Serial for debugging and Serial1 for Bluetooth
  Serial.begin(9600);
  Serial1.begin(9600);                   // Bluetooth communication

  pinMode(BUTTON_PIN, INPUT_PULLUP);     // Set button pin as input with pull-up resistor

  // Initialize SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    while (true);                        // Stop if SD card fails to initialize
  }
  Serial.println("SD card initialized.");
}

void loop() {
  // Check if button is pressed (LOW for INPUT_PULLUP)
  if (digitalRead(BUTTON_PIN) == LOW) {
    // Open the file on the SD card
    File dataFile = SD.open("DATALOG.txt");  // Replace "example.txt" with your file name

    if (dataFile) {
      Serial.println("Reading file...");
      // Read each line from the file and send it via Bluetooth
      while (dataFile.available()) {
        String line = dataFile.readStringUntil('\n'); // Read one line at a time
        Serial1.println(line);                        // Send line over Bluetooth
        Serial.println(line);                         // Print line to Serial Monitor
      }
      dataFile.close();                               // Close the file
      Serial.println("File sent!");
    } else {
      Serial.println("Error opening file!");
    }

    // Wait for button to be released to avoid multiple triggers
    while (digitalRead(BUTTON_PIN) == LOW);
  }
}
