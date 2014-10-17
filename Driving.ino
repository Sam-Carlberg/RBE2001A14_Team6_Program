

/*
 *  Turns the robot in the given direction at the given power (0 is stopped, +1 is full speed)
 */
void turnDirection(double power, int dir) {
  if(dir == CCW) {
    MotorNE.write(MOTOR_STOP - power * MOTOR_STOP);
    MotorSE.write(MOTOR_STOP - power * MOTOR_STOP);
    MotorSW.write(MOTOR_STOP - power * MOTOR_STOP);
    MotorNW.write(MOTOR_STOP - power * MOTOR_STOP);
  }  
  else if(dir == CW) {
    MotorNE.write(MOTOR_STOP + power * MOTOR_STOP);
    MotorSE.write(MOTOR_STOP + power * MOTOR_STOP);
    MotorSW.write(MOTOR_STOP + power * MOTOR_STOP);
    MotorNW.write(MOTOR_STOP + power * MOTOR_STOP);
  } 
  else{
    MotorNE.write(MOTOR_STOP);
    MotorSE.write(MOTOR_STOP);
    MotorSW.write(MOTOR_STOP);
    MotorNW.write(MOTOR_STOP);
  }
}

/*
 *  Moves the robot in the given direction at the given power (0 is stopped, +1 is full speed)
 */
void moveDirection(double power, char dir) {
  if(power > 1) power = 1;
  if(power < 0) power = 0;
  switch(dir) {
  case FORWARD_LEFT:
    MotorNE.write(MOTOR_STOP);
    MotorSE.write(MOTOR_STOP - MOTOR_STOP * power);
    MotorSW.write(MOTOR_STOP);
    MotorNW.write(MOTOR_STOP + MOTOR_STOP * power);
    break;
  case BACKWARD_RIGHT:
    MotorNE.write(MOTOR_STOP);
    MotorSE.write(MOTOR_STOP + MOTOR_STOP * power);
    MotorSW.write(MOTOR_STOP);
    MotorNW.write(MOTOR_STOP - MOTOR_STOP * power);
    break;
  case BACKWARD_LEFT:
    MotorNE.write(MOTOR_STOP - MOTOR_STOP  * power);
    MotorSE.write(MOTOR_STOP);
    MotorSW.write(MOTOR_STOP + MOTOR_STOP * power);
    MotorNW.write(MOTOR_STOP);
    break;
  case FORWARD_RIGHT:
    MotorNE.write(MOTOR_STOP + MOTOR_STOP  * power);
    MotorSE.write(MOTOR_STOP);
    MotorSW.write(MOTOR_STOP - MOTOR_STOP * power);
    MotorNW.write(MOTOR_STOP);
    break;
  case STOP:
    MotorNE.write(MOTOR_STOP);
    MotorSW.write(MOTOR_STOP);
    MotorNW.write(MOTOR_STOP);
    MotorSE.write(MOTOR_STOP);
    break;
  case RIGHT:
    MotorNE.write(MOTOR_STOP + MOTOR_STOP * power);
    MotorSE.write(MOTOR_STOP - MOTOR_STOP * power);
    MotorSW.write(MOTOR_STOP - MOTOR_STOP * power);
    MotorNW.write(MOTOR_STOP + MOTOR_STOP * power);
    break;
  case BACKWARD:
    MotorNE.write(MOTOR_STOP + MOTOR_STOP * power);
    MotorSE.write(MOTOR_STOP + MOTOR_STOP * power);
    MotorSW.write(MOTOR_STOP - MOTOR_STOP * power);
    MotorNW.write(MOTOR_STOP - MOTOR_STOP * power);
    break;
  case LEFT:
    MotorNE.write(MOTOR_STOP - MOTOR_STOP * power);
    MotorSE.write(MOTOR_STOP + MOTOR_STOP * power);
    MotorSW.write(MOTOR_STOP + MOTOR_STOP * power);
    MotorNW.write(MOTOR_STOP - MOTOR_STOP * power);
    break;
  case FORWARD:
    MotorNE.write(MOTOR_STOP - MOTOR_STOP * power);
    MotorSE.write(MOTOR_STOP - MOTOR_STOP * power);
    MotorSW.write(MOTOR_STOP + MOTOR_STOP * power);
    MotorNW.write(MOTOR_STOP + MOTOR_STOP * power);
    break;
  default:
    MotorNE.write(MOTOR_STOP);
    MotorSW.write(MOTOR_STOP);
    MotorNW.write(MOTOR_STOP);
    MotorSE.write(MOTOR_STOP);
    break;
  }
}

/*
 *  Moves the robot left/right at adjusted speeds
 */
void moveAdjusted(float spd, float adjSpdFront,float adjSpdBack, int dir) {
  switch(dir) {
  case FORWARD:
    MotorSE.write(MOTOR_STOP - spd + adjSpdBack * MOTOR_STOP);
    MotorSW.write(MOTOR_STOP + spd + adjSpdBack * MOTOR_STOP);
    MotorNE.write(MOTOR_STOP - spd + adjSpdFront * MOTOR_STOP);
    MotorNW.write(MOTOR_STOP + spd + adjSpdFront * MOTOR_STOP);
    break;
  case BACKWARD:
    MotorSE.write(MOTOR_STOP + spd + adjSpdBack * MOTOR_STOP);
    MotorSW.write(MOTOR_STOP - spd + adjSpdBack * MOTOR_STOP);
    MotorNE.write(MOTOR_STOP + spd + adjSpdFront * MOTOR_STOP);
    MotorNW.write(MOTOR_STOP - spd + adjSpdFront * MOTOR_STOP);
    break;
  }
} 



/*
 *  A simple function to test moving the robot in every direction.
 */
void testMotors() {
  moveDirection(.5,FORWARD);
  delay(500);
  moveDirection(0, STOP);
  delay(200);
  moveDirection(.5,BACKWARD);
  delay(500);
  moveDirection(0, STOP);
  delay(200);
  moveDirection(.5,LEFT);
  delay(500);
  moveDirection(0, STOP);
  delay(200);
  moveDirection(.5,RIGHT);
  delay(500);
  moveDirection(0, STOP);
  delay(200);
  moveDirection(.5,FORWARD_RIGHT);
  delay(500);
  moveDirection(0, STOP);
  delay(200);
  moveDirection(.5,FORWARD_LEFT);
  delay(500);
  moveDirection(0, STOP);
  delay(200);
  moveDirection(.5,BACKWARD_LEFT);
  delay(500);
  moveDirection(0, STOP);
  delay(200);
  moveDirection(.5,BACKWARD_RIGHT);
  delay(500);
  moveDirection(0, STOP);
  delay(200);
}