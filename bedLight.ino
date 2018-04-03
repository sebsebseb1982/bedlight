
const int redLED = 3; //red LED connects to digital pin 2
const int greenLED = 5; //green LED connects to digital pin 4
const int blueLED = 6; //blue LED connects to digital pin 7
const int button = 7; //blue LED connects to digital pin 7

const String lightOff       = "#000000";
const String lightLow       = "#000011";
const String lightHigh      = "#ffffff";

const unsigned long transitionDuration = 1 * 1000 /* ms */;
const unsigned long lightLowDuration = 480000 /* ms */;
const unsigned long lightHighDuration = 15 * 1000 /* ms */;
unsigned long lightLowEnd;
boolean isLightLowCycleStarted;
unsigned long lightHighEnd;
boolean isLightHighCycleStarted;

String currentStateValue;
String startStateValue;
String targetStateValue;
unsigned long transitionStart;

void setup() {
  Serial.begin(115200);
  currentStateValue = lightOff;

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(button, INPUT);
}

double Light (int RawADC0) {
  double Vout = RawADC0 * 0.0048828125;
  int lux=500/(10*((5-Vout)/Vout));//use this equation if the LDR is in the upper part of the divider
  //int lux = (2500 / Vout - 500) / 10;
  return lux;
}

int getRValue(String hexColor) {
  long number = strtol( &hexColor[1], NULL, 16);
  return number >> 16;
}

int getGValue(String hexColor) {
  long number = strtol( &hexColor[1], NULL, 16);
  return number >> 8 & 0xFF;
}

int getBValue(String hexColor) {
  long number = strtol( &hexColor[1], NULL, 16);
  return number & 0xFF;
}

void loggerCouleur(String hexColor) {
  Serial.print("R=");
  Serial.print(getRValue(hexColor));
  Serial.print(",G=");
  Serial.print(getGValue(hexColor));
  Serial.print(",B=");
  Serial.println(getBValue(hexColor));
}

/*void updateLight() {

  unsigned long maintenant = millis();
  boolean isTransitionFinie = maintenant - transitionStart > transitionDuration;

  if (!isTransitionFinie) {

    int startR = getRValue(startStateValue);
    int startG = getGValue(startStateValue);
    int startB = getBValue(startStateValue);

    int targetR = getRValue(targetStateValue);
    int targetG = getGValue(targetStateValue);
    int targetB = getBValue(targetStateValue);

    float transitionProgression = (maintenant - transitionStart) / (float)transitionDuration;

    int newR = startR + ((targetR - startR) * transitionProgression);
    int newG = startG + ((targetG - startG) * transitionProgression);
    int newB = startB + ((targetB - startB) * transitionProgression);

    currentStateValue = "#" + String(newR, HEX) + String(newG, HEX) + String(newB, HEX);
    analogWrite(redLED, newR);
    analogWrite(greenLED, newG);
    analogWrite(blueLED, newB);

    loggerCouleur(currentStateValue);
  }
  }*/

boolean isNight() {
  return int(Light(analogRead(A0))) < 10;
}

String padHexValue(String hexValue) {
  if (hexValue.length() == 1) {
    return "0" + hexValue;
  } else {
    return hexValue;
  }
}

void fadeTo(String hexColor) {
  Serial.println("fade from " + currentStateValue + " to " + hexColor);

  int startR = getRValue(currentStateValue);
  int startG = getGValue(currentStateValue);
  int startB = getBValue(currentStateValue);

  int targetR = getRValue(hexColor);
  int targetG = getGValue(hexColor);
  int targetB = getBValue(hexColor);

  for (int t = 1; t <= transitionDuration; t++) {

    float transitionProgression = (float)t / (float)transitionDuration;

    int newR = startR + ((targetR - startR) * transitionProgression);
    int newG = startG + ((targetG - startG) * transitionProgression);
    int newB = startB + ((targetB - startB) * transitionProgression);

    currentStateValue = "#" + padHexValue(String(newR, HEX)) + padHexValue(String(newG, HEX)) + padHexValue(String(newB, HEX));
    analogWrite(redLED, newR);
    analogWrite(greenLED, newG);
    analogWrite(blueLED, newB);
    delay(1);
  }
}

void lookingForPresence() {
  if (digitalRead(button) == HIGH) {
    isLightLowCycleStarted = true;
    isLightHighCycleStarted = true;
    fadeTo(lightHigh);
    lightLowEnd = millis() + lightLowDuration;
    lightHighEnd = millis() + lightHighDuration;
  }
}

void updateLight() {
  if (millis() > lightHighEnd && isLightHighCycleStarted) {
    fadeTo(lightLow);
    isLightHighCycleStarted = false;
  }
  if (millis() > lightLowEnd && isLightLowCycleStarted) {
    fadeTo(lightOff);
    isLightLowCycleStarted = false;
  }
}

void loop() {
  if (isNight()) {
    lookingForPresence();
  }
  updateLight();
}
