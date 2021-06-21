#include "Includes.h"
#include <PVision.h>
#include <Wire.h>
#include "CommonFunctions.h"
#include "Communication.h"
#include "Camera.h"
#include "StateMachine.h"
#include "Defines.h"
#include "Defines.h"

mainStateMachine mainMachine;
moveStateMachine moveMachine;


void setup()
{
  Serial.begin(115200);    //at 115200 bps
}


void loop()
{ 
  mainMachine.blobs.updateValues();

  (mainMachine.*(mainMachine.currentState))(mainMachine.transition);
  mainMachine.transition = NO_TRANSITION;
  moveMachine.assignMoveStateBasedOnEnumeration(mainMachine.nowMoveState);

  (moveMachine.*(moveMachine.movecurrentState))(mainMachine);

  //if we quit with error, reset arduino
}

