//Has all the methods that are needed to communicate with the Budapest Robot

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "arduino.h"
#include "CommonFunctions.h"

//Addresses of each of the robot's registers
#define ADDR_STATUS          0x00		
#define ADDR_CONFIG          0x01		
#define ADDR_CUR_POS         0x02		
#define ADDR_REF_POS         0x03		
#define ADDR_LIMITS          0x04		
#define ADDR_SHARP           0x05		
#define ADDR_RELAY           0x06		
#define ADDR_REQ             0x07		
#define ADDR_BLUEBOTS_VEL    125		
#define ADDR_BLUEBOTS_BUTTON 127	


//Robot's position register
typedef enum
{
  POSITION_REACHED,
  POSITION_NEAR,
  HALF_REACHED,
  ERROR_OVER_50
}
PositionState;

//Robot's speed limit presets register
typedef enum
{
  NO_SPEED_LIM,
  SPEED_LIM_75_99,
  SPEED_LIM_50_75,
  SPEED_LIM_0_50
}
DistanceSensorState;

//Has the methods that pertain the communication with the robot
class robotComm
{
public:
  boolean askAndReceiveValues(byte);
  virtual void setValuesAskFunc(byte[]);  //so that it can be invoked in the function askAndReceiveValues
  virtual byte customSizeOf();  //so that it can be invoked in the function askAndReceiveValues
};

//For Commmunication with the statusRegister
class statusRegister : 
public robotComm
{
public:
  static const byte addr=ADDR_STATUS;
  byte stat;
  byte acknAndID;

  statusRegister();

  PositionState positionState();
  DistanceSensorState distanceSensorState();

  byte checksum();
  void setValuesAskFunc(byte[]);
  byte customSizeOf();
  boolean receiveValues();
};


//For Communication with the Bluebots Velocity register
class referenceVelocity : 
public robotComm
{
public:
  static const byte addr=ADDR_BLUEBOTS_VEL;
  byte xVeloc;
  byte yVeloc;
  byte angVeloc;
  byte maxVeloc;

  referenceVelocity(byte, byte, byte, byte);
  void setValues (byte, byte, byte, byte);
  byte checksum();
  void sendValues ();
};

//For Communication with the position register
class posit : 
public robotComm
{
public:
  byte addr;
  long xPos;
  long yPos;
  int direc;

  void setValues (long, long, int);
  byte checksum();
  void sendValues ();
  void setValuesAskFunc(byte[]);
  byte customSizeOf(); 
};


//For communcation with the max velocity and aceleration registers
class maxVelocAcel : 
public robotComm
{
public:
  byte addr;
  byte maxVeloc;
  byte maxAngVeloc;
  ;
  byte maxAcel;
  byte maxAngAcel;

  void setValues (byte, byte, byte, byte);
  byte checksum();
  void sendValues ();
  void setValuesAskFunc(byte[]);
  byte customSizeOf(); 
};

#endif





