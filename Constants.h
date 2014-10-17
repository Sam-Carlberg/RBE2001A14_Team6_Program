#ifndef Constants_h
#define Constants_h

/*
 *  Motor pins
 */
const static int MOTOR_SW = 8;  // labelled SOUTH on the wire on the robot
const static int MOTOR_NE = 6;  //     "    NORTH
const static int MOTOR_NW = 12; //     "    WEST
const static int MOTOR_SE = 13; //     "    EAST

const static int MOTOR_ARM  = 11;
const static int MOTOR_CLAW = 10;

/*
 *  Motor constants
 */
const static int MOTOR_MAX_CCW = 0;
const static int MOTOR_STOP    = 90;
const static int MOTOR_MAX_CW  = 180;

enum MotorDirs {
  CW,
  CCW
};

/*
 *  Driving directions for moveDirection
 */
enum Directions {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  STOP,
  FORWARD_RIGHT,
  BACKWARD_RIGHT,
  FORWARD_LEFT,
  BACKWARD_LEFT
};

/*
 *  Arm and manipulator constants
 */
const static int ARM_UP_SPEED   = 140;
const static int ARM_DOWN_SPEED = 60;
const static int ARM_TOLERANCE  = 4; // maximum allowable difference between location and setpoint

const static int INTAKE   = MOTOR_MAX_CW;  // intake rod
const static int DISPENSE = MOTOR_MAX_CCW; // dispense rod

const static int CLAW_RUN_TIME = 2307; // ms

const static int SUPPLY_POSITION   = 250; // raw pot readings at each important position
const static int ARM_UP_POSITION   = 163;
const static int ARM_DOWN_POSITION = 856;

const static int LIMIT_CLAW_DOWN = 22;
const static int LIMIT_CLAW_UP   = 23;

enum ArmPositions {
  ARM_FIRST, CLAW_GRABPLACE, ARM_LAST
};

enum TubePositions {
  CLAW_DOWN, CLAW_UP, SUPPLY
};


/*
 *  Light sensor pins
 */
const static int _E_LS  = A0;
const static int _NE_LS = A9;
const static int _NC_LS = A10;
const static int _NW_LS = A11;
const static int _W_LS  = A7;
const static int _SW_LS = A4;
const static int _SC_LS = A3;
const static int _SE_LS = A2;

/*
 *  Light sensor constants
 */
const static int LS_THRESHOLD = 860;
const static int TURN_SPEED   = 15;

const static int WHITE = 0;
const static int BLACK = 1;

/*
 *  Line tracking constants
 */
enum GoToLine {
  INIT_GO_TO_LINE,
  GO_RIGHT,
  GO_LEFT,
  GO_STOP,
  NONE_HIT,
  EAST,
  WEST
};

const static int GOTOLINE_SPEED = 20;

/*
 *  LED pins
 */
const static int LED_HIGH = 30; // high radiation LED
const static int LED_LOW  = 31; // low radiation LED
const static int LED_BT   = 38; // bluetooth status LED

/*
 *  Bluetooth
 */
const static int TEAM_NUMBER = 0x06;
const static int ALL_ROBOTS  = 0x00;

enum MessageTypes {
  STORAGE_DATA    = 0x01,
  SUPPLY_DATA     = 0x02,
  RADIATION_ALERT = 0x03,
  STOP_MOVEMENT   = 0x04,
  START_MOVEMENT  = 0x05,
  ROBOT_STATUS    = 0x06,
  HEARTBEAT       = 0x07
};

enum RadiationLevel {
  NO_RADIATION   = 0x00,
  LOW_RADIATION  = 0x2C,
  HIGH_RADIATION = 0xff
};

/*
 * Miscellaneous
 */

const static int POTENTIOMETER = A6; // analog pin for arm pot
const static int START_BUTTON  = 24; // digital pin for start button
const static int LIMIT_FRONT   = 2;  // digital pin for front limit switch

const static int WAIT_TIME = 1000;      // base timeout in millis
const static int TURN_WAIT_TIME = 1600; // base turning timeout in millis

#endif

