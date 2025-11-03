// Experiment 1: 0°→180° 구간을 3deg/s로 등속 (편도 60초)
#include <Servo.h>

// Arduino pin assignment
#define PIN_SERVO 10

// configurable parameters
#define _DUTY_MIN 600    // 0°
#define _DUTY_NEU 1560   // 90°
#define _DUTY_MAX 2490   // 180°

#define _POS_START (_DUTY_MIN + 100)    
#define _POS_END   (_DUTY_MAX - 100)

#define _SERVO_SPEED 3.0f   // deg/s (편도 60초)
#define INTERVAL 20         // msec

// global variables
unsigned long last_sampling_time; // msec
unsigned long phase_start_ms;     // 현재 편도 구간이 시작된 시각
unsigned long move_ms;            // 한 편도 이동 시간(ms)
int dir = +1;                     // +1: START->END, -1: END->START

Servo myservo;

void setup() {
  myservo.attach(PIN_SERVO);
  Serial.begin(57600);

  // 한 편도(180°)에 걸리는 시간(ms) = (180/_SERVO_SPEED)*1000
  move_ms = (unsigned long)((180.0f / _SERVO_SPEED) * 1000.0f + 0.5f);

  // 시작: START 위치에서 즉시 연속 이동 시작
  myservo.writeMicroseconds(_POS_START);
  phase_start_ms = millis();
  last_sampling_time = phase_start_ms;

  Serial.print("move_ms="); Serial.println(move_ms); // ≈60000ms
}

void loop() {
  // 주기 맞춰 갱신
  unsigned long now = millis();
  if (now - last_sampling_time < INTERVAL) return;
  last_sampling_time = now;

  // 이번 편도에서 경과 시간 t (0..move_ms)
  unsigned long t = now - phase_start_ms;
  if (t > move_ms) t = move_ms;

  int a = (dir > 0) ? _POS_START : _POS_END;  // 출발점
  int b = (dir > 0) ? _POS_END   : _POS_START; // 도착점
  float alpha = (float)t / (float)move_ms;     // 0.0 → 1.0
  int duty = (int)(a + (b - a) * alpha + 0.5f);
  myservo.writeMicroseconds(duty);

  // 편도 완료 시점에 방향 반전
  if (t >= move_ms) {
    dir = -dir;                // 방향 뒤집기
    phase_start_ms = now;      // 새 편도 시작 시간
  }
}
