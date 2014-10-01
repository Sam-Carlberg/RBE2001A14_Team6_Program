#include <Servo.h>
#include <PPM.h>

#define MOTOR_LEFT 10
#define MOTOR_RIGHT 12
#define MOTOR_FRONT 11
#define MOTOR_REAR 9

#define LINE_RL 8
#define LINE_RR 9
#define LINE_FL 6
#define LINE_FR 11

Servo MotorLeft;
Servo MotorRight;
Servo MotorFront;
Servo MotorRear;

PPM ppm(2);

int front_back;
int left_right;

void setup() {
  pinMode(22, OUTPUT);
  MotorLeft.attach(MOTOR_LEFT, 1000, 2000);
  MotorRight.attach(MOTOR_RIGHT, 1000, 2000);
  MotorFront.attach(MOTOR_FRONT, 1000, 2000);
  MotorRear.attach(MOTOR_REAR, 1000, 2000);
  Serial.begin(9600);
}

void loop(){
//  teleop();
  debugLightSensors();
}

void teleop() {
    front_back = ppm.getChannel(2);
    left_right = ppm.getChannel(1);
    int fb = map(front_back, 0, 180, 0, 180);
    int rl = map(left_right, 0, 180, 0, 180);
    MotorLeft.write(180 - fb);
    MotorRight.write(fb);
    MotorFront.write(rl);
    MotorRear.write(180 - rl);  
}

void debugLightSensors() {

  Serial.print("RR: ");
  Serial.print(analogRead(LINE_RR));
  Serial.print("    RL: ");
  Serial.print(analogRead(LINE_RL));
  Serial.print("    FR: ");
  Serial.print(analogRead(LINE_FR));
  Serial.print("    FL: ");
  Serial.println(map(analogRead(LINE_FL),300,1000,50,1000));

}




