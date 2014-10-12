#ifndef Constants_h
#define Constants_h

#include "Arduino.h"

/*
 *  MOTOR CONSTANTS
 */

// pins
// These are the only pins where 393s will work. No idea why
const static int MOTOR_SW = 8;
const static int MOTOR_NE = 6;
const static int MOTOR_NW = 12;
const static int MOTOR_SE = 13;

const static int MOTOR_ARM = 11;
const static int MOTOR_CLAW = 10;

// speeds
const static int MOTOR_MAX_CW 	= 180;
const static int MOTOR_STOP 	= 90;
const static int MOTOR_MAX_CCW = 0;

/*
 *  LINE SENSOR CONSTANTS
 */

const static int BLACK = 1;
const static int WHITE = 0;

const static int ARM_POT_PIN = A6;

const static int ARM_UP     = 163;
const static int ARM_DOWN   = 856;
const static int ARM_SUPPLY = 250; // tune this
const static int ARM_TOLERANCE = 10;

#endif
