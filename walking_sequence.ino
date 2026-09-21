// 9-phase captured walking gait for the leg channels (CH7-CH14).
// Poses were hand-captured using slider_control.ino, then sequenced here
// with slow eased motion for stability. See README for phase descriptions.
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN   102
#define SERVOMAX   512
#define SERVO_FREQ 50

#define L_HIP_ROLL   7
#define L_HIP_PITCH  8
#define L_KNEE       9
#define L_ANKLE      10
#define R_HIP_ROLL   11
#define R_HIP_PITCH  12
#define R_KNEE       13
#define R_ANKLE      14

int currentAngle[16];

int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

float easeInOutSine(float t) {
  return -(cos(PI * t) - 1) / 2;
}

void moveTo(int channels[], int targets[], int count, int steps, int stepDelay) {
  int start[8];
  for (int i = 0; i < count; i++) start[i] = currentAngle[channels[i]];

  for (int s = 1; s <= steps; s++) {
    float t = easeInOutSine((float)s / steps);
    for (int i = 0; i < count; i++) {
      int interp = start[i] + (int)round((targets[i] - start[i]) * t);
      pwm.setPWM(channels[i], 0, angleToPulse(interp));
    }
    delay(stepDelay);
  }
  for (int i = 0; i < count; i++) currentAngle[channels[i]] = targets[i];
}

#define SLOW_STEPS   80
#define SLOW_DELAY   20
#define HOLD_MS      400

int legChannels[8] = {L_HIP_ROLL, L_HIP_PITCH, L_KNEE, L_ANKLE, R_HIP_ROLL, R_HIP_PITCH, R_KNEE, R_ANKLE};

int homeAngles[8] = {90, 90, 90, 90, 90, 90, 90, 90};

// Captured walk cycle, in order (right leg leads)
int pose1_shiftLeft[8]      = {100, 90, 90, 105, 90,  90, 90, 90};
int pose2_liftRight[8]      = {100, 90, 90, 105, 90,  90, 73, 98};
int pose3_swingRight[8]     = {100, 90, 90, 105, 90, 110, 73, 98};
int pose4_placeRight[8]     = {100, 90, 90, 105, 90, 110, 71, 96};
int pose5_shiftRight[8]     = { 96, 90, 90,  84, 78, 110, 71, 69};
int pose6_liftLeft[8]       = { 85, 83, 89,  70, 80,  90, 90, 65};
int pose7_swingLeft[8]      = { 85, 70, 89,  70, 80,  90, 90, 65};
int pose8_placeLeft[8]      = { 85, 67, 97,  63, 80,  90, 90, 65};
int pose9_shiftLeftAgain[8] = {100, 90, 90, 105, 80,  90, 90, 93};

void goTo(int pose[]) {
  moveTo(legChannels, pose, 8, SLOW_STEPS, SLOW_DELAY);
  delay(HOLD_MS);
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

  for (int i = 0; i < 16; i++) currentAngle[i] = 90;

  Serial.println("Home pose...");
  goTo(homeAngles);
}

void loop() {
  Serial.println("1: Shift weight left");
  goTo(pose1_shiftLeft);

  Serial.println("2: Lift right foot");
  goTo(pose2_liftRight);

  Serial.println("3: Swing right leg forward");
  goTo(pose3_swingRight);

  Serial.println("4: Place right foot down forward");
  goTo(pose4_placeRight);

  Serial.println("5: Shift weight onto right foot");
  goTo(pose5_shiftRight);

  Serial.println("6: Lift left foot");
  goTo(pose6_liftLeft);

  Serial.println("7: Swing left leg forward");
  goTo(pose7_swingLeft);

  Serial.println("8: Place left foot down forward");
  goTo(pose8_placeLeft);

  Serial.println("9: Shift weight onto left foot");
  goTo(pose9_shiftLeftAgain);
}
