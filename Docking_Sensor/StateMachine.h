#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "arduino.h"
#include "Camera.h"
#include "Communication.h"

//CONSTRUCTORS MAY NEED TWEAKING
const byte rightIsPositive = 1; //change to -1 if moving to the right is negative in the reference coordinate system
const byte forwardIsPositive = 1;
const int clockwiseIsPositive =-1;


/* These are the events that can affect our */
/* StateMachine                             */
typedef enum
{ 
  NO_TRANSITION,
  BEGIN,
  NO_BLOBS_FOUND,
  WRONG_BLOBS_FOUND,
  WEIRD_BLOBS_FOUND, 
  INITIAL_POSITION_REACHED, 
  CORRECT_BLOBS_FOUND, 
  PERPENDICULAR_POINT_REACHED, 
  NEAR_STATION,
  DOCKING_SENSORS_ARE_ON, 
  SHOULD_HAVE_DOCKED_BUT_HASNT, 
  PERPENDICULAR_REACHED_AND_TRIES_LESS_THAN_Y, 
  TOO_MANY_DOCKING_TRIES,
  DO_TRANSLATE
}
EIndication;


//errorcodes
typedef enum
{
  NO_ERROR_CODE,
  ROTATE_UNTIL_BLOBS_FOUND_TOO_MUCH_OSCILATIONS_FOCUSING,
  TWO_TURNS_WTHOUT_FINDING_CORRECT_BLOBS,
  CLARIFY_IMAGE_OSCILATIONS_GREATER_THAN_5,
  CLARIFY_IMAGE_NUMBER_OF_BLOBS_DOESNT_MAKE_SENSE,
  CLARIFY_IMAGE_BLOBS_DISAPPEARED,
  MOVE_TO_PERPENDICULAR_TOO_MANY_RESETS,
  MOVE_TO_STATION_TOO_MANY_RESETS,
  PENETRATE_STATION_TOO_MANY_DOCKING_TRIES,
  COULDNT_ALIGN_ROBOT_WITH_STATION,
  ALIGN_ROBOT_IS_STUCK
}
ErrorCodes;

//movemachine move states
typedef enum
{
  STOPPED,
  STRAFE_DANCE,
  MOVE_FORWARD_UNTIL_DIST_TO_STATION_IS_LESS_THAN,
  BACK_TO_POSITION_AT_BEGINNING_OF_STATE,
  ALIGN_WITH_BLOBS,
  CENTER_WITH_BLOBS 
}
EMoveStates;

//special states
typedef enum
{
  NO_MOVE_SPECIAL_STATES,
  TWO_TURNS_SINCE_BEGINNING_OF_STATE,
}
EMoveSpecialStates;


//basic states
typedef enum
{
  STRAFE_LEFT,
  STRAFE_RIGHT,
  MOVE_FORWARD,
  MOVE_BACKWARDS,
  ROTATE_CLOCKWISE,
  ROTATE_COUNTER_CLOCKWISE,
  STRAFE_ROTATE_LEFT,
  STRAFE_ROTATE_RIGHT
}
EMoveBasicStates;

//position of the station
typedef enum
{
  STATION_IS_TO_THE_RIGHT,
  STATION_IS_TO_THE_LEFT
}
EStationSide;


class mainStateMachine;
typedef void (mainStateMachine::*StateProc)( EIndication input); 

class moveStateMachine  ;
typedef void (moveStateMachine::*moveStateProc)(mainStateMachine& mainMachine); 

class basicMoves;
//typedef void (basicMoves::*basicMoves) ();

class basicMoves
{
  friend class mainStateMachine;
  friend class MoveStateMachine;

public:
  basicMoves();
};


class moveStateMachine
{
public:
  moveStateProc movecurrentState;

  statusRegister statusReg;

  basicMoves bMoves;

  boolean weAreMoving;

  int totalRotation;
  int previousRotation;
  int immediateRotation;

  int alignPreviowsXDist;
  byte alignOscilations;
  boolean positionReached;
  byte firstCycle1;
  byte firstCycle2;
  void assignMoveStateBasedOnEnumeration(EMoveStates enumeratedMoveState);
  moveStateMachine();
  void calculateRotation();

  void halfReached(mainStateMachine& mainMachine);
  void stopped(mainStateMachine& mainMachine);

  void strafeDance(mainStateMachine& mainMachine);

  void moveForwardUntilDistToStationIsLessThan(mainStateMachine& mainMachine);
  void backToPositionAtBeginningOfState(mainStateMachine& mainMachine);
  void alignWithBlobs(mainStateMachine& mainMachine);
  void centerWithBlobs(mainStateMachine& mainMachine);
  void rotateOverOwnAxis(mainStateMachine& mainMachine);
};


class mainStateMachine
{
public:
  posit beginningOfStatePosition;
  posit currentPosition;
  posit referencePosition;
  posit wrongBlobsPosition;
  posit beginningOfDockingPosition;
  maxVelocAcel velocAcelSettings;

  basicMoves bMoves;

  statusRegister statusReg;

  byte rotateUntilBlobsFoundOscilations;
  StateProc currentState;
  blobsInfo blobs;
  EIndication transition;
  EMoveSpecialStates specialStates;
  EMoveBasicStates basicState;
  EMoveBasicStates strafeSearchState;
  ErrorCodes errorCode;
  EStationSide sideTheStationIsIn;
  EMoveStates nowMoveState;
  boolean dockingSensorsAreOn;
  boolean weHaveWrongBlobsToMark;

  byte maxVelocity;
  long strafeAmount;
  long forwardBackAmount;
  int rotateAmount;
  int threshold; 
  unsigned long timeStart;
  unsigned long timeNow;
  byte stateResetCounter; 
  byte clarifyBlobsStrafeCounter;
  byte clarifiyImageOscilations;
  boolean clarifyImageWeFoundTheDirection;
  byte dockingTriesCounter;

  byte nBlobsPrevious;
  mainStateMachine();

  boolean rotateUntilBlobsFoundWeStartedRotation;
  int rotateUntilBlobsFoundRotAtBeginning;

  void startMachine (EIndication input);
  void rotateUntilBlobsFound    ( EIndication input); 
  void clarifyImage             (EIndication input); 
  void backToInitialPosition    (EIndication input); 
  void findOtherBlobs           (EIndication input);  
  void moveToPerpendicular      (EIndication input);  
  void moveToStation            (EIndication input); 
  void penetrateStation         (EIndication input);
  void backToPerpendicularPoint (EIndication input);  
  void errorState               (EIndication input);
  void weAreDocked              (EIndication input);
};


#endif




