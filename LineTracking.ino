/**
 *  LINETRACKING.INO
 * 
 *  Contains all the functionality to line track.
 */

/*
 * Moves the robot in the given direction until it crosses the given number of lines.
 * Returns true once the robot has counted the number of specified lines
 */
boolean dirCount(int crosses, int dir) {
  bounceAdjust();
  Serial.println(bounceNumCrosses);
  Serial.println(crosses);
  moveAdjusted(35, bounceErrorPowerFront, bounceErrorPowerRear, dir); //15 CHANGE HERE
  if(bounceNumCrosses == crosses) {
    bounceNumCrosses=0;
    moveDirection(0,STOP);
    return true;
  }
  return false;
}

/*
 * Translates the robot east until it's centered over a line.
 * Returns true once it's centered on both the north and south sides
 */
boolean goToLine() {
  boolean south = goToLineSouth(EAST);
  boolean north = goToLineNorth(EAST);
  return south && north;
}

/*
 * Translates the north side of the robot in the given direction until it's centered on a line.
 * Returns true once it's centered
 */
boolean goToLineNorth(char dir) {
  switch(newGoToLineStateNorth) {
  case INIT_GO_TO_LINE:
    if(dir == EAST) {
      MotorNE.write(MOTOR_STOP + GOTOLINE_SPEED);
      MotorNW.write(MOTOR_STOP + GOTOLINE_SPEED);
      if(getLS(_NW_LS))
        newGoToLineStateNorth=GO_RIGHT;
    }
    else{
      MotorNE.write(MOTOR_STOP - GOTOLINE_SPEED);
      MotorNW.write(MOTOR_STOP - GOTOLINE_SPEED);
      if(getLS(_NE_LS))
        newGoToLineStateNorth=GO_LEFT;
    }
    break;
  case GO_RIGHT:
    MotorNE.write(MOTOR_STOP - GOTOLINE_SPEED);
    MotorNW.write(MOTOR_STOP - GOTOLINE_SPEED);
    if(getLS(_NC_LS)) {
      newGoToLineStateNorth = GO_STOP;
    }
    if(getLS(_NE_LS)) {
      newGoToLineStateNorth = GO_LEFT;
    }
    break;
  case GO_LEFT:
    MotorNE.write(MOTOR_STOP + GOTOLINE_SPEED);
    MotorNW.write(MOTOR_STOP + GOTOLINE_SPEED);
    if(getLS(_NC_LS)) {
      newGoToLineStateNorth = GO_STOP;
    }
    if(getLS(_NW_LS)) {
      newGoToLineStateNorth = GO_RIGHT;
    }
    break;
  case GO_STOP:
    MotorNE.write(MOTOR_STOP);
    MotorNW.write(MOTOR_STOP);
    if(getLS(_NW_LS)) {
      newGoToLineStateNorth = GO_RIGHT;
    }
    else if(getLS(_NE_LS)) {
      newGoToLineStateNorth = GO_LEFT;
    }
    else
      return true;
    break;
  }
  return false;
}

/*
 * Translates the south side of the robot in the given direction until it's centered on a line.
 * Returns true once it's centered
 */
boolean goToLineSouth(char dir) {
  switch(newGoToLineStateSouth) {
  case INIT_GO_TO_LINE:
    if(dir == EAST) {
      MotorSW.write(MOTOR_STOP - GOTOLINE_SPEED);
      MotorSE.write(MOTOR_STOP - GOTOLINE_SPEED);
      if(getLS(_SE_LS))
        newGoToLineStateSouth=GO_RIGHT;
    } 
    else{
      MotorSW.write(MOTOR_STOP + GOTOLINE_SPEED);
      MotorSE.write(MOTOR_STOP + GOTOLINE_SPEED);
      if(getLS(_SW_LS))
        newGoToLineStateSouth=GO_LEFT;
    }
    break;
  case GO_RIGHT:
    MotorSW.write(MOTOR_STOP + GOTOLINE_SPEED);
    MotorSE.write(MOTOR_STOP + GOTOLINE_SPEED);
    if(getLS(_SC_LS)) {
      newGoToLineStateSouth = GO_STOP;
    }
    if(getLS(_SE_LS)) {
      newGoToLineStateSouth = GO_LEFT;
    }
    break;
  case GO_LEFT:
    MotorSE.write(MOTOR_STOP - GOTOLINE_SPEED);
    MotorSW.write(MOTOR_STOP - GOTOLINE_SPEED);
    if(getLS(_SC_LS)) {
      newGoToLineStateSouth = GO_STOP;
    }
    if(getLS(_SW_LS)) {
      newGoToLineStateSouth = GO_RIGHT;
    }
    break;
  case GO_STOP:
    MotorSE.write(MOTOR_STOP);
    MotorSW.write(MOTOR_STOP);
    if(getLS(_SW_LS)) {
      newGoToLineStateSouth = GO_RIGHT;
    } 
    else if(getLS(_SE_LS)) {
      newGoToLineStateSouth = GO_LEFT;
    } 
    else {
      return true;
    }
    break;  
  }
  return false;
}

/*
 *  Turns in the given direction to align with a line and stops if the given timeout is reached.
 */
boolean turnAround(int waitTime, char dir) {
  if(reset == 0 && timeLocal >= waitTime) {
    boolean south = turnAroundSouth(0, dir);
    boolean north = turnAroundNorth(0, dir);
    return south && north;
  }
  else
    turnDirection(0.25, dir);
  return false;
}

/*
 * Turns the north side of the robot until it's centered on a line.
 * Returns true once it's centered
 */
boolean turnAroundNorth(int waitTime, char dir) {
  switch(turnAroundNorthState) {
  case INIT_GO_TO_LINE:
    if(dir == CW) {
      MotorNE.write(MOTOR_STOP + TURN_SPEED);
      MotorNW.write(MOTOR_STOP + TURN_SPEED);
    }
    else{
      MotorNE.write(MOTOR_STOP - TURN_SPEED);
      MotorNW.write(MOTOR_STOP - TURN_SPEED);
    }
    if(timeLocal >= waitTime) {
      turnAroundNorthState = NONE_HIT;
    }
    break;

  case NONE_HIT:
    if(dir == CW) {
      MotorNE.write(MOTOR_STOP + TURN_SPEED);
      MotorNW.write(MOTOR_STOP + TURN_SPEED);
      if(getLS(_NW_LS))
        turnAroundNorthState = GO_RIGHT;
    }
    else{
      MotorNE.write(MOTOR_STOP - TURN_SPEED);
      MotorNW.write(MOTOR_STOP - TURN_SPEED);
      if(getLS(_NE_LS))
        turnAroundNorthState = GO_LEFT;
    }
    break;

  case GO_RIGHT:
    MotorNE.write(MOTOR_STOP - TURN_SPEED);
    MotorNW.write(MOTOR_STOP - TURN_SPEED);
    if(getLS(_NC_LS)) {
      turnAroundNorthState = GO_STOP;
    }
    if(getLS(_NE_LS)) {
      turnAroundNorthState = GO_LEFT;
    }
    break;

  case GO_LEFT:
    MotorNE.write(MOTOR_STOP + TURN_SPEED);
    MotorNW.write(MOTOR_STOP + TURN_SPEED);
    if(getLS(_NC_LS)) {
      turnAroundNorthState = GO_STOP;
    }
    if(getLS(_NW_LS)) {
      turnAroundNorthState = GO_RIGHT;
    }
    break;

  case GO_STOP:
    MotorNE.write(MOTOR_STOP);
    MotorNW.write(MOTOR_STOP);
    if(getLS(_NC_LS))
      return true;
    else if(getLS(_NW_LS)) {
      turnAroundNorthState = GO_RIGHT;
    }
    else if(getLS(_NE_LS)) {
      turnAroundNorthState = GO_LEFT;
    }
    break;
  }
  return false;
}

/*
 * Turns the south side of the robot until it's centered on a line.
 * Returns true once it's centered
 */
boolean turnAroundSouth(int waitTime, char dir) {
  switch(turnAroundSouthState) {
  case INIT_GO_TO_LINE:
    if(dir == CCW) {
      MotorSW.write(MOTOR_STOP - TURN_SPEED);
      MotorSE.write(MOTOR_STOP - TURN_SPEED);
    } 
    else{
      MotorSW.write(MOTOR_STOP + TURN_SPEED);
      MotorSE.write(MOTOR_STOP + TURN_SPEED);
    }
    if(timeLocal>=waitTime)
      turnAroundSouthState=NONE_HIT;
    break;

  case NONE_HIT:
    if(dir == CCW) {
      MotorSW.write(MOTOR_STOP - TURN_SPEED);
      MotorSE.write(MOTOR_STOP - TURN_SPEED);
      if(getLS(_SW_LS))
        turnAroundSouthState=GO_RIGHT;
    }
    else{
      MotorSW.write(MOTOR_STOP + TURN_SPEED);
      MotorSE.write(MOTOR_STOP + TURN_SPEED);
      if(getLS(_SE_LS))
        turnAroundSouthState=GO_LEFT;
    }
    break;

  case GO_RIGHT:
    MotorSW.write(MOTOR_STOP + TURN_SPEED);
    MotorSE.write(MOTOR_STOP + TURN_SPEED);
    if(getLS(_SC_LS)) {
      turnAroundSouthState = GO_STOP;
    }
    if(getLS(_SE_LS)) {
      turnAroundSouthState = GO_LEFT;
    }
    break;

  case GO_LEFT:
    MotorSE.write(MOTOR_STOP - TURN_SPEED);
    MotorSW.write(MOTOR_STOP - TURN_SPEED);
    if(getLS(_SC_LS)) {
      turnAroundSouthState = GO_STOP;
    }
    if(getLS(_SW_LS)) {
      turnAroundSouthState = GO_RIGHT;
    }
    break;

  case GO_STOP:
    MotorSE.write(MOTOR_STOP);
    MotorSW.write(MOTOR_STOP);
    if(getLS(_SC_LS))
      return true;
    else if(getLS(_SW_LS)) {
      turnAroundSouthState = GO_RIGHT;
    }
    else if(getLS(_SE_LS)) {
      turnAroundSouthState = GO_LEFT;
    }
    break;  
  }
  return false;
}

//adjusts error speeds for left and right motors
void bounceAdjust() {
  if(getLS(_NC_LS) || 
    getLS(_NE_LS) || 
    getLS(_NW_LS)) {
    bounceErrorPowerFront = 0.12 * getLS(_NE_LS) - 0.12 * getLS(_NW_LS);
  }
  if(getLS(_SC_LS)  || 
    getLS(_SE_LS)  || 
    getLS(_SW_LS)) {
    bounceErrorPowerRear = 0.12 * getLS(_SW_LS) - 0.12 * getLS(_SE_LS);
  }

  if(getLS(_E_LS) && 
    getLS(_W_LS)) {
    bounceOnCross = true;
  } 
  else if(bounceOnCross == 1 && 
    getLS(_E_LS) == WHITE && 
    getLS(_W_LS) == WHITE) {
    bounceOnCross = false;
    bounceNumCrosses++;
  } 
}

/*
 * Does an overshot check driving in the given direction
 */
boolean dirOvershoot(int dir) {
  bounceAdjust();
  moveAdjusted(14, bounceErrorPowerFront, bounceErrorPowerRear, dir);
  if(getLS(_W_LS) && 
    getLS(_E_LS)) {
    moveDirection(0, STOP);
    return true;
  }
  return false;
}


/*
 * Gets the direction the robot needs to turn to in order to drive forward down the storage lane
 */
int getDirToStorage() {
  return onLowSide ? CW : CCW;
}

/*
 * Gets the direction the robot needs to turn to in order to drive backward to the supply
 * lane from the storage lane, as well as the direction needed to turn in order to drive forward
 * down the supply lane once the robot reaches it.
 */
int getDirToSupply() {
  return storageTarget < supplyTarget ? CCW : CW;
}



/*
 *  Checks if the given light sensor reads black (true) or white (false)
 */
boolean getLS(int lineSensor) {
  return (analogRead(lineSensor) >= LS_THRESHOLD) ==  BLACK;
}


/*
 * Simple test to debug light sensor readings
 */
void testLS() { 
  Serial.print(getLS(_NE_LS));
  Serial.print('\t');
  Serial.print(getLS(_NC_LS));
  Serial.print('\t');
  Serial.print(getLS(_NW_LS));
  Serial.print('\t');
  Serial.print(getLS(_E_LS));
  Serial.print('\t');
  Serial.print(getLS(_SE_LS));
  Serial.print('\t');
  Serial.print(getLS(_SC_LS));
  Serial.print('\t');
  Serial.print(getLS(_SW_LS));
  Serial.print('\t');
  Serial.print(getLS(_W_LS));
  Serial.println(" NE-NC-NW-E-SE-SC-SW-W"); 
}

