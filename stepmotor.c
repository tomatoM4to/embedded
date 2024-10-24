#include <wiringPi.h>
#include <stdio.h>

#define STEP_BLUE_PIN 22
#define STEP_PINK_PIN 23
#define STEP_YELLOW_PIN 24
#define STEP_ORANGE_PIN 25

void stepMotorInit(void) {
    pinMode(STEP_BLUE_PIN, OUTPUT);
    pinMode(STEP_PINK_PIN, OUTPUT);
    pinMode(STEP_YELLOW_PIN, OUTPUT);
    pinMode(STEP_ORANGE_PIN, OUTPUT);
}

void setStepMotor(int blue, int pink, int yellow, int orange) {
    digitalWrite(STEP_BLUE_PIN, blue);
    digitalWrite(STEP_PINK_PIN, pink);
    digitalWrite(STEP_YELLOW_PIN, yellow);
    digitalWrite(STEP_ORANGE_PIN, orange);
}

// 각도를 스텝 수로 변환하는 함수
int angleToSteps(float angle) {
   return (int)(angle / STEP_ANGLE);
}

void rotateStepMotor(float angle) {
  int steps = angleToSteps(angle < 0 ? -angle : angle);  // 각도의 절대값으로 스텝 수 계산
  printf("회전 각도: %.1f도 (스텝 수: %d)\n", angle, steps);

  if(angle > 0) {  // 시계 방향 회전
      for(int i = 0; i < steps; i++) {
          setStepMotor(HIGH, HIGH, LOW, LOW); delay(2);
          setStepMotor(LOW, HIGH, HIGH, LOW); delay(2);
          setStepMotor(LOW, LOW, HIGH, HIGH); delay(2);
          setStepMotor(HIGH, LOW, LOW, HIGH); delay(2);
      }
  }
  else {  // 반시계 방향 회전
      for(int i = 0; i < steps; i++) {
          setStepMotor(HIGH, LOW, LOW, HIGH); delay(2);
          setStepMotor(LOW, LOW, HIGH, HIGH); delay(2);
          setStepMotor(LOW, HIGH, HIGH, LOW); delay(2);
          setStepMotor(HIGH, HIGH, LOW, LOW); delay(2);
      }
  }

  // 회전 완료 후 모터 정지
  setStepMotor(HIGH, HIGH, HIGH, HIGH);
}

// 사용 예시:
// rotateStepMotor(90);    // 시계 방향으로 90도 회전
// rotateStepMotor(-90);   // 반시계 방향으로 90도 회전
// rotateStepMotor(180);   // 시계 방향으로 180도 회전
// rotateStepMotor(-180);  // 반시계 방향으로 180도 회전