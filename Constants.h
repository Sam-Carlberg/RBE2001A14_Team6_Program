#ifndef Constants_h
#define Constants_h

#include "Arduino.h"

/*
 *  MOTOR CONSTANTS
 */

// pins
// These are the only pins where 393s will work. No idea why
const static byte MOTOR_SW = 8;
const static byte MOTOR_NE = 6;
const static byte MOTOR_NW = 12;
const static byte MOTOR_SE = 13;

const static byte MOTOR_ARM = 11;
const static byte MOTOR_CLAW = 10;

// speeds
const static byte MOTOR_MAX_CW 	= 180;
const static byte MOTOR_STOP 	  = 90;
const static byte MOTOR_MAX_CCW = 0;

/*
 *  LINE SENSOR CONSTANTS
 */

const static byte BLACK = 1;
const static byte WHITE = 0;

#endif
