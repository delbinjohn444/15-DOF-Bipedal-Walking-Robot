// Sets all 15 servos (CH0-CH14) to 90 degrees — idle standing pose / calibration baseline
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN   102
#define SERVOMAX   512
#define SERVO_FREQ 50
#define NUM_SERVOS 15

int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

const char* jointName[NUM_SERVOS] = {
  "Head Pan", "L Shoulder Roll", "L Shoulder Pitch", "L Elbow",
  "R Shoulder Roll", "R Shoulder Pitch", "R Elbow",
  "L Hip Roll", "L Hip Pitch", "L Knee", "L Ankle",
  "R Hip Roll", "R Hip Pitch", "R Knee", "R Ankle"
};

void setup() {
  Serial.begin(115200);
  Wire.begin(4, 5);
  Wire.setClock(400000);

  if (!pwm.begin()) {
    Serial.println("ERROR: PCA9685 not detected. Check wiring/address.");
    while (1);
  }
  pwm.setPWMFreq(SERVO_FREQ);
  delay(10);

  Serial.println("Moving all 15 servos to 90 degrees (idle standing pose)...");
  for (int ch = 0; ch < NUM_SERVOS; ch++) {
    pwm.setPWM(ch, 0, angleToPulse(90));
    Serial.print("CH"); Serial.print(ch);
    Serial.print(" ("); Serial.print(jointName[ch]); Serial.println(") -> 90 deg");
    delay(150);
  }
  Serial.println("Idle standing pose set.");
}

void loop() {}
