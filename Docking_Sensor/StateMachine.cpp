#include "StateMachine.h"
#include "basicMoves.h"

//Constructors of the classes probably need some work
mainStateMachine::mainStateMachine()
{
  currentPosition.addr=ADDR_CUR_POS;
  referencePosition.addr=ADDR_REF_POS;
  beginningOfStatePosition.addr = ADDR_CUR_POS;

  rotateUntilBlobsFoundOscilations=0;
  currentState=&mainStateMachine::startMachine;
  transition=BEGIN;
  EMoveSpecialStates specialStates=NO_MOVE_SPECIAL_STATES;

  ErrorCodes errorCode=NO_ERROR_CODE;

  nowMoveState=STOPPED;
  dockingSensorsAreOn=false;

  maxVelocity=10; //80 mm per second
  strafeAmount = 100; //100 milimeters
  forwardBackAmount = 100; //100 milimeters
  rotateAmount = 5; //ammount of degrees to rotate on each rotational step
  threshold = 30; //Pixel number threshold for comparing horizontal pixel coordinates

  stateResetCounter = 0; 
  clarifiyImageOscilations = 0;
  clarifyBlobsStrafeCounter = 0;

  strafeSearchState = STRAFE_ROTATE_LEFT;

  basicState = ROTATE_COUNTER_CLOCKWISE;
  sideTheStationIsIn = STATION_IS_TO_THE_RIGHT;
  clarifyImageWeFoundTheDirection = false;

  nBlobsPrevious = 0;

  rotateUntilBlobsFoundWeStartedRotation = false;

  dockingTriesCounter=0;
}


moveStateMachine::moveStateMachine()
{
  movecurrentState=&moveStateMachine::stopped;

  weAreMoving=false;

  totalRotation = 0; // sempre que haja erro ou se mude de estado seriamente
  previousRotation = 0;
  immediateRotation = 0;

  alignPreviowsXDist=false;
  alignOscilations=false;
  positionReached=true; ////////////////////////////////////////////////////////////////////////////////////////////////////
  firstCycle1=0;
  firstCycle2=0;
}


void mainStateMachine::startMachine (EIndication input)
{
  if (input==BEGIN)
  {
    beginningOfStatePosition.askAndReceiveValues(ADDR_CUR_POS);
    velocAcelSettings.askAndReceiveValues(ADDR_LIMITS);
    currentState=&mainStateMachine::rotateUntilBlobsFound;
  }
}


void mainStateMachine::rotateUntilBlobsFound(EIndication input)
{
  switch(input)
  {
  case NO_BLOBS_FOUND:
    beginningOfStatePosition.askAndReceiveValues(ADDR_CUR_POS);
    currentState=&mainStateMachine::errorState;
    return;

  case WRONG_BLOBS_FOUND:
    beginningOfStatePosition.askAndReceiveValues(ADDR_CUR_POS);
    currentState=&mainStateMachine::findOtherBlobs;
    return;

  case WEIRD_BLOBS_FOUND:
    beginningOfStatePosition.askAndReceiveValues(ADDR_CUR_POS);

    wrongBlobsPosition.xPos = beginningOfStatePosition.xPos;
    wrongBlobsPosition.yPos = beginningOfStatePosition.yPos;
    wrongBlobsPosition.direc= beginningOfStatePosition.direc;

    clarifiyImageOscilations=0;
    currentState=&mainStateMachine::clarifyImage;
    return;

  case CORRECT_BLOBS_FOUND:
    beginningOfStatePosition.askAndReceiveValues(ADDR_CUR_POS);
    currentState=&mainStateMachine::moveToPerpendicular;
    return;
  }  

  if(specialStates==TWO_TURNS_SINCE_BEGINNING_OF_STATE) //two turns since beginning of this state
  {
    currentState=&mainStateMachine::errorState;
    errorCode=TWO_TURNS_WTHOUT_FINDING_CORRECT_BLOBS;
    return;
    // currentMovement.totalRotation=0;////////////////////////////////////////////////////////////////////////////////////7É PRECISO FAZER RESET AO MEDIDOR DA ROTAÇÃO!!!
  }

  rotateUntilBlobsFoundWeStartedRotation=true;

  //clear unusable blobs from vision
  if(blobs.nUnusable()>0)
  {
    rotateBasedOnState(referencePosition, rotateAmount, basicState);
    do
    {
      statusReg.receiveValues();
      delay(1);
    }
    while (statusReg.positionState()!=POSITION_REACHED);

    return;
  }

  if (blobs.nBlobs<1)
  {   
    statusReg.receiveValues();

    basicState=ROTATE_CLOCKWISE;
    rotateBasedOnState (referencePosition, rotateAmount, basicState);
    return;
  }//////////////////////////////////////////////////////////////////////////

  if(!blobs.blaIsCentered())
  {
    nowMoveState = CENTER_WITH_BLOBS;
    return;
  }

  if (blobs.averageXCoordinate() < (X_RES/2 - threshold))
  {
    basicState=ROTATE_COUNTER_CLOCKWISE;
    return;
  }

  if (blobs.averageXCoordinate() > (X_RES/2 + threshold))
  {
    rotateUntilBlobsFoundOscilations++;
    if (rotateUntilBlobsFoundOscilations > 8) 
    {
      currentState=&mainStateMachine::errorState;
      errorCode=ROTATE_UNTIL_BLOBS_FOUND_TOO_MUCH_OSCILATIONS_FOCUSING;
      return;
    }  
    // rotateClockwise(referencePosition, rotateAmount);
    rotateAmount=(int)(0.8* (float)rotateAmount); /////////////////////////////////////fazer reset aos rotateAmounts
    basicState=ROTATE_CLOCKWISE;
    rotateBasedOnState (referencePosition, rotateAmount, basicState);
    return;  
  }

  rotateUntilBlobsFoundOscilations=0;

  if(!(blobs.validBlobs()))
  {
    transition = WEIRD_BLOBS_FOUND;
    return;
  }

  if(blobs.correctConf())
  {
    transition = CORRECT_BLOBS_FOUND;
    return;
  }

  //mark the three blobs as useless
  blobs.markBlaBlbBlcAsUnusable();
}


void mainStateMachine::clarifyImage (EIndication input)
{
  switch (input)
  {
  case WRONG_BLOBS_FOUND:
    currentState = &mainStateMachine::backToInitialPosition;
    return;
  case CORRECT_BLOBS_FOUND:
    beginningOfStatePosition.askAndReceiveValues(ADDR_CUR_POS);
    currentState=&mainStateMachine::moveToPerpendicular;
    return;
  }

  if(blobs.validBlobs())
  {
    if(blobs.correctConf())
    {
      transition = CORRECT_BLOBS_FOUND;
      return;
    }
    else
    {
      transition = WRONG_BLOBS_FOUND;
      blobs.markBlaBlbBlcAsUnusable();
      return;
    }
  }

  if(clarifiyImageOscilations>5)
  {
    currentState=&mainStateMachine::errorState;
    errorCode=CLARIFY_IMAGE_OSCILATIONS_GREATER_THAN_5;
    return;
  }

  if(blobs.nBlobs < nBlobsPrevious)
  {
    clarifyImageWeFoundTheDirection = true;
    invertState(strafeSearchState);
    clarifiyImageOscilations++;
  }

  if(blobs.nBlobs > nBlobsPrevious)
  {
    clarifyImageWeFoundTheDirection = true; ////////////////////////////////
  }

  nBlobsPrevious = blobs.nBlobs;

  if(blobs.nBlobs >0)
  {
    if(!blobs.blaIsCentered())
    {
      nowMoveState = ALIGN_WITH_BLOBS;
      return;
    }
  }

  if(blobs.nBlobs >2)
  { //we are seeing reflection because if this wasnt a reflection, the blobs would be valid
    moveForward(referencePosition, forwardBackAmount);
    return;
  }

  strafeRotateBasedOnState (referencePosition, strafeAmount, rotateAmount, strafeSearchState);
}


void mainStateMachine::backToInitialPosition (EIndication input)
{
  switch (input)
  {
  case INITIAL_POSITION_REACHED:
    currentState = &mainStateMachine::rotateUntilBlobsFound;
    return;
  }
  nowMoveState = BACK_TO_POSITION_AT_BEGINNING_OF_STATE;
}


void mainStateMachine::findOtherBlobs (  EIndication input)
{
  switch (input)
  {
  case NO_BLOBS_FOUND:
    currentState=&mainStateMachine::errorState;
    return;

  case WRONG_BLOBS_FOUND:
    currentState=&mainStateMachine::findOtherBlobs; //////////////////////////////////////////////////////////////NOT NEEDED????????????????????
    return;

  case WEIRD_BLOBS_FOUND:
    currentState=&mainStateMachine::clarifyImage;
    return;

  case CORRECT_BLOBS_FOUND:
    currentState=&mainStateMachine::moveToPerpendicular;
    return;
  }
}


void mainStateMachine::moveToPerpendicular(  EIndication input)
{
  switch (input)
  {
  case PERPENDICULAR_POINT_REACHED:
    currentState=&mainStateMachine::moveToStation;
    beginningOfStatePosition.askAndReceiveValues(ADDR_CUR_POS);
    return;
  }

  if(stateResetCounter >5)
  {
    currentState=&mainStateMachine::errorState;
    errorCode=MOVE_TO_PERPENDICULAR_TOO_MANY_RESETS;
    return;
  }

  if (!blobs.validBlobs() || !blobs.correctConf()) 
  {
    nowMoveState= BACK_TO_POSITION_AT_BEGINNING_OF_STATE;
    stateResetCounter++;
    return;
  }

  if(blobs.distToStation<perpendicularPointDist)
  {
    if(!blobs.isAligned())
    {
      nowMoveState = ALIGN_WITH_BLOBS;
      return;
    }

    if(!blobs.blaIsCentered())
    {
      nowMoveState = CENTER_WITH_BLOBS;
      return;
    }

    transition=PERPENDICULAR_POINT_REACHED;
    return;
  }

  moveForward(referencePosition, forwardBackAmount);
}


void mainStateMachine::moveToStation(EIndication input)
{
  switch (input)
  {
  case NEAR_STATION:
    currentState=&mainStateMachine::penetrateStation;
    timeStart=millis(); //start counting time from the start of the penetrateStation state
    return;
  }

  if(stateResetCounter > 5)
  {
    currentState=&mainStateMachine::errorState;
    errorCode=MOVE_TO_STATION_TOO_MANY_RESETS;
    return;
  }

  if (!blobs.validBlobs() || !blobs.correctConf())  
  {
    nowMoveState = BACK_TO_POSITION_AT_BEGINNING_OF_STATE;
    stateResetCounter++;
    return;
  }

  if(!blobs.isAligned())
  {
    nowMoveState = ALIGN_WITH_BLOBS;
    return;
  }

  if(!blobs.blaIsCentered())
  {
    nowMoveState = CENTER_WITH_BLOBS;
    return;
  }

  moveForward(referencePosition, forwardBackAmount);
}


void mainStateMachine::penetrateStation (EIndication input)
{
  switch(input)
  {
  case DOCKING_SENSORS_ARE_ON:
    currentState=&mainStateMachine::weAreDocked;
    return;

  case SHOULD_HAVE_DOCKED_BUT_HASNT:
    currentState=&mainStateMachine::moveToPerpendicular;
    dockingTriesCounter++;
    return;
  }


  if(dockingSensorsAreOn)
  {
    transition=DOCKING_SENSORS_ARE_ON;
    return;
  }

  if(dockingTriesCounter > 2)
  {
    currentState=&mainStateMachine::errorState;
    errorCode=PENETRATE_STATION_TOO_MANY_DOCKING_TRIES;
    return;
  }

  timeNow=millis();
  if ((timeNow-timeStart) > 10000) //MORE THAN TEN SECONDS HAVE Passed
  {
    transition=SHOULD_HAVE_DOCKED_BUT_HASNT;
    return;
  }
  if(!blobs.blaIsCentered())
  {
    nowMoveState = CENTER_WITH_BLOBS;
    return;
  }
  if(timeNow - timeStart > 2000)
  {
    velocAcelSettings.setValues (
    5, velocAcelSettings.maxAngVeloc, 
    velocAcelSettings.maxAcel, velocAcelSettings.maxAngAcel
      );

    velocAcelSettings.sendValues();
  }
  moveForward(referencePosition, forwardBackAmount);
}


void mainStateMachine::errorState (  EIndication input)
{
  //SEND ERROR STATE TO CENTRAL COMPUTER
  return;
}


void mainStateMachine::weAreDocked (  EIndication input)
{
  //reset Arduino, cut of its power?
  return;
}

///////////////////////////////////////////
///Basic Moves state machine starts now:///
///////////////////////////////////////////

basicMoves::basicMoves()
{
}


void moveStateMachine::calculateRotation()
{
  int displacement;
  displacement =totalRotation - immediateRotation; //in which totalRotation should be equal to the previows rotation
  if(previousRotation > 180 && immediateRotation < 180 )
  {
    totalRotation=totalRotation + (360-(previousRotation-immediateRotation));
  }

  else if (previousRotation < 180 && immediateRotation >180)
  {
    totalRotation = totalRotation - (previousRotation-360+immediateRotation);
  }
  else
  {
    totalRotation=totalRotation + (previousRotation-immediateRotation);
  }
}


void moveStateMachine::assignMoveStateBasedOnEnumeration(EMoveStates enumeratedMoveState)
{
  switch(enumeratedMoveState)
  {
  case     STOPPED:
    movecurrentState=&moveStateMachine::stopped;
    return;
  case BACK_TO_POSITION_AT_BEGINNING_OF_STATE:
    movecurrentState = &moveStateMachine::backToPositionAtBeginningOfState;
    return;
  }
}


void moveStateMachine::stopped(mainStateMachine& mainMachine)
{
  return;
}

void moveStateMachine::moveForwardUntilDistToStationIsLessThan(mainStateMachine& mainMachine)
{
  if(mainMachine.blobs.distToStation > 200) ///////////////////////////////////////////////////////////////////////////////////////7
  {
    if(statusReg.positionState()==HALF_REACHED || statusReg.positionState() == POSITION_REACHED)
    {
      moveForward(mainMachine.referencePosition, mainMachine.forwardBackAmount);
    }
  }
}


void moveStateMachine::backToPositionAtBeginningOfState(mainStateMachine& mainMachine)
{
  long xOffset;
  long yOffset;
  int rotOffset;
  mainMachine.currentPosition.askAndReceiveValues(mainMachine.currentPosition.addr); //which position are we in???

  xOffset   = (rightIsPositive) * (mainMachine.beginningOfStatePosition.xPos - mainMachine.currentPosition.xPos);
  yOffset   = (forwardIsPositive) * (mainMachine.beginningOfStatePosition.yPos - mainMachine.currentPosition.yPos);
  rotOffset = (clockwiseIsPositive) * (mainMachine.beginningOfStatePosition.direc - mainMachine.currentPosition.direc);

  mainMachine.referencePosition.setValues(xOffset, yOffset, rotOffset);
  mainMachine.referencePosition.sendValues();


  do
  {
    statusReg.receiveValues();
  } 
  while (statusReg.positionState() != POSITION_REACHED);

  mainMachine.blobs.markBlaBlbBlcAsUnusable();
  mainMachine.nowMoveState = STOPPED;
  mainMachine.transition=INITIAL_POSITION_REACHED;
}


void moveStateMachine::alignWithBlobs(mainStateMachine& mainMachine)
{
  if(alignOscilations >4)
  {
    if(mainMachine.blobs.distToStation > 0.5)
    {
      moveForward(mainMachine.referencePosition, mainMachine.forwardBackAmount);
      return;
    }
    else
    {
      mainMachine.currentState=&mainStateMachine::errorState;
      mainMachine.errorCode=COULDNT_ALIGN_ROBOT_WITH_STATION;
      return;
    }
  }

  if(!mainMachine.blobs.validBlobs())
  {
    weAreMoving=false;
    return;
  }

  if(mainMachine.blobs.isAligned())
  {
    movecurrentState=&moveStateMachine::stopped;
    return;
  }

  if(!mainMachine.blobs.blaIsCentered())
  {
    mainMachine.nowMoveState = CENTER_WITH_BLOBS;
    return;
  }

  alignPreviowsXDist=mainMachine.blobs.horizontalDistance();

  strafeLeft(mainMachine.referencePosition, mainMachine.strafeAmount); 

  if(mainMachine.blobs.horizontalDistance()>alignPreviowsXDist)
    return;
  if(!mainMachine.blobs.horizontalDistance()<alignPreviowsXDist) //robot is stuck
  {
    mainMachine.currentState=&mainStateMachine::errorState;
    mainMachine.errorCode = ALIGN_ROBOT_IS_STUCK;
    return;
  }
}


void moveStateMachine::centerWithBlobs(mainStateMachine& mainMachine)
{
  if( !mainMachine.blobs.validBlobs())
  {
    weAreMoving=false;
    return;
  }
  if(mainMachine.blobs.blaIsCentered())
  {
    weAreMoving=false;
    return; 
  }
  if(statusReg.positionState()==HALF_REACHED || statusReg.positionState() == POSITION_REACHED)
  {
    if(mainMachine.blobs.blobs[mainMachine.blobs.bla].X > X_RES /2) //bla is to the right of the center of the screen
    {
      rotateClockwise(mainMachine.referencePosition, mainMachine.rotateAmount);
    }
    else
    {
      rotateCounterClockwise(mainMachine.referencePosition, mainMachine.rotateAmount);
    }
  }
}



