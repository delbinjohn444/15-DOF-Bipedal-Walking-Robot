// Moves ONE channel through a safe reduced range to verify wiring/direction.
// Change TEST_CHANNEL and re-upload to test each joint individually.
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN   102
#define SERVOMAX   512
#define SERVO_FREQ 50
#define TEST_CHANNEL 3   // change this to test one joint at a time

int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(4, 5);
  Wire.setClock(400000);

  if (!pwm.begin()) {
    Serial.println("ERROR: PCA9685 not detected.");
    while (1);
  }
  pwm.setPWMFreq(SERVO_FREQ);
  delay(10);

  Serial.print("Testing channel ");
  Serial.println(TEST_CHANNEL);
}

void loop() {
  for (int angle = 60; angle <= 120; angle += 5) {
    pwm.setPWM(TEST_CHANNEL, 0, angleToPulse(angle));
    delay(30);
  }
  delay(300);
  for (int angle = 120; angle >= 60; angle -= 5) {
    pwm.setPWM(TEST_CHANNEL, 0, angleToPulse(angle));
    delay(30);
  }
  delay(300);
}
