#include <ArduinoBLE.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);
  BLE.begin();
  BLE.scanForUuid("fff0");
}

void loop() {
  // put your main code here, to run repeatedly:
  BLEDevice peripheral = BLE.available();

  if(peripheral){
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();
    
    int advDataLen = peripheral.advertisementDataLength();
    byte advData[advDataLen];
    peripheral.advertisementData(advData, advDataLen);

    Serial.println("Adv Data: ");
    for (int i=0; i<advDataLen; i++){
      Serial.print(advData[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    parseServiceData(advData, advDataLen);

    BLE.scan();
  }
}

void parseServiceData(byte* advData, int advDataLen){
  int index = 0;
  while(index < advDataLen){
    int length = advData[index];
    if (length == 0) break;

    byte type = advData[index + 1];
    if(type == 0x16){
      Serial.print(advData[index + 3], HEX);
      Serial.print(" ");
      Serial.print(advData[index + 2], HEX);
      Serial.println();

      Serial.print("Service Data: ");
      for(int i = index+4; i< index+length+1; i++){
        Serial.print(advData[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
    index +=  length +  1;
  }
}
