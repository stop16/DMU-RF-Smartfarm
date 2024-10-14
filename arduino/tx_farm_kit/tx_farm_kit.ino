#include <ArduinoBLE.h>
#include "Adafruit_CCS811.h"
#include "DHT.h"

BLEService myService("fff0");
BLEIntCharacteristic myCharacteristic("fff1", BLERead | BLEBroadcast);

// Advertising parameters should have a global scope. Do NOT define them in 'setup' or in 'loop'
uint8_t manufactData[4] = {0x01, 0x02, 0x03, 0x04};
uint8_t serviceData[7] = {0x00};  // soil, humi(int), humi(float), temp(int), temp(float), co2(big), co2(small)

// Macro
#define DHTPIN 2        
#define DHTTYPE DHT22   

// Sensor setup
DHT dht(DHTPIN, DHTTYPE);
Adafruit_CCS811 ccs;

// Variables setup
typedef struct {
    uint8_t soil;
    float humi;
    float temp;
    uint16_t co2;
} sensor_data;

bool ccsInitialized = false; // CCS811 센서 초기화 여부
bool dhtInitialized = false; // DHT22 센서 초기화 여부
sensor_data kitData;

void setup()
{
    Serial.begin(9600);
    while(!Serial);

    if(!BLE.begin()){
        Serial.println("failed to initialize BLE!");
        while (1);
    }

    myService.addCharacteristic(myCharacteristic);
    BLE.addService(myService);

    dht.begin();
    dhtInitialized = true;

    while (!ccsInitialized) {
    if (ccs.begin()) {
        ccsInitialized = true;
        Serial.println("CCS811 sensor initialized.");
    } else {
        Serial.println("Failed to start CCS811 sensor! Retrying...");
        delay(1000);  // 1초 대기 후 재시도
    }
    }

    updateAdvertisingData();
    BLE.advertise();
    Serial.println("advertising ...");
}

void loop()
{
    BLE.poll();
    
    // 센서값 최신화
    kitData.soil = readSoilData();

    float arr[2] = {0};
    readDHTData(arr);
    kitData.humi = arr[0];
    kitData.temp = arr[1];

    kitData.co2 = readCCSData();

    // 데이터 전송
    if(ccsInitialized && dhtInitialized) {
        sendData(kitData);
        BLE.stopAdvertise();
        updateAdvertisingData();
        BLE.advertise();
    }
}

uint8_t readSoilData(void){
    return map(analogRead(A0), 0, 1023, 0, 100);
}

void readDHTData(float *arr){
    float humi = dht.readHumidity();
    float temp = dht.readTemperature();
    arr[0] = humi;
    arr[1] = temp;
}

uint16_t readCCSData(void){
    if(ccs.available()){
        if(!ccs.readData()){
            return ccs.geteCO2();
        }
        else return 0;
    }
    else return 0;
}

void sendData(sensor_data data){
    Serial.print(data.soil);
    Serial.print(",");
    Serial.print(data.humi);
    Serial.print(",");
    Serial.print(data.temp);
    Serial.print(",");
    Serial.print(data.co2);
    Serial.println();
}

void updateAdvertisingData(){
    BLEAdvertisingData scanData;
    scanData.setLocalName("Farm_kit_01");
    BLE.setScanResponseData(scanData);

    // Build advertising data packet
    BLEAdvertisingData advData;
    advData.setManufacturerData(0x004C, manufactData, sizeof(manufactData));
    advData.setAdvertisedService(myService);
    advData.setAdvertisedServiceData(0xfff0, serviceData, sizeof(serviceData));
    BLE.setAdvertisingData(advData);
}

void updateServiceData(){
    serviceData[0] = kitData.soil;
    serviceData[1] = (uint8_t) kitData.humi;
    serviceData[2] = (uint8_t) ((kitData.humi - (int) kitData.humi)*100);
    serviceData[3] = (uint8_t) kitData.temp;
    serviceData[4] = (uint8_t) ((kitData.temp - (int) kitData.temp)*100);
    serviceData[5] = kitData.co2 & 0xFF00;
    serviceData[6] = kitData.co2 & 0x00FF;
}