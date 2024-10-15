#include <ArduinoBLE.h>

// Variables setup
typedef struct{
    uint8_t boardNum;
    uint8_t soil;
    float humi;
    float temp;
    uint16_t co2;
}kit_data;
kit_data kitData[4];

uint8_t solenoid_pin[4] = {2,3,4,5}; // Valve pin number
uint8_t solenoid_state[4] = {0};

String uart_string = "";

bool data_updated[4] = {false, false, false, false};

void setup() {
  // put your setup code here, to run once:
  
  // Serial setup
  Serial.begin(9600);
  Serial1.begin(9600);
  while (!Serial);
  while (!Serial1);

  // BLE setup
  BLE.begin();
  BLE.scanForUuid("fff0");

  // Valve initialization
  for(uint8_t i = 0; i < 4; i++){
    pinMode(solenoid_pin[i], OUTPUT);
    digitalWrite(solenoid_pin[i], LOW);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  BLEDevice peripheral = BLE.available();

  if (Serial1.available() > 0){
    byte data = Serial1.read();
    if(data == '\n'){
        parseUartData(uart_string);
        uart_string = "";
    }
    else uart_string += (char)data;
  }

  if(peripheral){
    int advDataLen = peripheral.advertisementDataLength();
    byte advData[advDataLen];
    peripheral.advertisementData(advData, advDataLen);
    parseServiceData(advData, advDataLen);
    BLE.scan(); // 다시 스캔
  }

  controlSolenoids();
  sendData(kitData);

}

void parseUartData(String data){
    String parsed_value = "";
    uint8_t seq = 0;
    uint8_t current_board = 0;
    for(int i = 0; i<data.length(); i++){
        if(data[i] == ',') {
            switch(seq){
                case 0:
                    current_board = parsed_value.toInt();
                    data_updated[current_board] = true;
                    kitData[current_board].boardNum = current_board;
                    break;
                case 1:
                    kitData[current_board].soil = parsed_value.toInt();
                    break;
                case 2:
                    kitData[current_board].humi = parsed_value.toFloat();
                    break;
                case 3:
                    kitData[current_board].temp = parsed_value.toFloat();
                    break;
                case 4:
                    kitData[current_board].co2 = parsed_value.toInt();
                    break;
                default:
                    break;
            }
            if(seq<4) seq++;
            else seq = 0;
            parsed_value = "";
        }
        else parsed_value += data[i];
    }
}

void parseServiceData(byte* advData, int advDataLen){
    uint8_t incoming_data[8] = {0};
    int index = 0;
    while(index < advDataLen){
        int length = advData[index];
        if (length == 0) break;

        byte type = advData[index + 1];
        if(type == 0x16){
        for(int i = index+4; i< index+length+1; i++){
            incoming_data[i - (index+4)] = advData[i]; // update adv data
        }
        }
        index +=  length +  1;
    }
    kitData[incoming_data[0]].boardNum = incoming_data[0];
    kitData[incoming_data[0]].soil = incoming_data[1];
    kitData[incoming_data[0]].humi = incoming_data[2] + incoming_data[3]/100;
    kitData[incoming_data[0]].temp = incoming_data[4] + incoming_data[5]/100;
    kitData[incoming_data[0]].co2 = incoming_data[6] << 8 + incoming_data[7];
    data_updated[incoming_data[0]] = true;
}

void sendData(kit_data data) {
    String data_to_send = (String) data.boardNum + "," + (String) data.soil + "," + (String) data.humi + "," + (String) data.temp + "," + (String) data.co2;
    Serial.println(data_to_send);
    Serial1.println(data_to_send);
}

void controlSolenoids(){
    for(uint8_t i = 0; i < 4; i++) {
        if(kitData[i].soil < 40 && data_updated[i] == true) {
            if(solenoid_state[i] == 0) {
                solenoid_state[i] = 1;
                digitalWrite(solenoid_pin[i], HIGH);
            }
        }
        else if(kitData[i].soil >= 45 && data_updated[i] == true) {
            if(solenoid_state[i] == 1) {
                solenoid_state[i] = 0;
                digitalWrite(solenoid_pin[i], LOW);
            }
        }
    }
}