#ifndef PacketConstants_h
#define PacketConstants_h

const static byte TEAM_NUMBER = 0x06;

/*
 *  Message type constants
 */
enum MessageType {
  STORAGE_AVAILABILITY = 0x01,
  SUPPLY_AVAILABILITY  = 0x02,
  RADIATION_ALERT      = 0x03,
  STOP_MOVEMENT        = 0x04,
  START_MOVEMENT       = 0x05,
  ROBOT_STATUS         = 0x06,
  HEARTBEAT            = 0x07
};

/*
 *  Radiation level constants
 */
enum RadiationLevel {
  NO_RAD = 0x00,
  CARRYING_SPENT_ROD = 0x2C,
  CARRYING_NEW_ROD   = 0xFF
};

/*
 *  Robot status data bytes
 */
enum MovementStatus {
  STOPPED      = 0x01, 
  TELEOPERATED = 0x02, 
  AUTONOMOUS   = 0x03
};

enum GripperStatus {
  NO_ROD   = 0x01, 
  HAVE_ROD = 0x02
};

enum OperationStatus {
  GRIP_ATTEMPT     = 0x01, 
  GRIP_RELEASE     = 0x02, 
  DRIVE_TO_REACTOR = 0x03, 
  DRIVE_TO_STORAGE = 0x04, 
  DRIVE_TO_SUPPLY  = 0x05, 
  IDLE             = 0x06
};

#endif