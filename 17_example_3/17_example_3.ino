#include <Servo.h>

// 핀 정의
#define PIN_IR_SENSOR     A0    // 적외선 거리 센서를 A0 핀에 연결
#define PIN_SERVO         10   // 서보를 디지털 핀 10에 연결
#define PIN_LED           9    // LED를 디지털 핀 9에 연결

// 서보 PWM 신호 범위 설정 (서보에 따라 다를 수 있음)
#define _DUTY_MIN 500 // 서보의 최대 시계 방향 위치 (0도)
#define _DUTY_MAX 2400  // 서보의 최대 반시계 방향 위치 (180도)

#define LOOP_INTERVAL 20   // 루프 간격 (밀리초)
#define ALPHA 0.3         // EMA 필터의 알파 값

// 서보 객체 생성
Servo myservo;
unsigned long last_loop_time = 0;   // 이전 루프 시간
float filtered_distance = -1;       // EMA 필터가 적용된 거리 값

void setup()
{
  // 서보 초기화 및 초기 위치 설정
  myservo.attach(PIN_SERVO); 
  myservo.writeMicroseconds(_DUTY_MIN);  // 서보를 초기 위치로 설정
  
  // LED 핀을 출력으로 설정
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  // 시리얼 통신 시작
  Serial.begin(1000000);
}

void loop()
{
  unsigned long time_curr = millis();
  int a_ir;
  float distance;
  
  // 다음 이벤트 시간까지 대기
  if (time_curr < (last_loop_time + LOOP_INTERVAL))
    return;
  last_loop_time += LOOP_INTERVAL;

  // 적외선 센서 값 읽기
  a_ir = analogRead(PIN_IR_SENSOR);  // 0 ~ 1023

  // 거리 계산: 비선형 모델 사용
  if (a_ir > 9) {  // 분모가 0이 되지 않도록 9보다 클 때만 계산
    distance = (6762.0 / (a_ir - 9) - 4.0) * 10.0;  // mm 단위
  }
  else {
    distance = -1;  // 유효하지 않은 거리
  }

  // 거리 필터링 (10cm ~ 25cm 범위 외 거리 무시)
  if (distance < 100 || distance > 250) {
    distance = -1;  // 유효하지 않은 거리
  }

  // EMA 필터 적용
  if (distance > 0) {
    if (filtered_distance < 0) {
      filtered_distance = distance;  // 초기화
    } else {
      filtered_distance = ALPHA * distance + (1 - ALPHA) * filtered_distance;
    }
  }

  // 서보 제어: 거리 값에 따라 서보 위치 변경 (10cm ~ 25cm => 0도 ~ 180도)
  int duty = _DUTY_MIN;
  if (filtered_distance > 0) {
    duty = _DUTY_MIN + (filtered_distance - 100) * (_DUTY_MAX - _DUTY_MIN) / (250 - 100);
    myservo.writeMicroseconds(duty);
  }

  // LED 제어: 거리 값이 10cm ~ 25cm 범위에 있을 때 켜기
  if (filtered_distance >= 100 && filtered_distance <= 250) {
    digitalWrite(PIN_LED, HIGH);
  } else {
    digitalWrite(PIN_LED, LOW);
  }

  // 시리얼 출력 
  Serial.print("_DUTY_MIN:"); Serial.print(_DUTY_MIN);
  Serial.print(", _DIST_MIN:"); Serial.print(100);
  Serial.print(", IR:"); Serial.print(a_ir);
  Serial.print(", dist_raw:"); Serial.print(distance);
  Serial.print(", ema:"); Serial.print(filtered_distance);
  Serial.print(", servo:"); Serial.print(duty);
  Serial.print(", _DIST_MAX:"); Serial.print(250);
  Serial.print(", _DUTY_MAX:"); Serial.print(_DUTY_MAX);
  Serial.println("");
} 
