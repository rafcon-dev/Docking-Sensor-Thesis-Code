//////////
//Camera.h
//////////
//Has all the methods relative to blob identification and their calculations

#ifndef CAMERA_H
#define CAMERA_H

#include "Arduino.h"
#include <PVision.h>

//horizontal resolution of the wiimote camera
const int X_RES=1024; 
//vertical resolution of the wiimote camera
const int Y_RES=768; 

//Horizontal field of view of the camera, in radians
const float FOV_X=44.5*PI/180.0; 
//Vertical field of view of the camera, in radians
const float FOV_Y=35*PI/180.0;  

//Horizontal distance between the LEDs of the docking station
const float SENS_X=0.081; 
//Vertical distance between the LEDs of the docking station
const float SENS_Y=0.081; 

//Maximum number of blobs the camera supports
const byte WIIMOTE_MAX_BLOBS=4; 

//Threshold that the camera considers the blobs at center. Might need tuning.
const int CENTER_ROBOT_THRESHOLD=50;

//distance to the station in meters of the "Perpendicular Point" the robot tries to get to before docking
const float perpendicularPointDist=1; 

class blobsInfo; //pre declaration

//The four possible LEDs configurations
typedef enum
{ 
  CONF_L,
  CONF_MIRRORED_L,
  CONF_VERTICALLY_INVERTED_L,
  CONF_MIRRORED_VERTICALLY_INVERTED_L
}
SensorConfiguration;

//Contains all the methods regarding blob handling
class blobsInfo
{
public:
  Blob blobs[WIIMOTE_MAX_BLOBS]; //array with the blobs information
  boolean usableBlobs[4]; //array that identifies if each blob is useful for calculations
  int previousX[4]; //array that saves the X coordinate of each of the blob from the previows cycle
  PVision ircam; //camera object

  float matDist[WIIMOTE_MAX_BLOBS][WIIMOTE_MAX_BLOBS]; //matrix with the distance between the blobs

  float r, ra, rb; //Each of the distances, explained in the documentation, in METERS
  float phi, gamma , teta, alpha; //each of the angles used in the calculations, explained in the documentation, in radians
  float teta1, teta2; //more angles

  //indexes of the position in blobs (which holds the blob information) of each of the blobs, now identified
  //on the following, bla is the topmost of the vertical blobs, blb is the lowermost of the vertical blobs, 
  //blc is the horizontal distanced blob, and bld is the dreaded fourth blob
  byte bla, blb, blc, bld; 
  //

  //indexes of the two blobs that have the least horizontal distance between them
  byte firstLeastHorizDistanceBlobIndex; 
  byte secondLeastHorizDistanceBlobIndex;
  //

  //number of blobs we are seeing
  byte nBlobs; 

  //distance to station, in meters
  float distToStation; 

  //configuration of the led arrangment of the docking stationg we want to dock on
  SensorConfiguration wantedConf; 
  //current configuration of the led arrangment of the docking station we are looking at
  SensorConfiguration conf; 

  blobsInfo();

  void assignBlobsToArray();
  void SortArrayByHeight();
  void calcDistanceMatrixandNearestBlobs();
  void identifyBlobs();
  void identifyConfiguration();
  int averageXCoordinate();
  boolean correctConf();
  boolean validBlobs();
  boolean twoBlobsAreVertical();
  boolean isAligned();
  boolean blaIsCentered();

  void saveBlobstoPrevious();

  void markUsableBlobs();
  void turnBlobUsable(byte index);
  void turnBlobUnusable(byte index);
  void markBlaBlbBlcAsUnusable();

  void calculateDistances();  //METERS
  void calculateTetaAlpha();
  int horizontalDistance();

  byte nUnusable();

  void updateValues();
};

#endif

