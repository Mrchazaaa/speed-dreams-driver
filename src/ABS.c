#include <stdio.h>
#include "ABS.h"

#define OFF 0 //state used to represent that ABS is non-active on selected wheels

static float TIMERS[4] = {-1, -1, -1, -1}; //timer for each wheel (-1 indicates that the timer is not being used)
static int phaseStates[4] = {OFF, OFF, OFF, OFF}; //used to remember which state the ABS algorithm is currently in (one for each wheel) 

static float *wheelBrakeCMD[4]; 
static float wheelSpinVelocity[4] = {0, 0, 0, 0};
static float wheelSpinAcceleration[4] = {0, 0, 0, 0};
static float wheelSlipAcceleration[4] = {0, 0, 0, 0}; 
static float wheelSlip[4] = {0, 0, 0, 0}; 
static float lastTimeStamp = 0; //store the last time that ABS was called
static float deltaTime = 0; //time since last ABS call
static float vehicleSpeed;

//getter functions that allow main system to see variables calculated by ABS (USED IN DEBUGGING) 
int   getPhaseStates(int index) { return phaseStates[index]; }
float getWheelSpinVelocity(int index) { return wheelSpinVelocity[index]; }
float getWheelSpinAcceleration(int index) { return wheelSpinAcceleration[index]; }
float getWheelSlipAcceleration(int index) { return wheelSlipAcceleration[index]; }
float getWheelSlip(int index) { return wheelSlip[index]; }
float getDeltaTime() { return deltaTime; }
float getVehicleSpeed() { return vehicleSpeed; }

//used to determine reference speed of vehicle (just use fastest wheel speed, may need to update this...)
float maxWheelVelocity(float wheelVelocity[4]) {
  float maxVelocity = wheelVelocity[0];

  int i;
  for ( i = 1 ; i < 4 ; i++ )
  {
    if (wheelVelocity[i] > maxVelocity) {
      maxVelocity = wheelVelocity[i];
    }
  }

  return maxVelocity;
}

void cycleABS( float newInputPressure, float *brakeCMD[4], float *newWheelSpinVelocity[4], float newTimeStamp, float refSpeed )
{
  //if this is the first time that ABS has been called
  if (lastTimeStamp == 0) { 
    //set the last time stamp to now
    lastTimeStamp = newTimeStamp;
    //update pointers to brake command variables 
    wheelBrakeCMD[0] = brakeCMD[0];
    wheelBrakeCMD[1] = brakeCMD[1];
    wheelBrakeCMD[2] = brakeCMD[2];
    wheelBrakeCMD[3] = brakeCMD[3];
  }

  //calculate time since last ABS call
  deltaTime = newTimeStamp - lastTimeStamp;

  //wheel1 spinvel is: 0.159155 * *wheelSpinVelocity[0] * 2 * PI * oCar->_wheelRadius(0)
  float velocityConst = 0.159155 * 2 * PI * WHEEL_RADIUS_STATIC; 

  //IMPLEMENT
  //need to add code to accurately calculate current wheel velocity (not just using system calc)
  //printf("speed syst: %.6f \n", refSpeed);
  //printf("speed mine: %.6f \n", *newWheelSpinVelocity[0] * velocityConst);
  //printf("speed syst: %.6f %.6f \n", refSpeed, *newWheelSpinVelocity[0] * velocityConst);
  //printf("speed syst: %.6f %.6f \n", refSpeed, ((*newWheelSpinVelocity[0] * velocityConst) + (*newWheelSpinVelocity[1] * velocityConst) + (*newWheelSpinVelocity[2] * velocityConst) + (*newWheelSpinVelocity[3] * velocityConst) )/4 );
  printf("speed syst: %.6f %.6f \n", refSpeed, ((*newWheelSpinVelocity[FL] * velocityConst) + (*newWheelSpinVelocity[FR] * velocityConst) )/2 );

  //update wheel acceleration values
  wheelSpinAcceleration[0] = ((*newWheelSpinVelocity[0] * velocityConst) - wheelSpinVelocity[0])/deltaTime;
  wheelSpinAcceleration[1] = ((*newWheelSpinVelocity[1] * velocityConst) - wheelSpinVelocity[1])/deltaTime;
  wheelSpinAcceleration[2] = ((*newWheelSpinVelocity[2] * velocityConst) - wheelSpinVelocity[2])/deltaTime;
  wheelSpinAcceleration[3] = ((*newWheelSpinVelocity[3] * velocityConst) - wheelSpinVelocity[3])/deltaTime;
  
  //update wheel spin velocity
  wheelSpinVelocity[0] = *newWheelSpinVelocity[0] * velocityConst;
  wheelSpinVelocity[1] = *newWheelSpinVelocity[1] * velocityConst;
  wheelSpinVelocity[2] = *newWheelSpinVelocity[2] * velocityConst;
  wheelSpinVelocity[3] = *newWheelSpinVelocity[3] * velocityConst;

  //IMPLEMENT
  //calculate vehicle speed (just use fastest wheel method or come up with new calc method)
  vehicleSpeed = refSpeed;//maxWheelVelocity(wheelSpinVelocity);

  //update wheel slip acceleration
  wheelSlipAcceleration[0] = (((vehicleSpeed - wheelSpinVelocity[0]) / vehicleSpeed) - wheelSlip[0])/deltaTime;
  wheelSlipAcceleration[1] = (((vehicleSpeed - wheelSpinVelocity[1]) / vehicleSpeed) - wheelSlip[1])/deltaTime;
  wheelSlipAcceleration[2] = (((vehicleSpeed - wheelSpinVelocity[2]) / vehicleSpeed) - wheelSlip[2])/deltaTime;
  wheelSlipAcceleration[3] = (((vehicleSpeed - wheelSpinVelocity[3]) / vehicleSpeed) - wheelSlip[3])/deltaTime;

  //update wheel slip values
  wheelSlip[0] = (vehicleSpeed - wheelSpinVelocity[0]) / vehicleSpeed;
  wheelSlip[1] = (vehicleSpeed - wheelSpinVelocity[1]) / vehicleSpeed;
  wheelSlip[2] = (vehicleSpeed - wheelSpinVelocity[2]) / vehicleSpeed;
  wheelSlip[3] = (vehicleSpeed - wheelSpinVelocity[3]) / vehicleSpeed; 
  
  lastTimeStamp = newTimeStamp;
 
  //only activate ABS if threshold values are exceeded
  if ( vehicleSpeed > MIN_VEHICLE_VELOCITY_THRESHOLD 
     && newInputPressure > MIN_PRESSURE_THRESHOLD/MAX_BRAKE_PRESSURE ) {
    
    //activate ABS for individual wheels whos velocity thresholds are exceeded
    int i;
    for ( i = 0 ; i < 4 ; i++ )
    {
      if ( wheelSpinVelocity[i] > MIN_WHEEL_VELOCITY_THRESHOLD ) {
        //loop over control algorithm for wheel i
        phase(i, newInputPressure);
      } else {
        //if wheel velocity is not exceeded, turn off ABS for that wheel
        phaseStates[i] = OFF;
        MAX_WHEEL_SLIP[i] = INITIAL_MAX_WHEEL_SLIP;
      }
    }
  } else { //if thresholds are not met, turn off ABS
    
    //reset system variables
    phaseStates[0] = OFF;
    phaseStates[1] = OFF;
    phaseStates[2] = OFF;
    phaseStates[3] = OFF;

    MAX_WHEEL_SLIP[0] = INITIAL_MAX_WHEEL_SLIP;
    MAX_WHEEL_SLIP[1] = INITIAL_MAX_WHEEL_SLIP;
    MAX_WHEEL_SLIP[2] = INITIAL_MAX_WHEEL_SLIP;
    MAX_WHEEL_SLIP[3] = INITIAL_MAX_WHEEL_SLIP;

    lastTimeStamp = 0;

    //set output pressure to input pressure (no intervention by ABS)
    *wheelBrakeCMD[0] = newInputPressure;
    *wheelBrakeCMD[1] = newInputPressure;
    *wheelBrakeCMD[2] = newInputPressure;
    *wheelBrakeCMD[3] = newInputPressure;
  }

  return;
}

void phase(int wheel, float inputPressure) {
  
  switch(phaseStates[wheel]) {
    case OFF:
    {
      phaseStates[wheel] = 1;
      break;
    }
    case 1: //Initial application 
      { 
      //apply requisted brake pressure to all wheels     
      *wheelBrakeCMD[wheel] = inputPressure;
      
      //if wheel spin acceleration threshold is surpassed, continue to phase 2
      if (MIN_WHEEL_SPIN_ACCELERATION > wheelSpinAcceleration[wheel]) {
        phaseStates[wheel] = 2;
      }
      break;
      }
    case 2: //Maintain pressure
      {
      //pressure at wheel is set equal to current pressure 
      *wheelBrakeCMD[wheel] = *wheelBrakeCMD[wheel];

      //when the tire longitudinal slip exceeds the slip associated with the Slip Threshold, continue to phase 3
      if (wheelSlip[wheel] > MAX_WHEEL_SLIP[wheel]) {
        
        //current tire slip is stored and used as the slip threshold criterion in later phases
        MAX_WHEEL_SLIP[wheel] = wheelSlip[wheel];

        phaseStates[wheel] = 3;
      }

      break;
      }
    case 3: //Reduce pressure 
      {

      //calculate how much pressure should have been released since last ABS call (deltaTime)
      float pressureToRelease = deltaTime * RELEASE_RATE;
      //convert pressure to brake cmd equivalent
      float cmdReleasePressure = pressureToRelease/MAX_BRAKE_PRESSURE;
      //apply pressure reduction
      *wheelBrakeCMD[wheel] = *wheelBrakeCMD[wheel] - cmdReleasePressure;
      
      //if wheel acceleration exceeds the maximum wheel acceleration threshold, continue to phase 4
      if (wheelSpinAcceleration[wheel] > MAX_WHEEL_SPIN_ACCELERATION) {
        phaseStates[wheel] = 4;
      }
      break;
      }
    case 4: //Maintain pressure
      {
      //pressure at wheel is set equal to current pressure 
      *wheelBrakeCMD[wheel] = *wheelBrakeCMD[wheel];

      //use timer to stop this phase from elapsing the APPLY_DELAY time limit
      if (TIMERS[wheel] == -1) { //I.E. the timer has not been set yet
        TIMERS[wheel] = APPLY_DELAY;
      } else {
        TIMERS[wheel] -= deltaTime;
      }

      //if apply delay has elapsed, or wheel acceleration exceeds +A (10*Maximum Spin Acceleration), continue to phase 5 
      if (TIMERS[wheel] < 0 ) {
        TIMERS[wheel] = -1; //reset timer
        phaseStates[wheel] = 5; 
      } 
      if (MAX_WHEEL_SPIN_ACCELERATION * 10 < wheelSpinAcceleration[wheel]) {
        TIMERS[wheel] = -1; //reset timer
        phaseStates[wheel] = 5;
      }

      break;
      }
    case 5: //Increase pressure
      {
      
      //calculate how much pressure should have been increased since last ABS call (deltaTime)
      float pressureToApply = deltaTime * PRIMARY_APPLY_RATE;
      //convert pressure to brake cmd equivalent
      float cmdApplyPressure = pressureToApply/MAX_BRAKE_PRESSURE;
      //apply pressure increase
      *wheelBrakeCMD[wheel] = *wheelBrakeCMD[wheel] + cmdApplyPressure;

      //if wheel spin acceleration drops and again becomes negative, continue to phase 6
      if (wheelSpinAcceleration[wheel] < 0 ) {
        phaseStates[wheel] = 6;
      }
      break; 
      }
    case 6: //Maintain pressure
      {
      //pressure at wheel is set equal to current pressure
      *wheelBrakeCMD[wheel] = *wheelBrakeCMD[wheel];

      //use timer to stop this phase from elapsing the APPLY_DELAY time limit
      if (TIMERS[wheel] == -1) { //I.E. the timer has not been set yet
        TIMERS[wheel] = APPLY_DELAY;
      } else {
        TIMERS[wheel] -= deltaTime;
      }

      //if apply delay has elapsed, or wheel acceleration exceeds the minium wheel acceleration, continue to phase 7 
      if (TIMERS[wheel] < 0 ) {
        TIMERS[wheel] = -1;
        phaseStates[wheel] = 7; 
      }
      if (MIN_WHEEL_SPIN_ACCELERATION > wheelSpinAcceleration[wheel]) {
        TIMERS[wheel] = -1;
        phaseStates[wheel] = 7;
      }

      break;
      }
    case 7: //Increase pressure
      {
      //calculate how much pressure should have been increased since last ABS call (deltaTime)
      float pressureToApply = deltaTime * SECONDARY_APPLY_RATE;
      //convert pressure to brake cmd equivalent
      float cmdApplyPressure = pressureToApply/MAX_BRAKE_PRESSURE;

      //apply brake pressure increase, but not past full brake application
      if (*wheelBrakeCMD[wheel] >= 1.0f) {
        *wheelBrakeCMD[wheel] = 1.0f;
      } else {
        *wheelBrakeCMD[wheel] = *wheelBrakeCMD[wheel] + cmdApplyPressure;
      }

      //if wheel angular acceleration drops below the Wheel Minimum Angular Acceleration (negative), continue to phase 3 
      if (MIN_WHEEL_SPIN_ACCELERATION > wheelSpinAcceleration[wheel]) {
        phaseStates[wheel] = 3;
      }
    
      break; 
      }
  
    default : 
      {
        printf("ABS state error\n");
        break;
      }
  }
}