#include <Wire.h>
#include <Adafruit_PN532.h>
#include <SPI.h>
#include <SD.h>

#define SD_CS 4           // Chip select pin for onboard SD card
#define SDA_PIN 11        // SDA pin for I2C on MKR Zero
#define SCL_PIN 12        // SCL pin for I2C on MKR Zero

Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);  // Initialize PN532 with I2C
File trackerFile;

void setup() {
  Serial.begin(9600);

  // Initialize SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

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

void loop() {
  trackerFile = SD.open("tracker_log.txt");
  if (!trackerFile) {
    Serial.println("Error opening tracker log file!");
    return;
  }

  // Read and send each line as an NFC message
  while (trackerFile.available()) {
    String logEntry = trackerFile.readStringUntil('\n');
    if (sendNFCMessage(logEntry)) {
      Serial.println("Sent: " + logEntry);
    } else {
      Serial.println("Failed to send: " + logEntry);
    }
    delay(500); // Adjust delay as needed
  }

  trackerFile.close();
  while (1); // End after sending all data
}

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
