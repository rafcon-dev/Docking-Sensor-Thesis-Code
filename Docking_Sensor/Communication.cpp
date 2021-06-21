
#include "Communication.h"

void robotComm::setValuesAskFunc(byte a[])
{
}

byte robotComm::customSizeOf()
{
}


statusRegister::statusRegister()
{
  stat=B11111111;
  acknAndID=0;
}


//Takes a positionstate input and returns the proper enumerated type
PositionState statusRegister::positionState()
{
  byte result=stat & B00110000;
  switch(result)
  {
  case B00000000: 
    return POSITION_REACHED;
  case B00010000: 
    return POSITION_NEAR;
  case B00100000: 
    return HALF_REACHED;
  default: 
    return ERROR_OVER_50;
  }
}

//Takes a distancesensorstate input and returns the proper enumerated type
DistanceSensorState statusRegister::distanceSensorState()
{
  byte result=stat & B00001100;
  switch(result)
  {
  case B00000000: 
    return NO_SPEED_LIM;
  case B00000100: 
    return SPEED_LIM_75_99;
  case B00001000: 
    return SPEED_LIM_50_75;
  default: 
    return SPEED_LIM_0_50;
  }
}


//function to set the values that is invoked when the arduino asks for values
void statusRegister::setValuesAskFunc(byte array[sizeof(statusRegister)]) 
{
  stat=array[1];
  acknAndID=array[2];
}


//returns the classe's own size
byte statusRegister::customSizeOf()
{
  return (byte)sizeof(statusRegister);
}

//returns the checksum pertaining the statusregister
byte statusRegister::checksum()
{
  return ((addr + stat+acknAndID) % 256);
}


//receives values pertaining to the statusRegister
boolean statusRegister::receiveValues()
{
  byte i=0, tries=0;
  int checksum=0;
  byte buffer[3+1];

  for(i=0;i<=3;i++)
    buffer[i]=0;

  do
  { 
    checksum=0;

    //clear incoming buffer
    clearInBuffer();   

    //Data request
    delay(50);
    if (Serial.available())
    {
      //time to receive
      i=0;

      while(Serial.available() && i<=3)
      {
        buffer[i]=Serial.read();  
        checksum+=buffer[i++];

      }

      checksum=(checksum-buffer[3]) % 256;  //DISRESGARD THE RECEIVED checksum
    }
    tries++;
  }
  while(( (checksum!=buffer[3]) ||  buffer[0]!=addr )&& tries < 4 ); ////NEEDS TESTING

  //tries<4 to see if we couldn't receive any data, which would get the checksum equal anyway
  if (checksum == buffer[3] && tries<4)  
  {
    setValuesAskFunc(buffer);
    return true;   
  }   
  else {  
    return false; 
  }
}

//sets the values of the referenceVelocity Class, does not send them
void referenceVelocity::setValues( byte a, byte b, byte c, byte d)
{
  xVeloc=a;
  yVeloc=b;
  angVeloc=c;
  maxVeloc=d;
}

//returns the referenceVelocity values checksum
byte referenceVelocity::checksum()
{ 
  return ((addr+xVeloc+yVeloc+angVeloc+maxVeloc) % 256);
}

//sends the referenceVelocity values
void referenceVelocity::sendValues()
{
  Serial.write(ADDR_BLUEBOTS_VEL);
  Serial.write((byte)xVeloc);
  Serial.write((byte)yVeloc);
  Serial.write((byte)angVeloc);
  Serial.write((byte)maxVeloc);
  Serial.write(referenceVelocity::checksum()); //checksum
}

//sets the values of the posit classs
void posit::setValues(long b, long c, int d)
{
  xPos=b;
  yPos=c;
  direc=d;
}

//returns the size of the posit class
byte posit::customSizeOf()
{
  return (byte)sizeof(posit);
}

//function to set the values that is invoked when the arduino asks for values
void posit::setValuesAskFunc(byte array[sizeof(posit)-sizeof(robotComm)])
{
  posit::setValues
    (    
  joinFourBytesLong(array[1], array[2], array[3], array[4]),
  joinFourBytesLong(array[5], array[6], array[7], array[8]),
  joinTwoBytesInt  (array[9], array[10])
    );
}

//returns the checksum of the posit class values
byte posit::checksum()
{
  int sum;
  //summing each individual byte
  sum=addr;
  sum+=(byte)(xPos>>24);
  sum+=(byte)(xPos>>16);
  sum+=(byte)(xPos>>8);
  sum+=(byte)(xPos);

  sum+=(byte)(yPos>>24);
  sum+=(byte)(yPos>>16);
  sum+=(byte)(yPos>>8);
  sum+=(byte)(yPos);

  sum+=(byte)(direc>>8);
  sum+=(byte)(direc);

  sum%=256;
  return (byte)sum;
}

//sends the values of the posit class
void posit::sendValues ()
{
  Serial.write(addr);

  //gota make bitwise moves to the left to the send the individual bytes of a variable
  Serial.write((byte)(xPos>>24));
  Serial.write((byte)(xPos>>16));
  Serial.write((byte)(xPos>>8));
  Serial.write((byte)xPos);

  Serial.write((byte)(yPos>>24));
  Serial.write((byte)(yPos>>16));
  Serial.write((byte)(yPos>>8));
  Serial.write((byte)yPos);

  Serial.write((byte)(direc>>8));
  Serial.write((byte)direc);

  Serial.write(posit::checksum()); //checksum
}

//sets the values of the maxVelocAcel class
void maxVelocAcel::setValues(byte a, byte b, byte c, byte d)
{
  maxVeloc = a;
  maxAngVeloc = b;
  maxAcel = c;
  maxAngAcel = d;
}

//returns the size of the maxVelocAcel class
byte maxVelocAcel::customSizeOf()
{
  return (byte)sizeof(maxVelocAcel);
}

//function to set the values that is invoked when the arduino asks for values
void maxVelocAcel::setValuesAskFunc(byte array[sizeof(posit)-sizeof(robotComm)])
{
  maxVelocAcel::setValues
    (    
  array[1] , 
  array[2], 
  array[3], 
  array[4]
    );
}

//returns the checksum of the maxVelocAcel class
byte maxVelocAcel::checksum()
{
  int sum;
  //summing each individual byte
  sum=addr;
  sum =  maxVeloc + maxAngVeloc + maxAcel + maxAngAcel;
  sum%=256;
  return (byte)sum;
}

//sends the values pertaining to the maxVelocAcel class
void maxVelocAcel::sendValues ()
{
  Serial.write(addr);

  Serial.write((byte)maxVeloc);
  Serial.write((byte)maxAngVeloc);
  Serial.write((byte)maxAcel);
  Serial.write((byte)maxAngAcel);

  Serial.write(maxVelocAcel::checksum()); //checksum
}

//function to be used by all this classe, to ask and receive values from the robot
boolean robotComm::askAndReceiveValues(byte address)
{
  byte i=0, tries=0;
  int checksum=0;
  byte buffer[customSizeOf()-sizeof(robotComm)+1];

  for(i=0;i<=customSizeOf();i++)
    buffer[i]=0;

  do
  { 
    checksum=0;

    //clear incoming buffer
    clearInBuffer();   

    //Data request
    Serial.write(ADDR_REQ); //requesting data
    Serial.write(address);  //address of the data to request
    Serial.write( (byte) ((ADDR_REQ + address)%256)); //checksum
    //

    delay(50); ///is there other way?
    if (Serial.available())
    {
      /////More than enough time to receive data
      i=0;

      while(Serial.available() && i<=customSizeOf()-sizeof(robotComm))
      {
        buffer[i]=Serial.read();
        checksum+=buffer[i++];
      }

      checksum=(checksum-buffer[customSizeOf()-sizeof(robotComm)]) % 256;  //DISRESGARD THE RECEIVED checksum
    }
    tries++;
  }
  while(((checksum!=buffer[customSizeOf()-sizeof(robotComm)]) ||  buffer[0]!=address) && tries < 4 );//needs testing

  //tries<4 to see if we couldn't receive any data, which would get the checksum equal anyway
  if (checksum == buffer[customSizeOf()-sizeof(robotComm)] && tries<4)  
  {
    setValuesAskFunc(buffer);
    return true;   
  }   
  else 
  {  
    return false; 
  }
}

