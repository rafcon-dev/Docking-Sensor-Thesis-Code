#include "CommonFunctions.h"

//Clears the serial input buffer
void clearInBuffer()
{
  while(Serial.available())
    Serial.read();
}


//joins four individual bytes and returns a long
long joinFourBytesLong(byte msb, byte byte2, byte byte3, byte lsb)
{
  long result=0;

  result=msb;
  result=result<<8;
  result|=byte2;
  result=result<<8;
  result|=byte3;
  result=result<<8;
  result|=lsb;

  return result;
}


//joins two bytes and returns an int
int joinTwoBytesInt(byte msb, byte lsb)
{
  long result=0;

  result=msb;
  result=result<<8;
  result|=lsb;

  return result;
}


//compares a and b and returns true if their diference is less or equal than range
boolean issim(int a, int b, int range)
{
  int res;
  res=a-b;
  return(abs(res)<=range);
}
