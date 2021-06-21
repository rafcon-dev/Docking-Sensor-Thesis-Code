//Has some functions that many other functions use

#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include "arduino.h"

void clearInBuffer();

long joinFourBytesLong(byte msb, byte byte2, byte byte3, byte lsb);

int joinTwoBytesInt(byte msb, byte lsb);

boolean issim(int a, int b, int range);

#endif

