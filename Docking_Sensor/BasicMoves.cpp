#include "BasicMoves.h"

//move the robot to its left by a certain amount, in mm
void strafeLeft (posit& referencePosition, long amount)
{
  referencePosition.setValues (rightIsPositive*amount, 0,0);
  referencePosition.sendValues();
}


//move the robot to its right by a certain amount, in mm
void strafeRight (posit& referencePosition, long amount)
{
  referencePosition.setValues ((-1)*rightIsPositive*amount, 0,0);
  referencePosition.sendValues();
}


//move the robot forward by a certain amount, in mm
void moveForward (posit& referencePosition, long amount)
{
  referencePosition.setValues (0, forwardIsPositive*amount,0);
  referencePosition.sendValues();
}


//move the robot backwards by a certain amount, in mm
void moveBackwards (posit& referencePosition, long amount)
{
  referencePosition.setValues (0, (-1) * forwardIsPositive*amount,0);
  referencePosition.sendValues();
}


//rotates the robot clockwise, by a certain amount, in degrees
void rotateClockwise (posit& referencePosition, int amount)
{
  referencePosition.setValues (0, 0,clockwiseIsPositive*amount);
  referencePosition.sendValues();
}


//rotates the robot counterclockwise, by a certain amount, in degrees
void rotateCounterClockwise (posit& referencePosition, int amount)
{
  referencePosition.setValues (0, 0,(-1) * clockwiseIsPositive*amount);
  referencePosition.sendValues();
}


//moves the robot to the left, by amount in mm, while rotating it clockwise by amount2, in degrees
void strafeRotateLeft(posit& referencePosition, long amount, int amount2 )
{
  referencePosition.setValues (0, (-1) * clockwiseIsPositive*amount,clockwiseIsPositive*amount2);
  referencePosition.sendValues(); 
}


//moves the robot to the right, by amount in mm, while rotating it counterclokwise by amount2, in degrees
void strafeRotateRight(posit& referencePosition, long amount, int amount2 )
{
  referencePosition.setValues (0, (-1) * clockwiseIsPositive*amount,(-1) * clockwiseIsPositive*amount2);
  referencePosition.sendValues();
}


//given a move state (basicState), move to the right or to the left, by amount, in mm
void strafeBasedOnState(posit& referencePosition, long amount, EMoveBasicStates basicState)
{
  if (basicState==STRAFE_LEFT)
    strafeLeft (referencePosition, amount);
  if (basicState ==STRAFE_RIGHT)
    strafeRight (referencePosition, amount);
}


//given a move state, rotate clockwise or anticlockwise, by amount, in degrees
void rotateBasedOnState (posit& referencePosition, long amount, EMoveBasicStates basicState)
{
  if (basicState == ROTATE_CLOCKWISE)
    rotateClockwise (referencePosition, amount);
  if (basicState ==ROTATE_COUNTER_CLOCKWISE)
    rotateCounterClockwise (referencePosition, amount);
}


//given a move state, strafe rotate to the left or to the right, by amount in mm and amound2 in degrees
void strafeRotateBasedOnState ( posit& referencePosition, long amount, int amount2, EMoveBasicStates basicState)
{
  if (basicState == STRAFE_ROTATE_LEFT)
    strafeRotateLeft (referencePosition, amount, amount2);
  if (basicState == STRAFE_ROTATE_RIGHT)
    strafeRotateRight (referencePosition, amount, amount2);
}


//given a move state, change that state to the corresponding oposite
void invertState (EMoveBasicStates& basicState)
{
  switch (basicState)
  {
  case STRAFE_LEFT:
    basicState = STRAFE_RIGHT;
    return;
  case STRAFE_RIGHT:
    basicState = STRAFE_LEFT;
    return;

  case ROTATE_CLOCKWISE:
    basicState = ROTATE_COUNTER_CLOCKWISE;
    return;
  case ROTATE_COUNTER_CLOCKWISE:
    basicState = ROTATE_CLOCKWISE;
    return;

  case MOVE_FORWARD:
    basicState = MOVE_BACKWARDS;
    return;
  case MOVE_BACKWARDS:
    basicState = MOVE_FORWARD;
    return;

  case STRAFE_ROTATE_LEFT:
    basicState = STRAFE_ROTATE_RIGHT;
    return;
  case STRAFE_ROTATE_RIGHT:
    basicState = STRAFE_ROTATE_LEFT;
    return;
  }
}
