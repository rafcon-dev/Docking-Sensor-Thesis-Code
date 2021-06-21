//////////////
//BasicMoves.h
//////////////
//takes care of the basic movements of the robot

#ifndef BASICMOVES_H
#define BASICMOVES_H

#include "arduino.h"
#include "Camera.h"
#include "Communication.h"
#include "StateMachine.h"

void strafeLeft (posit& referencePosition, long amount);
void strafeRight (posit& referencePosition, long amount);
void moveForward (posit& referencePosition, long amount);
void moveBackwards (posit& referencePosition, long amount);
void rotateClockwise (posit& referencePosition, int amount);
void rotateCounterClockwise (posit& referencePosition, int amount);
void strafeRotateLeft(posit& referencePosition, long amount, int amount2 );
void strafeRotateRight(posit& referencePosition, long amount, int amount2);

void strafeBasedOnState(posit& referencePosition, long amount, EMoveBasicStates basicState);
void rotateBasedOnState (posit& referencePosition, long amount, EMoveBasicStates basicState);

void strafeRotateBasedOnState ( posit& referencePosition, long amount, int amount2, EMoveBasicStates basicState);

void invertState (EMoveBasicStates& basicState);
#endif

