/**
 * MANIPULATOR.INO
 *
 * Contains all the functions needed to control the arm and manipulator
 */


/*
 * Given grab/place, a first and last position, first move the arm to the first position, 
 * grab or place a rod, and then move to the last location
 *
 * returns true if complete, false otherwise
 */
boolean grabAndPlace(int grabOrPlace, int firstLocation, int lastLocation, int clawRunTime) {
  switch(clawState) {
  case ARM_FIRST: // go to the first location
    if(reset == 0) {
      if(grabOrPlace == CW)
        moveManipulator(grabOrPlace);

      if(moveArm(firstLocation)) {
        clawState = CLAW_GRABPLACE;
        reset=1;
      }
    }
    break;
  case CLAW_GRABPLACE:
    if(reset == 0) { // grab or place the rod based on time
      moveManipulator(grabOrPlace);
      if(timeLocal >= clawRunTime) {
        clawState=ARM_LAST;
        MotorClaw.write(MOTOR_STOP);
      }
    }
    break;
  case ARM_LAST: // move to the last location
    if(moveArm(lastLocation)) {
      MotorArm.write(MOTOR_STOP);
      clawState = ARM_FIRST; // resets for next time
      return true;
    }
    break;
  }
  return false;
}

/*
 * Intakes or dispenses a rod based on the passed direction
 * returns true once passed a variable that's not CW or CCW
 */
boolean moveManipulator(char dir) {
  if(dir == CW) {
    MotorClaw.write(INTAKE);
  } 
  else if(dir == CCW) {
    MotorClaw.write(DISPENSE);
  } 
  else {
    MotorClaw.write(MOTOR_STOP);
    return true;
  }
  return false;
}

//Moves the arm to the desired position based on limit switches
//returns true if complete, false if in motion
boolean moveArm(int pos) {
  // set the current position
  if(!digitalRead(LIMIT_CLAW_UP)) { 
    currentPositionClaw = CLAW_UP;
  } 
  else if(!digitalRead(LIMIT_CLAW_DOWN)) {
    currentPositionClaw = CLAW_DOWN;
  } 
  else {
    currentPositionClaw = 7;
  }

  if(pos == CLAW_UP) {
    MotorArm.write(ARM_UP_SPEED);

  } 
  else if(pos == CLAW_DOWN) {
    MotorArm.write(ARM_DOWN_SPEED);

  } 
  else if(pos == SUPPLY) {
    if(timeLocal < 1.2 *  WAIT_TIME) { // go to the supply position for a certain time, then go to the normal "up" position
      armToSupply();
    } 
    else {
      MotorArm.write(ARM_UP_SPEED);
    }
  }

  if(pos == currentPositionClaw || (pos == SUPPLY && timeLocal >= 1.5 * WAIT_TIME)) {
    MotorArm.write(MOTOR_STOP);
    return true;
  }

  return false;
}

/*
 *  Modified P control to move the arm to the position required to remove a rod from a supply tube
 */
boolean armToSupply() {
  float error = SUPPLY_POSITION - analogRead(POTENTIOMETER); // between -700 and +700 (approx.)

  float normalError =  error / (ARM_DOWN_POSITION - ARM_UP_POSITION); // error normalized to be between -1 and +1

  // take the square root of the normalized error to have higher power at low errors than normal P control
  if(normalError < 0) 
    normalError = -sqrt(-normalError); // avoid imaginary numbers
  else
    normalError = sqrt(normalError);

  int speedArm = MOTOR_STOP - normalError * MOTOR_STOP;

  // make sure speed is within bounds
  if(speedArm < MOTOR_MAX_CCW) speedArm = MOTOR_MAX_CCW;
  if(speedArm > MOTOR_MAX_CW)  speedArm = MOTOR_MAX_CW;

  // don't run if the position is within tolerance
  if(abs(error) < ARM_TOLERANCE) {
    speedArm = MOTOR_STOP;
    return true;
  }

  MotorArm.write(speedArm);
  return false;
}

