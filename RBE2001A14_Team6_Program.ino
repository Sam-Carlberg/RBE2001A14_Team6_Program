#include <PPM.h>
#include <Servo.h>
#include "Constants.h"
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

//line tracking cases
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
#define LS_MIDPOINT 512 // change this later

//directions for movement
// #define UP     1
// #define DOWN   2
// #define LEFT   3
// #define RIGHT  4
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
  // moveDirection(0.3, F_L);
  // delay(1000);
  // moveDirection(0.3, F_R);
  // delay(1000);
  // moveDirection(0.3, B_R);
  // delay(1000);
  // moveDirection(0.3, B_L);
  // delay(1000);
  // moveDirection(0, NONE);
  driveToLine();
  while(1){}
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
    moveDirection(0, NONE);
    Serial.println("ERROR");
    break;
  }  
  
}

void driveToLine() {
  int left, right;
  while((left = analogRead(LS_FR)) < 512 && (right = analogRead(LS_RL)) < 512) {
    Serial.print(left);
    Serial.print("   ");
    Serial.println(right);
    moveDirection(0.4, F_L);
  }

  Serial.println("---------");
  moveDirection(0, NONE);
  delay(500);
  while((left = analogRead(LS_FR)) < 512 && (right = analogRead(LS_RL)) < 512) {
    Serial.print(left);
    Serial.print("   ");
    Serial.println(right);
    moveDirection(0.2, B_R);
  }
  moveDirection(0, NONE);
}

void driveToCross() {
}

void moveDirection(double power, byte dir) {
  if(power > 1) power = 1;
  if(power < 0) power = 0;

  switch(dir) {
    case FORWARD:
    break;
    case BACKWARD:
    break;
    case LEFT:
    break;
    case RIGHT:
    break;
    case B_R:
    MotorFront.write(90 + 90 * power);
    MotorRight.write(90 + 90 * power);
    MotorRear.write(90 - 90 * power);
    MotorLeft.write(90 - 90 * power);
    break;
    case B_L:
    MotorFront.write(90 - 90 * power);
    MotorRight.write(90 + 90 * power);
    MotorRear.write(90 + 90 * power);
    MotorLeft.write(90 - 90 * power);
    break;
    case F_R:
    MotorFront.write(90 + 90 * power);
    MotorRight.write(90 - 90 * power);
    MotorRear.write(90 - 90 * power);
    MotorLeft.write(90 + 90 * power);
    break;
    case F_L:
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