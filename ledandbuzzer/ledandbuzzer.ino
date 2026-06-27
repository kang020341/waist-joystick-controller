#include <SoftwareSerial.h>



SoftwareSerial mySerial(4, 3); // RX=4, TX=3



const int CENTER_X = 507;

const int CENTER_Y = 503;

const int DEADZONE = 30;

const int TRIG_PIN = 7;

const int ECHO_PIN = 8;

const int BUZZER_PIN = 11;

unsigned long buzzerTimer = 0;

bool buzzerState = false;

// LED 핀

const int LED_F = 5;

const int LED_B = 6;

const int LED_L = 9;

const int LED_R = 10;



void setup() {

  Serial.begin(9600);

  mySerial.begin(9600);



  pinMode(LED_F, OUTPUT);

  pinMode(LED_B, OUTPUT);

  pinMode(LED_L, OUTPUT);

  pinMode(LED_R, OUTPUT);


  pinMode(TRIG_PIN, OUTPUT);

  pinMode(ECHO_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);
}

int getDistance() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);

  int distance = duration * 0.034 / 2;

  return distance;
}


void loop() {



  while (mySerial.available()) {

    byte b = mySerial.read();
    


    if (b == 'S') {



      while (mySerial.available() < 5);



      byte xH = mySerial.read();

      byte xL = mySerial.read();

      byte yH = mySerial.read();

      byte yL = mySerial.read();

      byte clickState = mySerial.read();



      int xVal = (xH << 8) | xL;

      int yVal = (yH << 8) | yL;



      Serial.print("X=");

      Serial.print(xVal);

      Serial.print(" Y=");

      Serial.print(yVal);

      Serial.print(" Click=");

      Serial.println(clickState);



      // LED 초기화

      analogWrite(LED_F, 0);

      analogWrite(LED_B, 0);

      analogWrite(LED_L, 0);

      analogWrite(LED_R, 0);
      
      noTone(BUZZER_PIN);

      // X축 처리

      if (xVal > CENTER_X + DEADZONE) {



        int bright = map(xVal,

                         CENTER_X + DEADZONE,

                         1023,

                         0,

                         255); //오른쪽



        bright = constrain(bright, 0, 255);



        analogWrite(LED_R, bright);

      }

      else if (xVal < CENTER_X - DEADZONE) {



        int bright = map(xVal, 

                         CENTER_X - DEADZONE,

                         0,

                         0,

                         255); //왼쪽



        bright = constrain(bright, 0, 255);



        analogWrite(LED_L, bright);

      }



      // Y축 처리

      if (yVal > CENTER_Y + DEADZONE) {



        int bright = map(yVal,

                         CENTER_Y + DEADZONE,

                         1023,

                         0,

                         255); //전진



        bright = constrain(bright, 0, 255);



        analogWrite(LED_F, bright);

      }

      else if (yVal < CENTER_Y - DEADZONE) {
        
        unsigned long interval;


        int bright = map(yVal,

                         CENTER_Y - DEADZONE,

                         0,

                         0,

                         255);     //후진



        bright = constrain(bright, 0, 255);



        analogWrite(LED_B, bright);

        int distance = getDistance();
        Serial.print("Distance = ");
        Serial.print(distance);
        Serial.println(" cm");

        if (distance > 0 && distance < 10) {
          interval = 50;      // 매우 빠름
        }
        else if (distance < 20) {
          interval = 150;     // 보통
        }
        else if (distance < 30) {
          interval = 300;     // 느림
        }
        else {
          noTone(BUZZER_PIN);
          buzzerState = false;
          return;   // 30cm 이상이면 여기서 종료
        }

        if (millis() - buzzerTimer >= interval) {
          buzzerTimer = millis();
          buzzerState = !buzzerState;
          if (buzzerState)
            tone(BUZZER_PIN, 2000);
          else
            noTone(BUZZER_PIN);
        }
      }
    }

  }

}