#ifndef PID_h
#define PID_h

class PID {

public:

  /*
     * Constructor
   */
  PID(double p, double i, double d, double minOut, double maxOut);

  /*
     * Sets the setpoint to the given value.
   */
  void setSetpoint(double setpoint);

  /*
     * Gets the output of this PID controller
   */
  double get();

  /*
     * Resets this controller.
   */
  void reset();

  /*
     * Updates this controller with a new input.
   */
  double update(double input);


  const static double dt = 20;    // millis

private:
  double _p, _i, _d; 			// control constants
  double _error;				// error = setpoint - input
  double _setpoint;			// the value the input should approach
  double _output;				// the output value
  double _minOut, _maxOut;	// minimum and maximum possible outputs
  double _lastTime;			// the last time the update function was called
  double _iTerm;              // storage for i term in equation
  double _dTerm, _lastError;	// storage for d term
};

#endif

