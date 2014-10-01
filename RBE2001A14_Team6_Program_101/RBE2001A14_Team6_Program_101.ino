#include <PPM.h>
#include <Servo.h>
#include "PPM.h"

//PINS
#define MOTOR_LEFT 13
#define MOTOR_RIGHT 12
#define MOTOR_FRONT 11
#define MOTOR_REAR 10

#define LS_FR 11
#define LS_FL 6
#define LS_RR 9
#define LS_RL 8

//line tracking boolean cases
#define DEFAULT_CASE  0b0000
#define UP_CASE       0b1100
#define UP_CASE2      0b1110
#define DOWN_CASE     0b0110
#define DOWN_CASE2    0b0111
#define LEFT_CASE     0b0011
#define LEFT_CASE2    0b1011
#define RIGHT_CASE    0b1001
#define RIGHT_CASE2   0b1101

//line tracking midpoint constant
#define LS_MIDPOINT 750 // change this later

//directions for movement
#define FORWARD     1
#define BACKWARD   2
#define LEFT   3
#define RIGHT  4
#define STOP 5
#define FORWARD_RIGHT 6
#define BACKWARD_RIGHT 7
#define FORWARD_LEFT 8
#define BACKWARD_LEFT 9

//serial enable or disable for movement
#define SER_EN 1
#define SER_DI 0

//sam
#define KP_LINETRACKING 0.01

Servo MotorLeft;
Servo MotorRight;
Servo MotorFront;
Servo MotorRear;

PPM ppm(2);

double front_back;
double left_right;

void setup() {
  MotorLeft.attach(MOTOR_LEFT, 1000, 2000);
  MotorRight.attach(MOTOR_RIGHT, 1000, 2000);
  MotorFront.attach(MOTOR_FRONT, 1000, 2000);
  MotorRear.attach(MOTOR_REAR, 1000, 2000); 
  Serial.begin(9600);
}

void loop(){
  //  teleop();
  // lineTrackBool();
  //lineTrackProp();
  //motorTest();
  //driveToLine();
  //while(1){
  // }
 LeftCross(3);
  Serial.print(analogRead(LS_FR));
  Serial.print('\t');
  Serial.print(analogRead(LS_RR));
  Serial.print('\t');
  Serial.print(analogRead(LS_RL));
  Serial.print('\t');
  Serial.println(analogRead(LS_FL));
  while(1);
}

void motorTest(){
  moveDirection(.5,FORWARD);
  delay(500);
  moveDirection(0, STOP);
  delay(100);
  moveDirection(.5,BACKWARD);
  delay(500);
  moveDirection(0, STOP);
  delay(100);
  moveDirection(.5,LEFT);
  delay(500);
  moveDirection(0, STOP);
  delay(100);
  moveDirection(.5,RIGHT);
  delay(500);
  moveDirection(0, STOP);
  delay(100);
  moveDirection(.5,FORWARD_RIGHT);
  delay(500);
  moveDirection(0, STOP);
  delay(100);
  moveDirection(.5,FORWARD_LEFT);
  delay(500);
  moveDirection(0, STOP);
  delay(100);
  moveDirection(.5,BACKWARD_LEFT);
  delay(500);
  moveDirection(0, STOP);
  delay(100);
  moveDirection(.5,BACKWARD_RIGHT);
  delay(500);
  moveDirection(0, STOP);
  delay(100);
}

void teleop() {
  front_back = ppm.getChannel(2);
  left_right = ppm.getChannel(1);
  Serial.println(left_right);
  Serial.println(front_back);
  MotorLeft.write(180-front_back);
  MotorRight.write(front_back);
  MotorFront.write(left_right);
  MotorRear.write(180-left_right);
}

void lineTrackProp() {
  int fl = analogRead(LS_FL);
  int fr = analogRead(LS_FR);
  int rl = analogRead(LS_RL);
  int rr = analogRead(LS_RR);
  MotorLeft.write(60);
  MotorRight.write(120);
  MotorFront.write(90 + KP_LINETRACKING * (fl - fr));
  MotorRear.write(90 + KP_LINETRACKING * (rr - rl));
}

void lineTrackBool(){
  byte state= 
    analogRead(LS_FL)/LS_MIDPOINT << 3 | 
    analogRead(LS_FR)/LS_MIDPOINT << 2 | 
    analogRead(LS_RR)/LS_MIDPOINT << 1 | 
    analogRead(LS_RL)/LS_MIDPOINT << 0;

  Serial.println(state, BIN);
  switch(state){
  case DEFAULT_CASE:
    moveDirection(0.3, FORWARD);
    break;
  case RIGHT_CASE2:
  case RIGHT_CASE:
    moveDirection(0.3, RIGHT);
    break;
  case DOWN_CASE2:
  case DOWN_CASE:
    moveDirection(0.3, BACKWARD);
    break;
  case LEFT_CASE2:
  case LEFT_CASE:
    moveDirection(0.3, LEFT);
    break;
  case UP_CASE2:
  case UP_CASE:
    moveDirection(0.3, FORWARD);
    break;
  default:
    moveDirection(0, FORWARD); //default 
    Serial.println("ERROR");
    break;
  }  
}

boolean OnLine(){
  return (analogRead(LS_FL)<LS_MIDPOINT&&analogRead(LS_RL)<LS_MIDPOINT);
}

void LeftCross(int maxCrosses){
  moveDirection(.3, LEFT);
  boolean onCross=0;
  int numCrosses=0;
  while(numCrosses<maxCrosses){
    if(OnLine()){
      onCross=1;
    }
    else if(onCross){
      numCrosses++;
      onCross=0;
    }
    else{
      onCross=0;
    }
  }
  moveDirection(0,STOP);
}

void driveToLine() {
  int left_ls, right_ls;
  while((left_ls = analogRead(LS_FR)) < LS_MIDPOINT && (right_ls = analogRead(LS_RL)) < LS_MIDPOINT) {
    moveDirection(0.4, FORWARD); //change
  }
  Serial.println("---------");
  moveDirection(0, STOP);
  delay(500);
  while((left_ls = analogRead(LS_FR)) < LS_MIDPOINT && (right_ls = analogRead(LS_RL)) < LS_MIDPOINT) {
    moveDirection(0.2, BACKWARD);
  }
  moveDirection(0, STOP);
}

void moveDirection(double power, byte dir) {
  if(power > 1) power = 1;
  if(power < 0) power = 0;

  switch(dir) {
  case FORWARD:
    MotorFront.write(90);
    MotorRight.write(90 - 90 * power);
    MotorRear.write(90);
    MotorLeft.write(90 + 90 * power);
    break;
  case BACKWARD:
    MotorFront.write(90);
    MotorRight.write(90 + 90 * power);
    MotorRear.write(90);
    MotorLeft.write(90 - 90 * power);
    break;
  case LEFT:
    MotorFront.write(90 - 90 *power);
    MotorRight.write(90);
    MotorRear.write(90 + 90*power);
    MotorLeft.write(90);
    break;
  case RIGHT:
    MotorFront.write(90 + 90 *power);
    MotorRight.write(90);
    MotorRear.write(90 - 90*power);
    MotorLeft.write(90);
    break;
  case STOP:
    MotorFront.write(90);
    MotorRear.write(90);
    MotorLeft.write(90);
    MotorRight.write(90);
    break;
  case FORWARD_RIGHT:
    MotorFront.write(90 + 90 * power);
    MotorRight.write(90 - 90 * power);
    MotorRear.write(90 - 90 * power);
    MotorLeft.write(90 + 90 * power);
    break;
  case BACKWARD_RIGHT:
    MotorFront.write(90 + 90 * power);
    MotorRight.write(90 + 90 * power);
    MotorRear.write(90 - 90 * power);
    MotorLeft.write(90 - 90 * power);
    break;
  case BACKWARD_LEFT:
    MotorFront.write(90 - 90 * power);
    MotorRight.write(90 + 90 * power);
    MotorRear.write(90 + 90 * power);
    MotorLeft.write(90 - 90 * power);
    break;
  case FORWARD_LEFT:
    MotorFront.write(90 - 90 * power);
    MotorRight.write(90 - 90 * power);
    MotorRear.write(90 + 90 * power);
    MotorLeft.write(90 + 90 * power);
    break;
  default:
    MotorFront.write(90);
    MotorRear.write(90);
    MotorLeft.write(90);
    MotorRight.write(90);
    break;
  }
}








