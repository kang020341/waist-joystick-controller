#include <SoftwareSerial.h>

const int JOY_X = A0;  //X축 연결 핀
const int JOY_Y = A1;  //Y축 연결 핀
const int JOY_SW = 2;  //스위치 버튼 연결 핀 

//실제 측정된 조이스틱의 중앙값 설정
const int CENTER_X = 507;
const int CENTER_Y = 503;

int clickState = 0;         // 현재 클릭 상태 (0 또는 1)
bool lastButtonState = HIGH; // 이전에 버튼이 눌려있었는지 여부

SoftwareSerial mySerial(4, 3); // RX는 4번(사용안함), TX는 3번(슬레이브 전송용)

void setup() {
  Serial.begin(9600);    
  mySerial.begin(9600);  
  
  pinMode(JOY_SW, INPUT_PULLUP); // 2번 핀 내부 풀업 설정
  
  Serial.println("--- 조이스틱 활성화 제어 마스터 송신 시작 ---");
}

void loop() {
  // 1. 디지털 2번 핀 클릭 상태 감지 및 토글
  bool currentButtonState = digitalRead(JOY_SW);
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    if (clickState == 0) {
      clickState = 1;
    } else {
      clickState = 0;
    }
    Serial.print("-> [클릭 상태 변경] Click State: ");
    Serial.println(clickState);
  }
  lastButtonState = currentButtonState;

  // 2. 조이스틱의 실제 아날로그 값 읽기
  int xVal = analogRead(JOY_X);
  int yVal = analogRead(JOY_Y);

  // 핵심 기능: 클릭 상태가 0(비활성화)이면 측정된 중앙값으로 강제 고정
  if (clickState == 0) {
    xVal = CENTER_X;
    yVal = CENTER_Y;
  }

  // 3. PC 시리얼 모니터 출력
  Serial.print("X-Axis: ");
  Serial.print(xVal);
  Serial.print("\t|\tY-Axis: ");
  Serial.print(yVal);
  Serial.print("\t|\tClick State: ");
  Serial.println(clickState);

  // 4. [데이터 송신부] 슬레이브 아두이노 보드로 데이터 전송
  mySerial.write('S');                 // 데이터 시작 알림 
  mySerial.write(highByte(xVal));      // X축 상위 바이트
  mySerial.write(lowByte(xVal));       // X축 하위 바이트
  mySerial.write(highByte(yVal));      // Y축 상위 바이트
  mySerial.write(lowByte(yVal));       // Y축 하위 바이트
  mySerial.write(clickState);          // 클릭 상태 (0 또는 1)

  delay(100); 
}