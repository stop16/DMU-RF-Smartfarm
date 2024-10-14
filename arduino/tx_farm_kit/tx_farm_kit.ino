#include <ArduinoBLE.h>
#include "Adafruit_CCS811.h"
#include "DHT.h"

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
}

void loop()
{
    // 센서값 최신화
    kitData.soil = readSoilData();

    float arr[2] = {0};
    readDHTData(arr);
    kitData.humi = arr[0];
    kitData.temp = arr[1];

    kitData.co2 = readCCSData();

    // 데이터 전송
    if(ccsInitialized && dhtInitialized) sendData(kitData);
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