/**
 * 
 * OUTLINE:
 * 
 * 1. Global variables and objects
 * 2. Interrupts
 * 3. Setup
 * 4. Loop
 * 
 * Bluetooth functions:           BluetoothUtils.ino
 * Driving functions:             Driving.ino
 * LineTracking functions:        LineTracking.ino
 * Arm and manipulator functions: Manipulator.ino
 * 
 */

#include "Servo.h"
#include "TimerOne.h"
#include "BluetoothMaster.h"
#include "ReactorProtocol.h"
#include "Constants.h"

boolean started = false; // has the start button been pressed?

// Time variables
volatile long unsigned int timeTotal = 0;  // time in millis since the clock started ticking
volatile long unsigned int timeLocal = 0;  // time in millis since last reset
boolean reset = true;                      // should the local timer be reset?

int storageTarget = 3; // the storage lane to go to. Default to 3 for testing and to make it interesting even without bluetooth
int supplyTarget  = 2; // the supply lane to go to


// BLUETOOTH VARIABLES
boolean shouldMove   = true;  // should the robot be moving?
boolean sendHB       = false; // should we send the heartbeat message?
boolean sendRadAlert = false; // should we send the radiation alert message?

int radiationLevel   = NO_RADIATION; // the current radiation level

boolean onLowSide    = true;  // are we on the side of the field with the low-numbered lanes?
boolean connected    = false; // is Bluetooth connected?

// Linetracking bounce variables
float bounceErrorPowerFront = 0;       // between 0 and basePower
float bounceErrorPowerRear  = 0;       // between 0 and basePower
int bounceNumCrosses        = 0;       // haven't crossed any lines yet
boolean bounceOnCross       = 0;       

// line tracking variables
int newGoToLineStateSouth = INIT_GO_TO_LINE;
int newGoToLineStateNorth = INIT_GO_TO_LINE;

int turnAroundSouthState = INIT_GO_TO_LINE;
int turnAroundNorthState = INIT_GO_TO_LINE;

// Arm states
int currentPositionClaw = 7;
int clawState;

/*
 *  Enumeration of all states the robot can be in.
 */
enum GameState {
  GOTOLINE,
  GO_TO_REACTOR,
  ALIGN_WITH_REACTOR,
  GRAB_SPENT_ROD,
  SLIGHTREVERSE1,
  DRIVE_TO_STORAGE_LANE,
  OVERSHOTCHECK1,
  TURN_TO_STORAGE_LANE, 
  DRIVE_TO_STORAGE_TUBE,
  ALIGN_WITH_STORAGE_TUBE, // realign to the storage tube
  DEPOSIT_SPENT_ROD,       // store the spent rod
  SLIGHTREVERSE2,          // back up past the line in front of the storage tube
  BACK_TO_CENTER1,         // back up to the center line
  OVERSHOTCHECK2,          // make sure we didn't overshoot
  TURN_TO_CENTER1,         // turn to align with the center line
  SLIGHTREVERSE3,  
  DRIVE_TO_SUPPLY_LANE,    // back up to the supply line
  OVERSHOTCHECK3,          // make sure we didn't overshoot the line
  TURN_TO_SUPPLY_LANE,     // turn down the supply line
  DRIVE_TO_SUPPLY_TUBE,    // drive to the supply tube
  ALIGN_WITH_SUPPLY_TUBE,  // realign with the tube
  GRAB_NEW_ROD,            // grab a new rod
  SLIGHTREVERSE4,          // back up past the line in front of the supply tube
  BACK_TO_CENTER2,         // back up to the storage tube
  OVERSHOTCHECK4,          // make sure we didn't overshoot
  TURN_TO_CENTER2,         // turn down the center line
  DRIVE_BACK_TO_REACTOR,   // drive back to the reactor
  ALIGN_WITH_REACTOR2,     // realign with the reactor
  DEPOSIT_NEW_ROD,         // deposit the rod
  SLIGHTREVERSE5,          // back up past the line in front of the reactor
  TURN_TO_OTHER_SIDE       // do a 180 on the center line
};

// the state that the robot should currently be executing
int currentState = GOTOLINE;

// Drive motors
Servo MotorNW;
Servo MotorSE;
Servo MotorNE;
Servo MotorSW;

// Arm and manipulator motors
Servo MotorArm;
Servo MotorClaw;

/* 
 *  Timer ISR. Called every millisecond.
 */
void incrementTime() {
  if(reset) {
    timeLocal = 0;
    reset     = 0;
  } 
  else if (shouldMove) {
    timeLocal++;
  }

  timeTotal++; // time is measured in milliseconds
  if(timeTotal % 2000 == 0) {
    sendHB       = true;
    sendRadAlert = true;
  }
}

void setup() {

  // attach motors
  // all 393s, so they have different duty cycles than default
  MotorNW.attach(MOTOR_NW,     1000, 2000);
  MotorSE.attach(MOTOR_SE,     1000, 2000);
  MotorNE.attach(MOTOR_NE,     1000, 2000);
  MotorSW.attach(MOTOR_SW,     1000, 2000); 
  MotorArm.attach(MOTOR_ARM,   1000, 2000);
  MotorClaw.attach(MOTOR_CLAW, 1000, 2000);

  // init LED pins as output
  pinMode(LED_LOW,  OUTPUT);
  pinMode(LED_HIGH, OUTPUT);
  pinMode(LED_BT,   OUTPUT);

  // init limit switches and push buttons as inputs with internal pullups enabled
  pinMode(LIMIT_FRONT,     INPUT_PULLUP);
  pinMode(LIMIT_CLAW_DOWN, INPUT_PULLUP);
  pinMode(LIMIT_CLAW_UP,   INPUT_PULLUP);
  pinMode(START_BUTTON,    INPUT_PULLUP);

  // init light sensors as inputs with NO pullups
  pinMode(_NE_LS, INPUT);
  pinMode(_NW_LS, INPUT);
  pinMode(_SE_LS, INPUT);
  pinMode(_SW_LS, INPUT);
  pinMode(_NC_LS, INPUT);
  pinMode(_SC_LS, INPUT);
  pinMode(_E_LS,  INPUT);
  pinMode(_W_LS,  INPUT);

  // init the potentiometer as an input with NO pullups
  pinMode(POTENTIOMETER, INPUT);

  Serial.begin(9600);

  Timer1.initialize(1000);               // initializes timer with a period of 1ms
  Timer1.attachInterrupt(incrementTime); // increments time every 1ms
}

/*
 *  Main loop. The majority of the logic is in playGame() a few lines down
 */
void loop() {
  // wait for bluetooth to connect
  if(!connected) {
    connected = readPacket();
    return;
  }

  digitalWrite(LED_BT, HIGH); // turn on an LED to show we have a connection

  sendMessages(); // send messages, even if the robot hasn't started moving

  // wait for the start button to be pressed
  if(!started) {
    started = digitalRead(START_BUTTON) == 0;
    return;
  }

  readPacket();
  playGame();
  setRadiationLED();
}

/*
 *  Sets the radiation laert LEDs based on the current radiation level
 */
void setRadiationLED() {
  if(radiationLevel == NO_RADIATION) {
    digitalWrite(LED_HIGH, LOW);
    digitalWrite(LED_LOW,  LOW);
  }
  else if(radiationLevel == LOW_RADIATION) {
    digitalWrite(LED_HIGH, LOW);
    digitalWrite(LED_LOW,  HIGH);
  }
  else if(radiationLevel == HIGH_RADIATION) {
    digitalWrite(LED_HIGH, HIGH);
    digitalWrite(LED_LOW,  HIGH);
  }
}

/*
 *  400-line main method that executes the state machine logic
 */
int playGame() {

  switch(currentState) {

  case GOTOLINE:
    if(shouldMove) {
      if(goToLine() == 1) {
        currentState++;
        reset=1;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case GO_TO_REACTOR:
    if(shouldMove) {
      dirCount(9, FORWARD);
      if(!digitalRead(LIMIT_FRONT)) {
        currentState++;
        bounceNumCrosses = 0;
        moveDirection(0, STOP);
        reset=1;
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case ALIGN_WITH_REACTOR:
    if(shouldMove) {
      dirCount(9, FORWARD);
      if(reset == 0 && timeLocal >= .25 * WAIT_TIME) {
        currentState++;
        bounceNumCrosses = 0;
        reset=1;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;   

  case GRAB_SPENT_ROD:
    moveDirection(0, STOP);
    radiationLevel=LOW_RADIATION;
    if(grabAndPlace(CW, CLAW_DOWN, CLAW_UP, CLAW_RUN_TIME * .95)) {
      reset=1;
      currentState++;
      moveDirection(0, STOP);
    }
    break;

  case SLIGHTREVERSE1:
    if(shouldMove) {
      if(reset == 0 && (dirCount(9, BACKWARD) || timeLocal > .5 * WAIT_TIME)) {
        reset=1;
        currentState++;
        bounceNumCrosses = 0;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case DRIVE_TO_STORAGE_LANE:
    if(shouldMove) {
      if(reset == 0 && timeLocal > .25 * WAIT_TIME && dirCount(onLowSide ? storageTarget : 5 - storageTarget, BACKWARD)) {
        reset=1;
        bounceNumCrosses = 0;
        currentState++;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case OVERSHOTCHECK1:
    if(shouldMove) {
      if(reset == 0 && timeLocal > 0.75 * WAIT_TIME &&  dirOvershoot(FORWARD)) {
        reset=1;
        currentState++;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case TURN_TO_STORAGE_LANE:
    if(shouldMove) {
      if(reset == 0 && timeLocal > 0.75 * WAIT_TIME && (turnAround(TURN_WAIT_TIME, getDirToStorage()) || timeLocal> 2.5 * WAIT_TIME)) { //CW
        reset=1;
        currentState++;
        moveDirection(0, STOP);
        turnAroundNorthState = INIT_GO_TO_LINE;
        turnAroundSouthState = INIT_GO_TO_LINE;
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case DRIVE_TO_STORAGE_TUBE:
    if(shouldMove && reset == 0 && timeLocal > 0.75 * WAIT_TIME) {
      dirCount(2, FORWARD);
      if(!digitalRead(LIMIT_FRONT)) {
        bounceNumCrosses= 0;
        reset=1;
        currentState++;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case ALIGN_WITH_STORAGE_TUBE:
    if(shouldMove) {
      dirCount(9, FORWARD);
      if(reset == 0 && timeLocal >= .25 * WAIT_TIME) {
        currentState++;
        bounceNumCrosses = 0;
        reset=1;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case DEPOSIT_SPENT_ROD:   
    moveDirection(0, STOP);
    if(grabAndPlace(CCW, CLAW_UP, CLAW_UP, CLAW_RUN_TIME * 2)) {
      currentState++;
      bounceNumCrosses=0;
      radiationLevel=NO_RADIATION;
      moveDirection(0, STOP);
      reset=1;
    }
    break;

  case SLIGHTREVERSE2:
    if(shouldMove) {
      if(reset == 0 && (dirCount(9, BACKWARD) || timeLocal > .6 * WAIT_TIME)) {
        reset=1;
        currentState++;
        bounceNumCrosses = 0;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;    

  case BACK_TO_CENTER1:
    if(shouldMove) {
      if(reset == 0 && timeLocal > .25 * WAIT_TIME && (dirCount(1, BACKWARD) || timeLocal > 5 * WAIT_TIME)) {
        reset=1;
        currentState++;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case OVERSHOTCHECK2:
    if(shouldMove) {
      if(reset == 0 && timeLocal > .5 * WAIT_TIME &&  (dirOvershoot(FORWARD) || timeLocal > 1.5 * WAIT_TIME)) {
        reset=1;
        currentState++;
        moveDirection(0, STOP);
        turnAroundNorthState = INIT_GO_TO_LINE;
        turnAroundSouthState = INIT_GO_TO_LINE;
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case TURN_TO_CENTER1:
    if(shouldMove) {
      if(reset == 0 && timeLocal > 0.75 * WAIT_TIME && (turnAround(WAIT_TIME+ TURN_WAIT_TIME/3, getDirToSupply()) || timeLocal>= 5.5 * WAIT_TIME)) { //CCW
        reset=1;
        currentState++;
        moveDirection(0, STOP);
        bounceNumCrosses=0;
      }
    }
    else
      moveDirection(0, STOP);
    break;
  case SLIGHTREVERSE3:
    if(shouldMove) {
      if(reset == 0 && (dirCount(9, BACKWARD) || timeLocal > .1 * WAIT_TIME)) {
        reset=1;
        currentState++;
        bounceNumCrosses = 0;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case DRIVE_TO_SUPPLY_LANE: 
    if(shouldMove) {
      if(reset == 0 && timeLocal > 0.75 * WAIT_TIME && dirCount(abs(storageTarget - supplyTarget), BACKWARD)) {
        reset=1;
        currentState++;
        moveDirection(0, STOP);
        bounceNumCrosses=0;
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case OVERSHOTCHECK3:
    if(shouldMove) {
      if(reset == 0 && timeLocal > .5 * WAIT_TIME &&  (dirOvershoot(FORWARD) || timeLocal > 2 * WAIT_TIME)) {
        reset=1;
        currentState++;
        bounceNumCrosses=0;
        moveDirection(0, STOP);
        turnAroundNorthState = INIT_GO_TO_LINE;
        turnAroundSouthState = INIT_GO_TO_LINE;
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case TURN_TO_SUPPLY_LANE:
    if(shouldMove) {
      if(reset == 0 && timeLocal > 0.75 * WAIT_TIME && (turnAround(WAIT_TIME+ TURN_WAIT_TIME/6, getDirToSupply()) ||  timeLocal > 2 * WAIT_TIME)) {//CCW
        radiationLevel = HIGH_RADIATION;
        reset=1;
        currentState++;
        moveDirection(0, STOP);
        bounceNumCrosses=0;
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case DRIVE_TO_SUPPLY_TUBE:
    if(shouldMove && reset == 0 && timeLocal > WAIT_TIME) {
      dirCount(2, FORWARD);
      if(!digitalRead(LIMIT_FRONT)) {
        bounceNumCrosses= 0;
        reset=1;
        currentState++;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case ALIGN_WITH_SUPPLY_TUBE:
    if(shouldMove) {
      dirCount(9, FORWARD);
      if(reset == 0 && timeLocal >= .25 * WAIT_TIME) {
        currentState++;
        bounceNumCrosses = 0;
        reset=1;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case GRAB_NEW_ROD:   
    moveDirection(0, STOP);
    radiationLevel=HIGH_RADIATION;
    if(grabAndPlace(CW, SUPPLY, CLAW_UP, CLAW_RUN_TIME * 1/2 * .85)) {
      currentState++;
      bounceNumCrosses=0;
      moveDirection(0, STOP);
      reset=1;
    }
    break;

  case SLIGHTREVERSE4:
    if(shouldMove) {
      if(reset == 0 && (dirCount(9, BACKWARD) || timeLocal>.5 * WAIT_TIME)) {
        reset=1;
        currentState++;
        moveDirection(0, STOP);
        bounceNumCrosses=0;
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case BACK_TO_CENTER2:
    if(shouldMove) {
      if(reset == 0 && timeLocal> .25 * WAIT_TIME && dirCount(1, BACKWARD)) {
        reset=1;
        currentState++;
        moveDirection(0, STOP);
        bounceNumCrosses=0;
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case OVERSHOTCHECK4:
    if(shouldMove) {
      if(reset == 0 && timeLocal > .5 * WAIT_TIME &&  (dirOvershoot(FORWARD) || timeLocal > 2 * WAIT_TIME)) {
        reset=1;
        currentState++;
        moveDirection(0, STOP);
        turnAroundNorthState = INIT_GO_TO_LINE;
        turnAroundSouthState = INIT_GO_TO_LINE;
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case TURN_TO_CENTER2: 
    if(shouldMove) {
      if(reset == 0 && timeLocal > 0.75 * WAIT_TIME && turnAround(WAIT_TIME+ TURN_WAIT_TIME/3, getDirToStorage())) {//CW
        reset=1;
        currentState++;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case DRIVE_BACK_TO_REACTOR:
    if(shouldMove && reset == 0 && timeLocal > WAIT_TIME) {
      dirCount(2, FORWARD);
      if(!digitalRead(LIMIT_FRONT)) {
        bounceNumCrosses= 0;
        reset=1;
        currentState++;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case ALIGN_WITH_REACTOR2:
    if(shouldMove) {
      dirCount(9, FORWARD);
      if(reset == 0 && timeLocal >= .25 * WAIT_TIME) {
        currentState++;
        bounceNumCrosses = 0;
        reset=1;
        moveDirection(0, STOP);
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case DEPOSIT_NEW_ROD:
    moveDirection(0, STOP);
    if(grabAndPlace(DISPENSE, CLAW_DOWN, CLAW_UP, CLAW_RUN_TIME * 3)) {
      reset=1;
      currentState++;
      moveDirection(0, STOP);
      radiationLevel=NO_RADIATION;
    }
    break;

  case SLIGHTREVERSE5:
    if(shouldMove) {
      if(reset == 0 && timeLocal> .25 * WAIT_TIME && (dirCount(9, BACKWARD) ||  timeLocal >= .5 * WAIT_TIME)) {
        reset=1;
        currentState++;
        turnAroundNorthState = INIT_GO_TO_LINE;
        turnAroundSouthState = INIT_GO_TO_LINE;
        moveDirection(0, STOP);
        bounceNumCrosses=0;
      }
    }
    else
      moveDirection(0, STOP);
    break;

  case TURN_TO_OTHER_SIDE: 
    if(shouldMove) {
      if(reset == 0 && timeLocal > .5 * WAIT_TIME && turnAround(WAIT_TIME + TURN_WAIT_TIME/3, CW)) {
        reset=1;
        currentState=GO_TO_REACTOR;
        moveDirection(0, STOP);
        onLowSide = !onLowSide;
      }
    }
    else
      moveDirection(0, STOP);
    break;
  }
  return currentState;
}

