// WiFi AP + browser slider UI for live per-channel leg servo tuning.
// Connect to WiFi network defined below, then open http://192.168.4.1
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

const char* ap_ssid     = "15-DOF";
const char* ap_password = "12345789";

ESP8266WebServer server(80);
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

const int leftChannels[4]  = {L_HIP_ROLL, L_HIP_PITCH, L_KNEE, L_ANKLE};
const char* leftNames[4]   = {"L Hip Roll", "L Hip Pitch", "L Knee", "L Ankle"};
const int rightChannels[4] = {R_HIP_ROLL, R_HIP_PITCH, R_KNEE, R_ANKLE};
const char* rightNames[4]  = {"R Hip Roll", "R Hip Pitch", "R Knee", "R Ankle"};

int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

void setServo(int ch, int angle) {
  angle = constrain(angle, 0, 180);
  pwm.setPWM(ch, 0, angleToPulse(angle));
  currentAngle[ch] = angle;
  Serial.print("CH"); Serial.print(ch); Serial.print(" -> "); Serial.println(angle);
}

void sendSliderRow(const char* name, int ch) {
  String row = "<div class='row'><label>" + String(name) + " (CH" + String(ch) + ") ";
  row += "<span class='val' id='v" + String(ch) + "'>" + String(currentAngle[ch]) + "</span></label>";
  row += "<input type='range' min='0' max='180' value='" + String(currentAngle[ch]) + "' id='s" + String(ch) + "' ";
  row += "oninput=\"document.getElementById('v" + String(ch) + "').innerText=this.value; setServo(" + String(ch) + ", this.value)\">";
  row += "</div>";
  server.sendContent(row);
  yield();
}

void handleRoot() {
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");

  server.sendContent(
    "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>"
    "<title>Leg Servo Control</title><style>"
    "body{font-family:sans-serif;background:#111;color:#eee;padding:16px;max-width:820px;margin:auto}"
    "h2{text-align:center;margin-bottom:24px}"
    "h3{color:#4fd1c5;border-bottom:1px solid #333;padding-bottom:6px;margin-bottom:16px}"
    ".cols{display:flex;gap:28px;flex-wrap:wrap;justify-content:center}"
    ".col{flex:1;min-width:280px;max-width:360px}"
    ".row{margin-bottom:20px}"
    "label{display:flex;justify-content:space-between;font-size:14px;margin-bottom:6px}"
    "input[type=range]{width:100%;height:26px}"
    ".val{font-weight:bold;color:#4fd1c5;min-width:28px;text-align:right}"
    "button{width:100%;padding:12px;margin-top:8px;font-size:15px;background:#4fd1c5;border:none;border-radius:6px;color:#111;font-weight:bold}"
    "button.secondary{background:#333;color:#eee}"
    "#poseOutput{width:100%;margin-top:12px;background:#000;color:#4fd1c5;font-family:monospace;font-size:12px;padding:10px;border-radius:6px;min-height:70px;box-sizing:border-box;border:1px solid #333}"
    "</style></head><body><h2>Leg Servo Control</h2><div class='cols'>"
  );

  server.sendContent("<div class='col'><h3>Left leg</h3>");
  for (int i = 0; i < 4; i++) sendSliderRow(leftNames[i], leftChannels[i]);
  server.sendContent("</div>");

  server.sendContent("<div class='col'><h3>Right leg</h3>");
  for (int i = 0; i < 4; i++) sendSliderRow(rightNames[i], rightChannels[i]);
  server.sendContent("</div></div>");

  server.sendContent(
    "<button onclick='resetAll()'>Reset all to 90&deg;</button>"
    "<button class='secondary' onclick='copyPose()'>Copy current pose as code</button>"
    "<textarea id='poseOutput' readonly placeholder='Pose code will appear here...'></textarea>"
    "<script>"
    "const chans=[7,8,9,10,11,12,13,14];"
    "function setServo(ch, angle){fetch('/set?ch='+ch+'&angle='+angle);}"
    "function resetAll(){"
    "chans.forEach(ch=>{document.getElementById('s'+ch).value=90;document.getElementById('v'+ch).innerText=90;});"
    "fetch('/resetAll');"
    "}"
    "function copyPose(){"
    "let vals=chans.map(ch=>document.getElementById('v'+ch).innerText);"
    "let code='LegPose pose = {'+vals.join(', ')+'}; // hipRollL, hipPitchL, kneeL, ankleL, hipRollR, hipPitchR, kneeR, ankleR';"
    "document.getElementById('poseOutput').value=code;"
    "}"
    "</script></body></html>"
  );

  server.sendContent("");
}

void handleSet() {
  if (server.hasArg("ch") && server.hasArg("angle")) {
    int ch = server.arg("ch").toInt();
    int angle = server.arg("angle").toInt();
    if (ch >= 0 && ch <= 15) {
      setServo(ch, angle);
      server.send(200, "text/plain", "OK");
      return;
    }
  }
  server.send(400, "text/plain", "Bad request");
}

void handleResetAll() {
  int allCh[8] = {L_HIP_ROLL, L_HIP_PITCH, L_KNEE, L_ANKLE, R_HIP_ROLL, R_HIP_PITCH, R_KNEE, R_ANKLE};
  for (int i = 0; i < 8; i++) setServo(allCh[i], 90);
  server.send(200, "text/plain", "OK");
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

  int allCh[8] = {L_HIP_ROLL, L_HIP_PITCH, L_KNEE, L_ANKLE, R_HIP_ROLL, R_HIP_PITCH, R_KNEE, R_ANKLE};
  for (int i = 0; i < 16; i++) currentAngle[i] = 90;
  for (int i = 0; i < 8; i++) setServo(allCh[i], 90);

  WiFi.mode(WIFI_AP);
  bool apStarted = WiFi.softAP(ap_ssid, ap_password);
  delay(500);

  if (apStarted) {
    Serial.println("Access Point started.");
    Serial.print("Connect to WiFi '"); Serial.print(ap_ssid); Serial.println("', then open:");
    Serial.print("http://"); Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("ERROR: Failed to start Access Point.");
  }

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.on("/resetAll", handleResetAll);
  server.begin();
}

void loop() {
  server.handleClient();
}
