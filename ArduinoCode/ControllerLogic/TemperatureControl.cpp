#include "TemperatureControl.h"

#include "Configuration.h"

TemperatureControl::TemperatureControl()
  : therm1(SENS1, SENS1_TYPE)
{
  // setup output pin and set it to low state
  pinMode(OUT1, OUTPUT);
  digitalWrite(OUT1, LOW);
}

void TemperatureControl::runLoop()
{
  if (targetTemp > 0.0 && isHeating)
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
      unsigned int currentRuntime = millis() - heatingStart;
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

      const double t1 = (double) CURVE_TIME[curvePhase];
      const double t2 = (double) CURVE_TIME[curvePhase + 1];
      const double p1 = CURVE_TEMP[curvePhase];
      const double p2 = CURVE_TEMP[curvePhase + 1];
      // find target temperature with linear interpulation
      targetTemp = ((p2 - p1)/(t2 - t1)) * (((double) currentRuntime) - t1);
    }

    // TODO: add PID controller here, for now lets just use hard ON/OFF for thermal control
    if (currentTemp < targetTemp)
    {
      digitalWrite(OUT1, HIGH);
    }
    else
    {
      digitalWrite(OUT1, LOW);
    }

    // update time variable used for thermal runaway if the temperature is within spec
    const double highBound = lastTemp + THERMAL_RUNAWAY_DEGREE_DIFF;
    const double lowBound = lastTemp - THERMAL_RUNAWAY_DEGREE_DIFF;
    if (lowBound <= currentTemp && currentTemp <= lowBound)
    {
      lastTempIncrease = millis();
    }      
    lastTemp = currentTemp;
  }
  else
  {
    digitalWrite(OUT1, LOW);
  }
}

void TemperatureControl::setIsHeating(bool newValue) 
{ 
  isHeating = newValue; 
  if (newValue)
  {
    curvePhase = 0;
    heatingStart = lastTempIncrease = millis();
  }
}