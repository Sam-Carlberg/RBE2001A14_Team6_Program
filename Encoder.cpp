#include "Arduino.h"

#define TICKS_PER_REV 90
#define TIME_UNIT_MULT 1000000	// multiplier for base units to seconds

class Encoder {

public:
  Encoder(int _pinA, int _pinB): 
  pinA(_pinA), pinB(_pinB){
    n = LOW;
  }

  // Gets the angular velocity in RPM.
  double get() {

    const double time = micros();

    if(lastTime == 0){
      lastTime = time;
      return 0;
    }

    n = digitalRead(pinA);	// read channel A
    if(~pinALast & n) {		// if channel A is falling
      if(digitalRead(pinB)) count--;	// decrement count if channel B is HIGH (1)
      else count++;					// increment count if channel B is LOW (0)

      rate = count / (time  - lastTime);
      rate *= TIME_UNIT_MULT;
      rate /= TICKS_PER_REV;
      rate *= 60;
      // reset counter
      count = 0;
      lastTime = time;
    }

    pinALast = n;
    return rate;
  }

private:
  int pinA, pinB;
  int count;
  int lastTime;
  int pinALast;
  int n;
  double rate;

};

