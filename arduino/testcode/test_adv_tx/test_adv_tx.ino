#include <ArduinoBLE.h>

BLEService myService("fff0");
BLEIntCharacteristic myCharacteristic("fff1", BLERead | BLEBroadcast);

// Advertising parameters should have a global scope. Do NOT define them in 'setup' or in 'loop'
uint8_t manufactData[4] = {0x01, 0x02, 0x03, 0x04};
uint8_t serviceData[3] = {0x00, 0x01, 0x02};  // This will be updated by Serial input

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("failed to initialize BLE!");
    while (1);
  }

  myService.addCharacteristic(myCharacteristic);
  BLE.addService(myService);

  // Initial setup for BLE advertisement
  updateAdvertisingData();
  BLE.advertise();
  Serial.println("advertising ...");
}

void loop() {
  BLE.poll();

  // Check if there is new data available in the Serial Monitor
  if (Serial.available() > 0) {
    // Read and update advertising data from Serial input
    updateServiceData();
    // Update the advertising data
    updateAdvertisingData();
    Serial.println("Updated advertising data.");
  }
}

// Function to update the advertising data with the current serviceData values
void updateAdvertisingData() {
  // Build scan response data packet
  BLEAdvertisingData scanData;
  scanData.setLocalName("Test enhanced advertising");
  BLE.setScanResponseData(scanData);

  // Build advertising data packet
  BLEAdvertisingData advData;
  advData.setManufacturerData(0x004C, manufactData, sizeof(manufactData));
  advData.setAdvertisedService(myService);
  advData.setAdvertisedServiceData(0xfff0, serviceData, sizeof(serviceData));
  BLE.setAdvertisingData(advData);
}

// Function to update the serviceData from Serial input
void updateServiceData() {
  Serial.println("Enter 3 integers (0-255) separated by space or comma for new service data:");
  
  int inputValues[3];
  int i = 0;
  
  // Parse input values
  while (i < 3) {
    if (Serial.available() > 0) {
      inputValues[i] = Serial.parseInt();  // Read and convert input to an integer
      if (inputValues[i] >= 0 && inputValues[i] <= 255) {
        serviceData[i] = (uint8_t)inputValues[i];  // Update serviceData array
        i++;
      } else {
        Serial.println("Invalid input! Please enter values between 0 and 255.");
      }
    }
  }

  // Print new values for confirmation
  Serial.print("New service data: ");
  for (int j = 0; j < 3; j++) {
    Serial.print(serviceData[j]);
    if (j < 2) Serial.print(", ");
  }
  Serial.println();
}
