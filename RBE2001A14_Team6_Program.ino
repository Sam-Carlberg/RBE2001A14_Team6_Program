//OUTLINE:
//#DEFINES AND GLOBALS
//INTERRUPTS
//SETUP
//LOOP,
//BLUE TOOTH
//GAMESTATE
//LS FUNCTIONS
//LINE TRACK FUNCTIONS
//MOTOR FUNCTIONS

#include <PPM.h>
#include "Servo.h"
#include "PPM.h"
#include "TimerOne.h"

//PINS
#define MOTOR_SW  8  //SOUTH
#define MOTOR_NE  6  //NORTH
#define MOTOR_NW  12 //WEST 
#define MOTOR_SE  13 //EAST

#define MOTOR_ARM 11
#define MOTOR_CLAW 10

#define LIMIT_FRONT                2
#define LIMIT_CLAW_DOWN           22
#define LIMIT_CLAW_UP             23
#define LIMIT_CLAW_ROD            22

#define START_BUTTON              24
#define POTENTIOMETER A6

/* #define LIMIT_FRONT_INTERRUPT      0
 * #define LIMIT_CLAW_DOWN_INTERRUPT  4
 * #define LIMIT_CLAW_UP_INTERRUPT    3
 */
//Line Sensor PINS
#define _E_LS  A0
#define _NE_LS A9
#define _NC_LS A8
#define _NW_LS A11
#define _W_LS  A7
#define _SW_LS A4
#define _SC_LS A3
#define _SE_LS A2

//directions for movement
#define FORWARD        1
#define BACKWARD       2
#define LEFT           3
#define RIGHT          4
#define STOP           5
#define FORWARD_RIGHT  6
#define BACKWARD_RIGHT 7
#define FORWARD_LEFT   8
#define BACKWARD_LEFT  9

//line sensor definitions
#define WHITE 0
#define BLACK 1

//time
volatile long unsigned int timeTotal=0;
long unsigned volatile int timeLocal=0;
boolean reset=true;

//goto Line
#define LINE_FORWARD 1
#define LINE_SLOW    2
#define LINE_SLOWEST 3
#define LINE_TURN    4
#define STOP_WAIT    5
#define LINE_REVERSE 6

#define LINE_SPEED         .22
#define LINE_SLOW_SPEED    .18
#define LINE_SLOWEST_SPEED .18
#define LINE_WAIT_TIME     1000 //ms

#define CW  1
#define CCW 0

char goToLineState = LINE_FORWARD;
char goToLineDir = CW;

//Bounce
float bounceErrorPowerFront = 0; // between 0 and basePower
float bounceErrorPowerRear = 0; // between 0 and basePower
int bounceNumCrosses = 0; //you haven't crossed any lines yet
int bounceForwardBack = FORWARD;
boolean bounceOnCross = 0;

//gotoline
#define INIT_GO_TO_LINE 0
#define GO_RIGHT 2
#define GO_LEFT 3
#define GO_STOP 4
#define EAST 0
#define WEST 1
int newGoToLineStateSouth = INIT_GO_TO_LINE;
int newGoToLineStateNorth = INIT_GO_TO_LINE;

//turnaround
#define WAIT_TIME 1000
#define TURN_WAIT_TIME 1600
#define NONE_HIT 1

int turnAroundSouthState = INIT_GO_TO_LINE;
int turnAroundNorthState = INIT_GO_TO_LINE;

//Arm
#define ARM_FIRST 0
#define CLAW_GRABPLACE 1
#define ARM_LAST 2

#define CLAW_UP 1
#define CLAW_DOWN 0
#define SUPPLY 2
#define CLAW_RUN_TIME 2500 //ms
#define SUPPLY_POSITION 250
#define ARM_UP_POSITION 163
#define ARM_DOWN_POSITION 856

int currentPositionClaw=7;
int currentPositionPot = 7;
int clawState;

//LED radiation
#define LED_HIGH 30
#define LED_LOW 31
#define NO_INTENSITY 0
#define LOW_INTENSITY 1
#define HIGH_INTENSITY 2

int intensity = NO_INTENSITY;

//Lets Play a game.
int ArisGameState=0;
#define ARISGAMESPEED .2
#define FIRST_SIDE 1
#define SECOND_SIDE 0
boolean side = FIRST_SIDE;

enum ArisGameState{
  GOTOLINE,
  FORWARDTOLIMIT1,
  REALIGNFORWARD1,
  DEPLOYCLAW1,
  SLIGHTREVERSE1,
  REVERSETOLINE1,
  OVERSHOTCHECK1,
  TURN1,
  FORWARDTOLIMIT2,
  REALIGNFORWARD2,
  DEPLOYCLAW2,
  SLIGHTREVERSE2,
  REVERSETOLINE2,
  OVERSHOTCHECK2,
  TURN2,
  SLIGHTREVERSE3,
  REVERSETOLINE3,
  OVERSHOTCHECK3,
  TURN3,
  FORWARDTOLIMIT3,
  REALIGNFORWARD3,
  DEPLOYCLAW3,
  SLIGHTREVERSE4,
  REVERSETOLINE4,
  OVERSHOTCHECK4,
  TURN4,
  FORWARDTOLIMIT4,
  REALIGNFORWARD4,
  DEPLOYCLAW4,

  SLIGHTREVERSE5,
  REVERSETOLINE5,
  OVERSHOOTCHECK5,
  TURN5,
  TURN6  
};

//teleop
PPM ppm(2);
double front_back;
double left_right;

//motors
Servo MotorNw;
Servo MotorSe;
Servo MotorNe;
Servo MotorSw;

Servo MotorArm;
Servo MotorClaw;

//timer interrupt
void incrementTime(){
  if(reset){
    timeLocal=0;
    reset=0;
  }
  else{
    timeLocal++;
  }
  //*SAM* BLUE TOOTH
  timeTotal++; //time is measured in milliseconds
}

void limitHitFront(void){
  bounceForwardBack=BACKWARD;
}

void limitHitDown(void){
  currentPositionClaw=CLAW_DOWN;
}

void limitHitUp(void){
  currentPositionClaw=CLAW_UP;
}

void setup(){
  Timer1.initialize(1000);			        //initializes timer with a period of 1ms
  Timer1.attachInterrupt(incrementTime); 		//increments time every 1ms
  MotorNw.attach(MOTOR_NW, 1000, 2000);
  MotorSe.attach(MOTOR_SE, 1000, 2000);
  MotorNe.attach(MOTOR_NE, 1000, 2000);
  MotorSw.attach(MOTOR_SW, 1000, 2000); 
  MotorArm.attach(MOTOR_ARM, 1000, 2000);
  MotorClaw.attach(MOTOR_CLAW, 1000, 2000);

  pinMode(LED_LOW, OUTPUT);
  pinMode(LED_HIGH, OUTPUT);

  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(POTENTIOMETER, INPUT);

#if 0
  attachInterrupt(LIMIT_FRONT_INTERRUPT, limitHitFront, RISING); 
  attachInterrupt(LIMIT_CLAW_DOWN_INTERRUPT, limitHitDown, RISING); 
  attachInterrupt(LIMIT_CLAW_UP_INTERRUPT, limitHitUp, RISING);
#endif

  pinMode(LIMIT_FRONT, INPUT_PULLUP);
  pinMode(LIMIT_CLAW_DOWN, INPUT_PULLUP);
  pinMode(LIMIT_CLAW_UP, INPUT_PULLUP);
  pinMode(LIMIT_CLAW_ROD, INPUT_PULLUP);

  //limit sensors
  pinMode(_NE_LS, INPUT);
  pinMode(_NW_LS, INPUT);
  pinMode(_SE_LS, INPUT);
  pinMode(_SW_LS, INPUT);
  pinMode(_NC_LS, INPUT);
  pinMode(_SC_LS, INPUT);
  pinMode(_E_LS, INPUT);
  pinMode(_W_LS, INPUT);
  Serial.begin(9600);
}

void loop(){
  while(digitalRead(START_BUTTON));
  while(1){
    ArisGame();
  }
}

//blue tooth functions
void heartbeat(){
}
void sendRadiation(int intensity){
}
boolean getMovementSignal(){
  return true;
}

//sets LED radiation signal
void setRadiationLED(int intensity){
  if(intensity==NO_INTENSITY){
    digitalWrite(LED_HIGH,LOW);
    digitalWrite(LED_LOW,LOW);
  }
  else if(intensity==LOW_INTENSITY){
    digitalWrite(LED_HIGH,LOW);
    digitalWrite(LED_LOW,HIGH);
  }
  else if(intensity==HIGH_INTENSITY){
    digitalWrite(LED_HIGH,HIGH);
    digitalWrite(LED_LOW,HIGH);
  }
}
//Let's Play a Game.
int ArisGame(){
  setRadiationLED(intensity);
  switch(ArisGameState){
  case GOTOLINE:
    if(getMovementSignal()){
      if(goToLine()==1){
        ArisGameState++;
        reset=1;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case FORWARDTOLIMIT1:
    if(getMovementSignal()){
      dirCount(9, FORWARD);
      if(!digitalRead(LIMIT_FRONT)){
        ArisGameState++;
        bounceNumCrosses = 0;
        moveDirection(0, STOP);
        reset=1;
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case REALIGNFORWARD1:
    if(getMovementSignal()){
      dirCount(9, FORWARD);
      if(reset==0 && timeLocal >= .25*WAIT_TIME){
        ArisGameState++;
        bounceNumCrosses = 0;
        reset=1;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;   
  case DEPLOYCLAW1:
    moveDirection(0, STOP);
    intensity=LOW_INTENSITY;
    if(grabAndPlace(CW, CLAW_DOWN, CLAW_UP)){
      reset=1;
      ArisGameState++;
      moveDirection(0, STOP);
    }
    break;
  case SLIGHTREVERSE1:
    if(getMovementSignal()){
      if(reset==0 && (dirCount(9, BACKWARD) || timeLocal > .5*WAIT_TIME)){
        reset=1;
        ArisGameState++;
        bounceNumCrosses = 0;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case REVERSETOLINE1:
    if(getMovementSignal()){
      if(reset==0 && timeLocal > .25*WAIT_TIME && dirCount(3, BACKWARD)){
        reset=1;
        bounceNumCrosses = 0;
        ArisGameState++;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case OVERSHOTCHECK1:
    if(getMovementSignal()){
      if(reset==0 && timeLocal > .75*WAIT_TIME &&  dirOvershoot(FORWARD)){
        reset=1;
        ArisGameState++;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case TURN1:
    if(getMovementSignal()){
      if(reset==0 && timeLocal > .75*WAIT_TIME && (turnAround(TURN_WAIT_TIME, CW) || timeLocal> 2.5*WAIT_TIME)){
        reset=1;
        ArisGameState++;
        moveDirection(0, STOP);
        turnAroundNorthState = INIT_GO_TO_LINE;
        turnAroundSouthState = INIT_GO_TO_LINE;
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case FORWARDTOLIMIT2:
    if(getMovementSignal() && reset==0 && timeLocal > .75*WAIT_TIME){
      dirCount(2, FORWARD);
      if(!digitalRead(LIMIT_FRONT)){
        bounceNumCrosses= 0;
        reset=1;
        ArisGameState++;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case REALIGNFORWARD2:
    if(getMovementSignal()){
      dirCount(9, FORWARD);
      if(reset==0 && timeLocal >= .25*WAIT_TIME){
        ArisGameState++;
        bounceNumCrosses = 0;
        reset=1;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case DEPLOYCLAW2:   
    moveDirection(0, STOP);
    if(grabAndPlace(CCW, CLAW_UP, CLAW_UP)){
      ArisGameState++;
      bounceNumCrosses=0;
      intensity=NO_INTENSITY;
      moveDirection(0, STOP);
      reset=1;
    }
    break;
  case SLIGHTREVERSE2:
    if(getMovementSignal()){
      if(reset==0 && (dirCount(9, BACKWARD) || timeLocal > .6*WAIT_TIME)){
        reset=1;
        ArisGameState++;
        bounceNumCrosses = 0;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;     
  case REVERSETOLINE2:
    if(getMovementSignal()){
      if(reset==0 && timeLocal > .25*WAIT_TIME && (dirCount(1, BACKWARD) || timeLocal > 5*WAIT_TIME)){
        reset=1;
        ArisGameState++;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case OVERSHOTCHECK2:
    if(getMovementSignal()){
      if(reset==0 && timeLocal > .5*WAIT_TIME &&  (dirOvershoot(FORWARD)||timeLocal > 1.5*WAIT_TIME)){
        reset=1;
        ArisGameState++;
        moveDirection(0, STOP);
        turnAroundNorthState = INIT_GO_TO_LINE;
        turnAroundSouthState = INIT_GO_TO_LINE;
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case  TURN2:
    if(getMovementSignal()){
      if(reset==0 && timeLocal > .75*WAIT_TIME && (turnAround(WAIT_TIME+ TURN_WAIT_TIME/3, CCW) || timeLocal>= 5.5*WAIT_TIME)){
        intensity = HIGH_INTENSITY;
        reset=1;
        ArisGameState++;
        moveDirection(0, STOP);
        bounceNumCrosses=0;
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case SLIGHTREVERSE3:
    if(getMovementSignal()){
      if(reset==0 && (dirCount(9, BACKWARD) || timeLocal > .1*WAIT_TIME)){
        reset=1;
        ArisGameState++;
        bounceNumCrosses = 0;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case  REVERSETOLINE3: 
    if(getMovementSignal()){
      if(reset==0 && timeLocal > .75*WAIT_TIME && dirCount(1, BACKWARD)){
        reset=1;
        ArisGameState++;
        moveDirection(0, STOP);
        bounceNumCrosses=0;
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case  OVERSHOTCHECK3:
    if(getMovementSignal()){
      if(reset==0 && timeLocal > .5*WAIT_TIME &&  (dirOvershoot(FORWARD) || timeLocal > 2*WAIT_TIME)){
        reset=1;
        ArisGameState++;
        bounceNumCrosses=0;
        moveDirection(0, STOP);
        turnAroundNorthState = INIT_GO_TO_LINE;
        turnAroundSouthState = INIT_GO_TO_LINE;
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case TURN3:
    if(getMovementSignal()){
      if(reset==0 && timeLocal > .75*WAIT_TIME && (turnAround(WAIT_TIME+ TURN_WAIT_TIME/6, CCW)|| timeLocal > 2*WAIT_TIME)){
        intensity = HIGH_INTENSITY;
        reset=1;
        ArisGameState++;
        moveDirection(0, STOP);
        bounceNumCrosses=0;
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case FORWARDTOLIMIT3:
    if(getMovementSignal() && reset==0 && timeLocal > WAIT_TIME){
      dirCount(2, FORWARD);
      if(!digitalRead(LIMIT_FRONT)){
        bounceNumCrosses= 0;
        reset=1;
        ArisGameState++;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case REALIGNFORWARD3:
    if(getMovementSignal()){
      dirCount(9, FORWARD);
      if(reset==0 && timeLocal >= .25*WAIT_TIME){
        ArisGameState++;
        bounceNumCrosses = 0;
        reset=1;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case DEPLOYCLAW3:   
    moveDirection(0, STOP);
    if(grabAndPlace(CW, SUPPLY, CLAW_UP)){
      ArisGameState++;
      bounceNumCrosses=0;
      intensity=HIGH_INTENSITY;
      moveDirection(0, STOP);
      reset=1;
    }
    break;
  case SLIGHTREVERSE4:
    if(getMovementSignal()){
      if(reset==0 && (dirCount(9, BACKWARD) || timeLocal>.5*WAIT_TIME)){
        reset=1;
        ArisGameState++;
        moveDirection(0, STOP);
        bounceNumCrosses=0;
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case REVERSETOLINE4:
    if(getMovementSignal()){
      if(reset==0 && timeLocal> .25*WAIT_TIME && dirCount(1, BACKWARD)){
        reset=1;
        ArisGameState++;
        moveDirection(0, STOP);
        bounceNumCrosses=0;
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case OVERSHOTCHECK4:
    if(getMovementSignal()){
      if(reset==0 && timeLocal > .5*WAIT_TIME &&  (dirOvershoot(FORWARD)||timeLocal > 2*WAIT_TIME)){
        reset=1;
        ArisGameState++;
        moveDirection(0, STOP);
        turnAroundNorthState = INIT_GO_TO_LINE;
        turnAroundSouthState = INIT_GO_TO_LINE;
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case TURN4: 
    if(getMovementSignal()){
      if(reset==0 && timeLocal > .75*WAIT_TIME && turnAround(WAIT_TIME+ TURN_WAIT_TIME/3, CW)){
        reset=1;
        ArisGameState++;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case FORWARDTOLIMIT4:
    if(getMovementSignal() && reset==0 && timeLocal > WAIT_TIME){
      dirCount(2, FORWARD);
      if(!digitalRead(LIMIT_FRONT)){
        bounceNumCrosses= 0;
        reset=1;
        ArisGameState++;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case REALIGNFORWARD4:
    if(getMovementSignal()){
      dirCount(9, FORWARD);
      if(reset==0 && timeLocal >= .25*WAIT_TIME){
        ArisGameState++;
        bounceNumCrosses = 0;
        reset=1;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case DEPLOYCLAW4:
    moveDirection(0, STOP);
    intensity=NO_INTENSITY;
    if(grabAndPlace(CCW, CLAW_DOWN, CLAW_UP)){
      reset=1;
      ArisGameState++;
      moveDirection(0, STOP);
    }
    break;

  case SLIGHTREVERSE5:
    if(getMovementSignal()){
      if(reset==0 && timeLocal> .25*WAIT_TIME && (dirCount(9, BACKWARD)|| timeLocal >= .4*WAIT_TIME)){
        reset=1;
        ArisGameState++;
        moveDirection(0, STOP);
        bounceNumCrosses=0;
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case REVERSETOLINE5:
    if(getMovementSignal()){
      if(reset==0 && timeLocal> .25*WAIT_TIME && dirCount(1, BACKWARD)){
        reset=1;
        ArisGameState++;
        moveDirection(0, STOP);
        bounceNumCrosses=0;
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case OVERSHOOTCHECK5:
    if(getMovementSignal()){
      if(reset==0 && timeLocal > .5*WAIT_TIME &&  (dirOvershoot(FORWARD)||timeLocal > 1.5*WAIT_TIME)){
        reset=1;
        ArisGameState++;
        moveDirection(0, STOP);
        turnAroundNorthState = INIT_GO_TO_LINE;
        turnAroundSouthState = INIT_GO_TO_LINE;
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case TURN5:
    if(getMovementSignal()){
      if(reset==0 && timeLocal > .5*WAIT_TIME && turnAround(WAIT_TIME+ TURN_WAIT_TIME/3, CW)){
        reset=1;
        ArisGameState++;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case TURN6: 
    if(getMovementSignal()){
      if(reset==0 && timeLocal > .5*WAIT_TIME && turnAround(WAIT_TIME+ TURN_WAIT_TIME/3, CW)){
        reset=1;
        ArisGameState=FORWARDTOLIMIT1;
        moveDirection(0, STOP);
        if(side==SECOND_SIDE){
          side = FIRST_SIDE;
        }
        else
          side = SECOND_SIDE;
      }
    }
    else
      moveDirection(0, STOP);
    break;
  }
  return ArisGameState;
}

//returns line sensor values
int getLS(int lineSensor){
  return (analogRead(lineSensor)>860)==BLACK; //850
}

void testLS(){
  Serial.println("NE-NW-E-SE-SW-W");  
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
}

boolean dirOvershoot(int dir){
  bounceAdjust();
  moveAdjusted(14, bounceErrorPowerFront, bounceErrorPowerRear, dir); //14
  if(getLS(_W_LS)==BLACK&&getLS(_E_LS)==BLACK){
    moveDirection(0,STOP);
    return true;
  }
  return false;
}

//moves backward and line tracks
//counts number of lines you pass along the way
//returns true when the number of crosses passed equals the number desired
boolean dirCount(int crosses, int dir){
  bounceAdjust();
  Serial.println(bounceNumCrosses);
  Serial.println(crosses);
  moveAdjusted(35, bounceErrorPowerFront, bounceErrorPowerRear, dir); //15 CHANGE HERE
  if(bounceNumCrosses == crosses){
    bounceNumCrosses=0;
    moveDirection(0,STOP);
    return true;
  }
  return false;
}
//moves forward towards a line and then adjusts itself over it
boolean goToLine(){
  boolean South = goToLineSouth(EAST);
  boolean North = goToLineNorth(EAST);
  return South && North;
}

boolean goToLineNorth(char dir){
  switch(newGoToLineStateNorth){
  case INIT_GO_TO_LINE:
    if(dir==EAST){
      MotorNe.write(110);
      MotorNw.write(110);
      if(getLS(_NW_LS)==BLACK)
        newGoToLineStateNorth=GO_RIGHT;
    }
    else{
      MotorNe.write(70);
      MotorNw.write(70);
      if(getLS(_NE_LS)==BLACK)
        newGoToLineStateNorth=GO_LEFT;
    }
    break;
  case GO_RIGHT:
    MotorNe.write(70);
    MotorNw.write(70);
    if(getLS(_NC_LS)==BLACK){
      newGoToLineStateNorth = GO_STOP;
    }
    if(getLS(_NE_LS)==BLACK){
      newGoToLineStateNorth = GO_LEFT;
    }
    break;
  case GO_LEFT:
    MotorNe.write(110);
    MotorNw.write(110);
    if(getLS(_NC_LS)==BLACK){
      newGoToLineStateNorth = GO_STOP;
    }
    if(getLS(_NW_LS)==BLACK){
      newGoToLineStateNorth = GO_RIGHT;
    }
    break;
  case GO_STOP:
    MotorNe.write(90);
    MotorNw.write(90);
    if(getLS(_NW_LS)==BLACK){
      newGoToLineStateNorth = GO_RIGHT;
    }
    else if(getLS(_NE_LS)==BLACK){
      newGoToLineStateNorth = GO_LEFT;
    }
    else
      return 1;
    break;
  }
  return 0;
}

boolean goToLineSouth(char dir){
  switch(newGoToLineStateSouth){
  case INIT_GO_TO_LINE:
    if(dir==EAST){
      MotorSw.write(70);
      MotorSe.write(70);
      if(getLS(_SE_LS)==BLACK)
        newGoToLineStateSouth=GO_RIGHT;
    }
    else{
      MotorSw.write(110);
      MotorSe.write(110);
      if(getLS(_SW_LS)==BLACK)
        newGoToLineStateSouth=GO_LEFT;
    }
    break;
  case GO_RIGHT:
    MotorSw.write(110);
    MotorSe.write(110);
    if(getLS(_SC_LS)==BLACK){
      newGoToLineStateSouth = GO_STOP;
    }
    if(getLS(_SE_LS)==BLACK){
      newGoToLineStateSouth = GO_LEFT;
    }
    break;
  case GO_LEFT:
    MotorSe.write(70);
    MotorSw.write(70);
    if(getLS(_SC_LS)==BLACK){
      newGoToLineStateSouth = GO_STOP;
    }
    if(getLS(_SW_LS)==BLACK){
      newGoToLineStateSouth = GO_RIGHT;
    }
    break;
  case GO_STOP:
    MotorSe.write(90);
    MotorSw.write(90);
    if(getLS(_SW_LS)==BLACK){
      newGoToLineStateSouth = GO_RIGHT;
    }
    else if(getLS(_SE_LS)==BLACK){
      newGoToLineStateSouth = GO_LEFT;
    }
    else
      return 1;
    break;  
  }
  return 0;
}

boolean turnAround(int waitTime, char dir){
  if(reset==0 && timeLocal>= waitTime){
    boolean South = turnAroundSouth(0, dir);
    boolean North = turnAroundNorth(0, dir);
    return (South && North) ;
  }
  else
    turnDirection(.25, dir);
  return false;
}

boolean turnAroundNorth(int waitTime, char dir){
  switch(turnAroundNorthState){
  case INIT_GO_TO_LINE:
    if(dir==CW){
      MotorNe.write(105);
      MotorNw.write(105);
    }
    else{
      MotorNe.write(75);
      MotorNw.write(75);
    }
    if(timeLocal>=waitTime){
      turnAroundNorthState=NONE_HIT;
    }
    break;
  case NONE_HIT:
    if(dir==CW){
      MotorNe.write(105);
      MotorNw.write(105);
      if(getLS(_NW_LS)==BLACK)
        turnAroundNorthState=GO_RIGHT;
    }
    else{
      MotorNe.write(75);
      MotorNw.write(75);
      if(getLS(_NE_LS)==BLACK)
        turnAroundNorthState=GO_LEFT;
    }
    break;
  case GO_RIGHT:
    MotorNe.write(75);
    MotorNw.write(75);
    if(getLS(_NC_LS)==BLACK){
      turnAroundNorthState = GO_STOP;
    }
    if(getLS(_NE_LS)==BLACK){
      turnAroundNorthState = GO_LEFT;
    }
    break;
  case GO_LEFT:
    MotorNe.write(105);
    MotorNw.write(105);
    if(getLS(_NC_LS)==BLACK){
      turnAroundNorthState = GO_STOP;
    }
    if(getLS(_NW_LS)==BLACK){
      turnAroundNorthState = GO_RIGHT;
    }
    break;
  case GO_STOP:
    MotorNe.write(90);
    MotorNw.write(90);
    if(getLS(_NC_LS)==BLACK )
      return true;
    else if(getLS(_NW_LS)==BLACK){
      turnAroundNorthState = GO_RIGHT;
    }
    else if(getLS(_NE_LS)==BLACK){
      turnAroundNorthState = GO_LEFT;
    }
    break;
  }
  return false;
}

boolean turnAroundSouth(int waitTime, char dir){
  switch(turnAroundSouthState){
  case INIT_GO_TO_LINE:
    if(dir==CCW){
      MotorSw.write(75);
      MotorSe.write(75);
    }
    else{
      MotorSw.write(105);
      MotorSe.write(105);
    }
    if(timeLocal>=waitTime)
      turnAroundSouthState=NONE_HIT;
    break;
  case NONE_HIT:
    if(dir==CCW){
      MotorSw.write(75);
      MotorSe.write(75);
      if(getLS(_SW_LS)==BLACK)
        turnAroundSouthState=GO_RIGHT;
    }
    else{
      MotorSw.write(105);
      MotorSe.write(105);
      if(getLS(_SE_LS)==BLACK)
        turnAroundSouthState=GO_LEFT;
    }
    break;
  case GO_RIGHT:
    MotorSw.write(105);
    MotorSe.write(105);
    if(getLS(_SC_LS)==BLACK){
      turnAroundSouthState = GO_STOP;
    }
    if(getLS(_SE_LS)==BLACK){
      turnAroundSouthState = GO_LEFT;
    }
    break;
  case GO_LEFT:
    MotorSe.write(75);
    MotorSw.write(75);
    if(getLS(_SC_LS)==BLACK){
      turnAroundSouthState = GO_STOP;
    }
    if(getLS(_SW_LS)==BLACK){
      turnAroundSouthState = GO_RIGHT;
    }
    break;
  case GO_STOP:
    MotorSe.write(90);
    MotorSw.write(90);
    if(getLS(_SC_LS)==BLACK)
      return true;
    else if(getLS(_SW_LS)==BLACK){
      turnAroundSouthState = GO_RIGHT;
    }
    else if(getLS(_SE_LS)==BLACK){
      turnAroundSouthState = GO_LEFT;
    }
    break;  
  }
  return false;
}

//adjusts error speeds for left and right motors
void bounceAdjust(){
  if(getLS(_NC_LS)==BLACK||getLS(_NE_LS)==BLACK||getLS(_NW_LS)==BLACK){
    bounceErrorPowerFront = .12*getLS(_NE_LS)-.12*getLS(_NW_LS); //.1
  }
  if(getLS(_SC_LS)==BLACK||getLS(_SE_LS)==BLACK||getLS(_SW_LS)==BLACK){
    bounceErrorPowerRear = .12*getLS(_SW_LS)-.12*getLS(_SE_LS);
  }

  if(getLS(_E_LS)==BLACK&&getLS(_W_LS)==BLACK){
    bounceOnCross=1;
  }
  else if(bounceOnCross==1 && getLS(_E_LS)==WHITE&&getLS(_W_LS)==WHITE){
    bounceOnCross=0;
    bounceNumCrosses++;
  } 
}

//move Right/Left with adjusted speeds
void moveAdjusted(float spd, float adjSpdFront,float adjSpdBack, int dir){
  switch(dir){
  case FORWARD:
    MotorSe.write(90 -spd + adjSpdBack*90);
    MotorSw.write(90 +spd + adjSpdBack*90);
    MotorNe.write(90 -spd + adjSpdFront*90);
    MotorNw.write(90 +spd + adjSpdFront*90);
    break;
  case BACKWARD:
    MotorSe.write(90 +spd + adjSpdBack*90);
    MotorSw.write(90 -spd + adjSpdBack*90);
    MotorNe.write(90 +spd + adjSpdFront*90);
    MotorNw.write(90 -spd + adjSpdFront*90);
    break;
  }
} 

//given grab/place, a first and last position, it will first move to the first position, grab or place a rod, and then move to the last location
//returns true if complete, false otherwise
boolean grabAndPlace(int grabOrPlace, int firstLocation, int lastLocation){
  switch(clawState){
  case ARM_FIRST: //go to the first location
    if(reset==0){
      if(grabOrPlace==CW)
        moveClaw(grabOrPlace);
      if(moveArm(firstLocation)){
        clawState=CLAW_GRABPLACE;
        reset=1;
      }
    }
    break;
  case CLAW_GRABPLACE:
    if(reset==0){ //grab or place the rod based on time
      moveClaw(grabOrPlace);
      if(timeLocal>=CLAW_RUN_TIME){
        clawState=ARM_LAST;
        MotorClaw.write(90);
      }
    }
    Serial.println("No Im here.");
    break;
  case ARM_LAST: //move to the last location
    if(moveArm(lastLocation)){
      MotorArm.write(90);
      clawState=ARM_FIRST;//resets for next time
      return true;
    }
    break;
  }
  return false;
}

//Spins the claw for CLAW_RUN_TIME ms in the desired direction
//returns true if complete, false if in process
void moveClaw(char dir){
  if(dir==CW){
    MotorClaw.write(180);
  }
  else if(dir==CCW){
    MotorClaw.write(0);
  }
  else{
    MotorClaw.write(90);
  }
}

//Moves the arm to the desired position based on limit switches
//returns true if complete, false if in motion
boolean moveArm(int pos){
  //sets current position
  if(!digitalRead(LIMIT_CLAW_UP)){ 
    currentPositionClaw=CLAW_UP;
  }
  else if(!digitalRead(LIMIT_CLAW_DOWN)){
    currentPositionClaw=CLAW_DOWN;
  }
  else{
    currentPositionClaw=7;
  }
  //if you want to be where you are, stop
  //otherwise, go there
  if(pos==CLAW_UP){
    MotorArm.write(140);
    Serial.println("GO UP");
  }
  else if(pos==CLAW_DOWN){
    MotorArm.write(70);
    Serial.println("GO DOWN");
  }
  else if(pos==SUPPLY){
    if(timeLocal>= 1.2* WAIT_TIME){
      MotorArm.write(140);
    }
    else
      goToSupply();
  }
  if(pos==currentPositionClaw || (pos==SUPPLY && timeLocal>=1.5*WAIT_TIME)){
    MotorArm.write(90);
    Serial.println("GOOD JOB");
    return true;
  }
  return false;
}

void goToSupply(){
  float error = SUPPLY_POSITION - analogRead(POTENTIOMETER); // between 0 and +- ~700
  float normalError =  error / (ARM_DOWN_POSITION - ARM_UP_POSITION);
  if(normalError < 0) normalError = -sqrt(-normalError);
  else normalError = sqrt(normalError); // quadratic
  int speedArm = 90 - normalError * 90;
  // make sure speed is within bounds
  if(speedArm < 0)   speedArm = 0;
  if(speedArm > 180) speedArm = 180;
  if(abs(error) < 4) { // don't run if we're close enough
    speedArm = 90;
  }
  Serial.print(error);
  Serial.print("   ");
  Serial.println(speedArm);
  MotorArm.write(speedArm);
  currentPositionPot = analogRead(POTENTIOMETER);
}

//tests motors by going in different directions
void motorTest(){
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

//teleoperated control
void teleop() {
  front_back = ppm.getChannel(2);
  left_right = ppm.getChannel(1);
  Serial.println(left_right);
  Serial.println(front_back);
  MotorNw.write(180-front_back);
  MotorSe.write(front_back);
  MotorNe.write(left_right);
  MotorSw.write(180-left_right);
}

// given a power and a direction, it will turn the robot in that direction
void turnDirection(double power, int dir){
  if(dir==CCW){
    MotorNe.write(90 - power*90);
    MotorSe.write(90 - power*90);
    MotorSw.write(90 - power*90);
    MotorNw.write(90 - power*90);
  }  
  else if(dir==CW){
    MotorNe.write(90 + power*90);
    MotorSe.write(90 + power*90);
    MotorSw.write(90 + power*90);
    MotorNw.write(90 + power*90);
  } 
  else{
    MotorNe.write(90);
    MotorSe.write(90);
    MotorSw.write(90);
    MotorNw.write(90);
  }
}

//given a power and a direction, the robot will move
void moveDirection(double power, char dir) {
  if(power > 1) power = 1;
  if(power < 0) power = 0;
  switch(dir) {
  case FORWARD_LEFT:
    MotorNe.write(90);
    MotorSe.write(90 - 90 * power);
    MotorSw.write(90);
    MotorNw.write(90 + 90 * power);
    break;
  case BACKWARD_RIGHT:
    MotorNe.write(90);
    MotorSe.write(90 + 90 * power);
    MotorSw.write(90);
    MotorNw.write(90 - 90 * power);
    break;
  case BACKWARD_LEFT:
    MotorNe.write(90 - 90 *power);
    MotorSe.write(90);
    MotorSw.write(90 + 90*power);
    MotorNw.write(90);
    break;
  case FORWARD_RIGHT:
    MotorNe.write(90 + 90 *power);
    MotorSe.write(90);
    MotorSw.write(90 - 90*power);
    MotorNw.write(90);
    break;
  case STOP:
    MotorNe.write(90);
    MotorSw.write(90);
    MotorNw.write(90);
    MotorSe.write(90);
    break;
  case RIGHT:
    MotorNe.write(90 + 90 * power);
    MotorSe.write(90 - 90 * power);
    MotorSw.write(90 - 90 * power);
    MotorNw.write(90 + 90 * power);
    break;
  case BACKWARD:
    MotorNe.write(90 + 90 * power);
    MotorSe.write(90 + 90 * power);
    MotorSw.write(90 - 90 * power);
    MotorNw.write(90 - 90 * power);
    break;
  case LEFT:
    MotorNe.write(90 - 90 * power);
    MotorSe.write(90 + 90 * power);
    MotorSw.write(90 + 90 * power);
    MotorNw.write(90 - 90 * power);
    break;
  case FORWARD:
    MotorNe.write(90 - 90 * power);
    MotorSe.write(90 - 90 * power);
    MotorSw.write(90 + 90 * power);
    MotorNw.write(90 + 90 * power);
    break;
  default:
    MotorNe.write(90);
    MotorSw.write(90);
    MotorNw.write(90);
    MotorSe.write(90);
    break;
  }
}




