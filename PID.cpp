#include "Arduino.h"
#include "PID.h"

PID::PID(double p, double i, double d, double minOut, double maxOut): 
_p(p), _i(i), _d(d), _minOut(minOut), _maxOut(maxOut) { 
}

double PID::get() {
  return _output;
}

void PID::reset() {
  _lastTime = 0;
  _lastError = 0;
  _error = 0;
  _output = 0;
}

void PID::setSetpoint(double setPoint) {
  _setpoint = setPoint;
}

double PID::update(double input) {
  if (_lastTime == 0) {
    _lastTime = millis();
    return _output;
  }

  _error = _setpoint - input;
  _iTerm += _error * dt / 1000.0;
  _dTerm = (_error - _lastError) * 1000.0 / dt;

  _output = _p * _error + _i * _iTerm - _d * _dTerm;

  if (_output > _maxOut) _output = _maxOut;
  if (_output < _minOut) _output = _minOut;

  _lastError = _error;
  _lastTime = millis();

  return _output;
}

