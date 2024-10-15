#include "Adafruit_CCS811.h"
#include "DHT.h"

#define DHTPIN 2        // SDA 핀의 설정
#define DHTTYPE DHT11   // DHT22 (AM2302) 센서종류 설정

DHT dht(DHTPIN, DHTTYPE);
Adafruit_CCS811 ccs;

void setup() {
  Serial1.begin(9600); // UART 통신을 위한 시리얼 포트 초기화, 9600 bps
  dht.begin();
  Serial.begin(9600);
//  if(!ccs.begin()){
//    Serial.println("Failed to start sensor! Please check your wiring.");
//    while(1);
//  }
// 
//  // 센서가 준비될 때까지 대기
//  while(!ccs.available());
//  delay(5000);

}

void loop() {
  int co2 = 0;  // 현재 CO2 센서가 연결되지 않았으므로 0으로 처리, 나중에 센서 연결 시 수정
  int TVOC;     // 추후 사용을 위한 변수 선언
  int soil;     // 토양 센서 값
  soil = analogRead(A0); // 토양 센서를 아날로그 핀 A0에 연결
  delay(100);

  float Humi = dht.readHumidity();      // 습도 값 읽기
  float Temp = dht.readTemperature();   // 온도 값 읽기

  // 데이터 전송
  Serial1.print(soil);
  Serial1.print(",");
  Serial1.print(Humi);
  Serial1.print(",");
  Serial1.print(Temp);
  Serial1.print(",");
  Serial1.println(co2);  // 현재 CO2 값은 0으로 전송
  Serial.print(soil);
  Serial.print(",");
  Serial.print(Humi);
  Serial.print(",");
  Serial.print(Temp);
  Serial.print(",");
  Serial.println(co2);  // 현재 CO2 값은 0으로 전송
  delay(1000);
}
