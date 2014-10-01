#include "Arduino.h"
#include "OmniDrive.h"
#include "Constants.h"

OmniDrive::OmniDrive(int lmPort, int rmPort, int fmPort, int bmPort) {
  lm.attach(lmPort, 1000, 2000); // Using 393 motors, so we need the extra parameters
  rm.attach(rmPort, 1000, 2000);
  fm.attach(fmPort, 1000, 2000);
  bm.attach(bmPort, 1000, 2000);
} 

// Stops all drive motors
void OmniDrive::stop() {
  lm.write(MOTOR_STOP);
  rm.write(MOTOR_STOP);
  fm.write(MOTOR_STOP);
  bm.write(MOTOR_STOP);
}

// Drives forward at the given power (-1..1)
void OmniDrive::forward(double power) {
  const int realPower = map(power, -1, 1, MOTOR_MAX_CW, MOTOR_MAX_CCW);
  Serial.print(power);
  Serial.print("->");
  Serial.println(realPower);
  lm.write(realPower);
  rm.write(180 - realPower);
  fm.write(MOTOR_STOP);
  bm.write(MOTOR_STOP);
}

// Drives sideways at the given power (-1..1)
// Positive is right, negative is left
void OmniDrive::sideways(double power) {
  const int realPower = map(power, -1, 1, MOTOR_MAX_CW, MOTOR_MAX_CCW);
  lm.write(MOTOR_STOP);
  rm.write(MOTOR_STOP);
  fm.write(realPower);
  bm.write(180 - realPower);
}

// Spins at the given power (-1..1)
// Positive is counter clockwise, negative is clockwise.
void OmniDrive::spin(double power) {
  const int realPower = map(power, -1, 1, MOTOR_MAX_CW, MOTOR_MAX_CCW);
  lm.write(realPower);
  rm.write(realPower);
  fm.write(realPower);
  bm.write(realPower);
}


// Moves at the given front-back power and left-right power
// Positive is forward/right
// Negative is backward/left
void OmniDrive::move(double fbPower, double lrPower) {
  int realFBPower = map(fbPower, -1, 1, MOTOR_MAX_CW, MOTOR_MAX_CCW);
  int realLRPower = map(lrPower, -1, 1, MOTOR_MAX_CW, MOTOR_MAX_CCW);
  lm.write(180 - realFBPower);
  rm.write(realFBPower);
  fm.write(realLRPower);
  bm.write(180 - realLRPower);
  Serial.print(fbPower); Serial.print("   "); Serial.println(lrPower);
}
