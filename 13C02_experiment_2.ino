// Experiment 2: 0°→90° 구간을 0.3 deg/s로 등속 (편도 300초)
#include <Servo.h>


#define _DUTY_MIN 600
#define _DUTY_NEU 1560
#define _DUTY_MAX 2490

#define _POS_START (_DUTY_MIN + 100)   
#define _POS_END   (_DUTY_MAX - 100)   

#define _SERVO_SPEED 0.3f  // deg/s (90°/0.3 = 300s)
#define MOVE_DEG     90.0f
#define INTERVAL     20    // ms

unsigned long last_ms, phase_start_ms, move_ms;
int dir = +1; // +1: A->B, -1: B->A

int POS_A_US, POS_B_US;    // 0° 근처 → 90° 근처

Servo myservo;

static inline int clamp_us(int u){
  if (u < _POS_START) return _POS_START;
  if (u > _POS_END)   return _POS_END;
  return u;
}

void setup() {
  Serial.begin(57600);
  myservo.attach(10);

  // 마진 이후 실이동폭을 180°로 간주 → 1deg당 us
  float us_per_deg = (float)(_POS_END - _POS_START) / 180.0f;

  // 구간 정의: 끝단에서 시작
  POS_A_US = _POS_START;                                             // 시작(0°)
  POS_B_US = clamp_us(_POS_START + (int)(us_per_deg * MOVE_DEG + 0.5f)); // 0°→+90°

  // 편도 시간(정확히 300초)
  move_ms = (unsigned long)((MOVE_DEG / _SERVO_SPEED) * 1000.0f + 0.5f);

  // 시작: A에서 즉시 B로 이동 시작
  myservo.writeMicroseconds(POS_A_US);
  phase_start_ms = millis();
  last_ms = phase_start_ms;

  // 디버그(선택)
  Serial.print("us/deg="); Serial.println(us_per_deg, 4);
  Serial.print("A="); Serial.print(POS_A_US);
  Serial.print("  B="); Serial.println(POS_B_US);
  Serial.print("move_ms="); Serial.println(move_ms);
}

void loop() {
  unsigned long now = millis();
  if (now - last_ms < INTERVAL) return;
  last_ms = now;

  unsigned long t = now - phase_start_ms;
  if (t > move_ms) t = move_ms;

  int a = (dir > 0) ? POS_A_US : POS_B_US;
  int b = (dir > 0) ? POS_B_US : POS_A_US;
  float alpha = (float)t / (float)move_ms;      // 0→1
  int duty = (int)(a + (b - a) * alpha + 0.5f);
  myservo.writeMicroseconds(duty);

  if (t >= move_ms) {            // 편도 끝 → 즉시 반전(멈춤 없음)
    dir = -dir;
    phase_start_ms = now;
  }
}

