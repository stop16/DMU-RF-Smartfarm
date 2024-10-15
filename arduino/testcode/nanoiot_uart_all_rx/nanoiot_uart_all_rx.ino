//수신 (솔레노이드 제어)
String incomingData = ""; // 수신한 전체 문자열 데이터를 저장할 변수
int soil = 0;
float humi = 0.0;
float temp = 0.0;
int co2 = 0;

int solenoid_pin[] = {2, 3, 4, 5};  // 벨브 핀 번호 (1번 벨브는 2번 핀)
int solenoid_state[] = {0, 0, 0, 0};  // 벨브 상태 (0: 닫힘, 1: 열림)

void setup() {
  Serial1.begin(9600); // UART 통신을 위한 시리얼1 포트 초기화, 9600 bps
  Serial.begin(9600);

  // 벨브 핀을 출력 모드로 설정
  pinMode(solenoid_pin[0], OUTPUT);
  digitalWrite(solenoid_pin[0], LOW);  // 초기 상태는 닫힘
}

void loop() {
  if (Serial1.available() > 0) {
    incomingData = Serial1.readStringUntil('\n');  // 개행문자('\n')가 나올 때까지 데이터를 읽음
    parseData(incomingData);  // 데이터를 파싱하여 변수에 저장
    control_solenoids(soil);  // 파싱한 soil 값을 이용해 벨브 제어
  }
  delay(1000);
}

void parseData(String data) {
  int commaIndex1 = data.indexOf(',');  // 첫 번째 ',' 위치
  int commaIndex2 = data.indexOf(',', commaIndex1 + 1);  // 두 번째 ',' 위치
  int commaIndex3 = data.indexOf(',', commaIndex2 + 1);  // 세 번째 ',' 위치
  
  // 각각의 데이터를 ','로 구분하여 파싱하고 변수에 저장
  soil = data.substring(0, commaIndex1).toInt();
  humi = data.substring(commaIndex1 + 1, commaIndex2).toFloat();
  temp = data.substring(commaIndex2 + 1, commaIndex3).toFloat();
  co2 = data.substring(commaIndex3 + 1).toInt();
  
  // 저장된 값을 시리얼 모니터에 출력 (확인용)
  Serial.print("Soil: ");
  Serial.println(soil);
  Serial.print("Humidity: ");
  Serial.println(humi);
  Serial.print("Temperature: ");
  Serial.println(temp);
  Serial.print("CO2: ");
  Serial.println(co2);
}

void control_solenoids(int soil_value) {
    if (soil_value < 40) { // soil 값이 40 이하이면
        if (solenoid_state[0] == 0) {  // 1번 벨브만 제어
            solenoid_state[0] = 1;
            digitalWrite(solenoid_pin[0], HIGH);  // 1번 벨브 열기
            Serial.println("VALVE 1 ON");
        }
    } else if (soil_value >= 45) { // soil 값이 45 이상이면
        if (solenoid_state[0] == 1) {  // 1번 벨브만 제어
            solenoid_state[0] = 0;
            digitalWrite(solenoid_pin[0], LOW);  // 1번 벨브 닫기
            Serial.println("VALVE 1 OFF");
        }
    }
}
