#pragma once
// thermal resistor library -> https://github.com/miguel5612/ThermistorLibrary
#include <thermistor.h>
#include "Configuration.h"

class TemperatureControl
{
public:
  TemperatureControl();

  // main loop for temperature control
  void runLoop();

  // getters and setters for target temperature
  inline void setTargetTemp(double temp) { targetTemp = min(MAX_TEMP, max(temp, 0)); }
  inline double getTargetTemp() const { return targetTemp; }

  // enable/disable use of reflow curve
  inline void setUseReflowCurve(bool enabled) { useCurve = enabled; }
  inline bool getUseReflowCurve() const { return useCurve; }

  // enable/disable heating of the plate
  void setIsHeating(bool newValue);
  inline bool getIsHeating() const { return isHeating; }

  // return timestamp where we started to heat the plate
  inline unsigned long getHeaterStartTime() const { return heatingStart; }
  // returns last measured temp in C
  inline double getLastTemp() const { return lastTemp; }
  // returns true if we have been heating for a while and there is no temperature increase on the sensor to avoid fire
  inline bool isThermalRunawayDetected() const { return thermalRunawayDetected; }

private:
  thermistor therm1;
  // thermal runaway related variables
  unsigned long lastTempIncrease  { 0 };
  double lastTemp { 0.0 };
  // heating related variables
  unsigned long heatingStart { 0 };
  double targetTemp { 0.0 };
  bool isHeating { false };
  // reflow curve related variables
  char curvePhase { 0 };
  bool useCurve { false };
  // flag to detect if sensor or heater is not working as expected
  bool thermalRunawayDetected { false };

  bool checkThermalRunaway();
};
