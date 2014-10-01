#ifndef OmniDrive_h
#define OmniDrive_h

#include <Servo.h>

/*
 *  A class to hold all the basic functionality of controlling our omni-drive robot.
 */
class OmniDrive {

public:

  OmniDrive(int, int, int, int);

  // Stops all motors
  void stop();

  // Drives forward at the given power (-1..1)
  // Positive is forward, negative is backward
  void forward(double);

  // Drives sideways at the given power (-1..1)
  // Positive is right, negative is left
  void sideways(double);

  // Spins at the given power (-1..1).
  // Positive is counter clockwise, negative is clockwise.
  void spin(double);

  // Moves at the given front-back power and left-right power
  // Positive is forward/right
  // Negative is backward/left
  void move(double, double);

private:
  Servo lm, rm, fm, bm;
};

#endif

