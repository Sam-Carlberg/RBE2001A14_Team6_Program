#ifndef Constants_h
#define Constants_h

#include "Arduino.h"


enum Direction {
  FORWARD, LEFT, RIGHT, BACKWARD,
  F_L, F_R, B_L, B_R,
  NONE
};

/*
 *  PACKET CONSTANTS
 */
const byte TEAM_NUMBER = 0x06;

const byte PACKET_HEADER = 0x5F;

// Message Type enumeration
enum MessageType {
  STORAGE_AVAIL = 0x01,
  SUPPLY_AVAIL  = 0x02,
  RAD_ALERT     = 0x03,
  STOP          = 0x04,
  RESUME        = 0x05,
  ROBOT_STATUS  = 0x06,
  HEARTBEAT     = 0x07
};

// Radiation Level enumeration
enum RadiationLevel {
  SPENT = 0x2C,
  NEW   = 0xFF
};

// Robot Movement Status enumeration
enum MovementStatus {
  STOPPED = 0x01,
  TELEOP  = 0x02,
  AUTON   = 0x03
};

// Robot Gripper Status enumeration
enum GripperStatus {
  CARRYING = 0x01,
  EMPTY    = 0x02
};

// Robot Operation Status enumeration
enum OperationStatus {
  GRIP_ATTEMPT_IN_PROGRESS = 0x01,
  GRIP_RELEASE_IN_PROGRESS = 0x02,
  DRIVING_TO_ROD           = 0x03,
  DRIVING_TO_STORAGE       = 0x04,
  DRIVING_TO_SUPPLY        = 0x05,
  IDLE                     = 0x06
};

/*
 *  MOTOR CONSTANTS
 */

const int LM_PORT = 1; // left motor
const int RM_PORT = 2; // right motor
const int FM_PORT = 3; // front motor
const int BM_PORT = 4; // back motor

const int MOTOR_MAX_CW 	= 0;
const int MOTOR_STOP 	= 90;
const int MOTOR_MAX_CCW = 180;


/*
 *  ARM AND MANIPULATOR CONSTANTS
 */

#endif
