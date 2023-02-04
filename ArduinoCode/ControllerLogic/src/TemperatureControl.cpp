#include "TemperatureControl.h"

TemperatureControl::TemperatureControl()
  : therm1(SENS1, SENS1_TYPE)
{
  // setup output pin and set it to low state
  pinMode(OUT1, OUTPUT);
  digitalWrite(OUT1, LOW);
}

void TemperatureControl::runLoop()
{
  if ((targetTemp > 0.0 || useCurve) && isHeating)
  {
    double currentTemp = therm1.analog2temp();
    // detect thermal runaway
    if (millis() - lastTempIncrease >= THERMAL_RUNAWAY_TIMEOUT)
    {
      isHeating = false;
      thermalRunawayDetected = true;
    }

    // find target temperature from curve
    // update target temperature from reflow curve 
    if (useCurve)
    {
      // find time and target temp
      double currentRuntime = (double) ((millis() - heatingStart) / 1000);
      // check to see if we have more points in curve, if not then disable heating
      if (curvePhase + 1 >= CURVE_SIZE)
      {
        targetTemp = 0.0;
        isHeating = false;
        digitalWrite(OUT1, LOW);
        return;
      }
      
      if (currentRuntime >= CURVE_TIME[curvePhase + 1])
      {
        curvePhase++;
      }

      const double t1 = CURVE_TIME[curvePhase];
      const double t2 = CURVE_TIME[curvePhase + 1];
      const double p1 = CURVE_TEMP[curvePhase];
      const double p2 = CURVE_TEMP[curvePhase + 1];
      if (p1 == p2)
      {
        targetTemp = p2;
      }
      else
      {
        // find target temperature with linear interpulation
        targetTemp = ((p2 - p1) / (t2 - t1)) * (((double) currentRuntime) - t1);
      }
    }

    // TODO: add PID controller here (because of thermal mass of the heater), and adjust thermal runaway code to take into account state of the output pin
    const auto now = millis();
    const auto dt = static_cast<double>(now - lastPidTime) / 1000.0;
    const double diff = targetTemp - currentTemp;
    integral = integral + diff * static_cast<double>(dt);
    const double dxdt = (diff - lastDiff) / static_cast<double>(dt);
    lastDiff = diff;
    lastPidTime = now;

    const double output = diff * P + integral * I + dxdt * D;
    if (output > 0.0)
    {
      digitalWrite(OUT1, HIGH);
    }
    else
    {
      digitalWrite(OUT1, LOW);
    }
    Serial.println(output);

    // update time variable used for thermal runaway if the temperature is within spec
    const double highBound = lastTemp + THERMAL_RUNAWAY_DEGREE_DIFF;
    const double lowBound = lastTemp - THERMAL_RUNAWAY_DEGREE_DIFF;
    if (lowBound <= currentTemp && currentTemp <= highBound)
    {
      lastTempIncrease = millis();
    }      
    lastTemp = currentTemp;
  }
  else
  {
    lastTemp = therm1.analog2temp();
    digitalWrite(OUT1, LOW);
  }
}

void TemperatureControl::setIsHeating(bool newValue) 
{ 
  isHeating = newValue; 
  if (newValue)
  {
    curvePhase = 0;
    lastPidTime = heatingStart = lastTempIncrease = millis();
    lastDiff = 0.0;
    integral = 0.0;
  }
}
