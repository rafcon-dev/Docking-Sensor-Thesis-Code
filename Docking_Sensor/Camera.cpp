#include "Camera.h"


blobsInfo::blobsInfo()
{
  ircam.init(); //initialize camera object
}


//takes the data from the wiimote camera and puts the coordinates in a more sensible coordinate system, in a array
void blobsInfo::assignBlobsToArray()
{
  nBlobs=ircam.read(); //read() returns the number of blobs seen

  blobs[0].X=X_RES-ircam.Blob1.X;
  blobs[1].X=X_RES-ircam.Blob2.X;
  blobs[2].X=X_RES-ircam.Blob3.X;
  blobs[3].X=X_RES-ircam.Blob4.X;

  blobs[0].Y=Y_RES-ircam.Blob1.Y;
  blobs[1].Y=Y_RES-ircam.Blob2.Y;
  blobs[2].Y=Y_RES-ircam.Blob3.Y;
  blobs[3].Y=Y_RES-ircam.Blob4.Y;

  blobs[0].Size=ircam.Blob1.Size;
  blobs[1].Size=ircam.Blob2.Size;
  blobs[2].Size=ircam.Blob3.Size;
  blobs[3].Size=ircam.Blob4.Size;
}


//puts the blobs lower in height lower in the array positions
void blobsInfo::SortArrayByHeight() 
{
  //Insertion sort
  Blob blobTemp;
  int i,j;
  for (i=1;i<WIIMOTE_MAX_BLOBS;i++)
  {
    j=i;
    while(j>0 && blobs[j-1].Y > blobs[j].Y)
    {
      blobTemp=blobs[j-1];
      blobs[j-1]=blobs[j];
      blobs[j]=blobTemp;
      j--;
    }
  }
}


//calculates the distance matrix and the two blobs which have the least horizontal distance between them
void blobsInfo::calcDistanceMatrixandNearestBlobs ()
{
  int min_dist_x=10000; //big number
  int dist_x=0;
  int dist_y=0;
  byte i, j;

  firstLeastHorizDistanceBlobIndex=0; 
  secondLeastHorizDistanceBlobIndex=1;

  //cycle trough all the positions
  for(i=0;i<WIIMOTE_MAX_BLOBS;i++)
  {
    for(j=0;j<WIIMOTE_MAX_BLOBS;j++)
    {
      //distance between two points
      matDist[i][j]=(float)sqrt
        (
      pow(blobs[i].X-blobs[j].X,2)
        +
        pow(blobs[i].Y-blobs[j].Y,2)
        );

      //find the two valid blobs that have the least horizontal distance between them
      if(blobs[i].Y>=0&&blobs[j].Y>=0) //that is, if both blobs are valid (a non valid blob has Y=-255)
      {  
        dist_x=abs(blobs[i].X-blobs[j].X);

        if((min_dist_x>dist_x) && i!=j )
        {
          firstLeastHorizDistanceBlobIndex=i; 
          secondLeastHorizDistanceBlobIndex=j;
          min_dist_x=dist_x;
        }
      }
    }
  }
}


//Identifies which of the observed blob is A, B, C and D
void blobsInfo::identifyBlobs()
{
  byte bytemp;

  //find provisory blob c;
  blc=0;
  while(blc==firstLeastHorizDistanceBlobIndex  ||  blc==secondLeastHorizDistanceBlobIndex)
    blc++;

  //find provisory blob d;
  bld=0;
  while(bld==firstLeastHorizDistanceBlobIndex  ||  bld==secondLeastHorizDistanceBlobIndex  ||  bld==blc)
    bld++;

  if (blobs[blc].Y<blobs[bld].Y) //because bld should be a reflection
  {
    bytemp=blc;

    blc=bld;
    bld=bytemp;   
  }

  //figure out which one is above and below
  if(blobs[firstLeastHorizDistanceBlobIndex].Y   >   blobs[secondLeastHorizDistanceBlobIndex].Y) 
  {
    bla=firstLeastHorizDistanceBlobIndex;
    blb=secondLeastHorizDistanceBlobIndex;
  }
  else
  {
    blb=firstLeastHorizDistanceBlobIndex;
    bla=secondLeastHorizDistanceBlobIndex;
  }      
}


//Identifies the blob configuration we are currently observing
void blobsInfo::identifyConfiguration()
{
  if(blobs[blc].X>=blobs[bla].X)
  {
    if(matDist[blc][bla]<=matDist[blc][blb])
      conf=CONF_VERTICALLY_INVERTED_L;
    else
      conf=CONF_L;
  }
  else
  {
    if(matDist[blc][bla]<=matDist[blc][blb])
      conf=CONF_MIRRORED_VERTICALLY_INVERTED_L;
    else
      conf=CONF_MIRRORED_L;
  }
}


//Returns the average X coordinates of all the blobs
int blobsInfo::averageXCoordinate()  //Not exactly average X coordinate, but should work
{
  byte i;
  int result=0;
  for (i=0; i < nBlobs; i++)
  {
    result+=blobs[i].X;
  }
  return result/nBlobs;
}


//Returns true if the configuration we are seeing is the configuration wanted
boolean blobsInfo::correctConf()
{
  return (wantedConf==conf);
}


//Returns true if two blobs are near the same vertical line
boolean blobsInfo::twoBlobsAreVertical()
{
  return (abs(blobs[bla].X-blobs[blb].X)<abs(blobs[bla].Y-blobs[blb].Y));
  return false;
}


//Marks all blobs as usable
void blobsInfo::markUsableBlobs()
{
  int i;
  for (i=0; i<WIIMOTE_MAX_BLOBS;i++)
  {
    //less than zero because then the blob is not being found; or if there is a sudden change of position
    if(blobs[i].Y<0 || abs (blobs[i].X-previousX[i]) > (int)(X_RES/2)) 
      turnBlobUsable(i);
  }
}


//Saves the current blobs to memory, to be used later
void blobsInfo::saveBlobstoPrevious()
{
  previousX[0]=blobs[0].X;
  previousX[1]=blobs[1].X;
  previousX[2]=blobs[2].X;
  previousX[3]=blobs[3].X;
}


//Turns a blob usable
void blobsInfo::turnBlobUsable (byte index)
{
  usableBlobs[index]=true;
}


//Turns a blob unusable
void blobsInfo::turnBlobUnusable (byte index)
{
  usableBlobs[index]=false;
}


//Marks blob A, B and C as unusable
void blobsInfo::markBlaBlbBlcAsUnusable()
{
  turnBlobUnusable (bla);
  turnBlobUnusable (blb);
  turnBlobUnusable (blc);
}


//Returns the number of unusable blobs
byte blobsInfo::nUnusable()
{
  byte result=0;
  byte i;
  for(i=0;i<WIIMOTE_MAX_BLOBS;i++)
  {
    if (!usableBlobs[i])
      result+=result;
  }
  return result;
}


//Returns true if the blobs seem to have a valid configuration
boolean blobsInfo::validBlobs()
{ 
  byte i,j;
  int distBlaBlc;
  if (nBlobs !=3)
    return false;

  //needs testing
  if (abs (blobs[blc].X - blobs[bla].X) < abs(blobs[blc].Y-blobs[bla].Y && 
    abs(blobs[blc].X-blobs[blb].X) < abs(blobs[blc].Y-blobs[blb].Y))) 
    //
    return false;

  return true;
}


//Returns true if the robot is near perpendicular to the Station
boolean blobsInfo::isAligned()
{
  if (!validBlobs()) //if the blobs are not valid, no use doing this
      return false;

  if (abs(teta1)>0.25) //Might need tunning
    return false;
  return true;
}


//Returns true if A blob is centered in the camera
boolean blobsInfo::blaIsCentered()
{
  if (nBlobs==0) //it's as if it is centered
    return true;

  if( blobs[bla].X<( (X_RES/2) + CENTER_ROBOT_THRESHOLD ) && blobs[bla].X> ((X_RES/2)-CENTER_ROBOT_THRESHOLD))
    return true;

  return false;
}

//Calculates distances between blobs and individual LEDs
void blobsInfo::calculateDistances()
{ 
  gamma = atan( matDist[bla][blb]/float(Y_RES)*tan(FOV_Y)); //linear FOV to angular FOV

  if(blobs[bla].Y<(float(Y_RES)/2.0)) //if the lights are below the horizon
  {
    phi= atan( (float(Y_RES)/2.0-blobs[bla].Y)/float(Y_RES)* tan(FOV_Y) ); //is zero by default

    ra=sin((PI/2.0-gamma-phi))*SENS_Y/sin(gamma); //distance to point a//   sacode baby

    r=cos(phi)*ra;   //distance to vertical axis of vertical leds//

    rb=r/cos((phi+gamma)); //distance to point b// 
  }
  else if(blobs[blb].Y>(float(Y_RES)/2.0)) //if the lights are above the horizon
  {
    phi= atan((blobs[blb].Y-float(Y_RES)/2.0)/float(Y_RES)*tan(FOV_Y) ); //is zero by default

    rb=sin((PI/2.0-gamma-phi))*SENS_Y/sin(gamma); //distance to point b//   sacode baby

    r=cos(phi)*rb;   //distance to vertical axis of vertical leds//

    ra=r/cos(phi+gamma); //distance to point a//
  }
  else
  {
    phi=atan( (blobs[bla].Y-float(Y_RES)/2.0)/float(Y_RES)* tan(FOV_Y) ); //is zero by default

    ra=(phi*SENS_Y/gamma)/sin(phi); //distance to point A//

    r=cos(phi)*ra;

    rb=r/cos(gamma-phi);    
  }
}


void blobsInfo::calculateTetaAlpha()
{
  if( conf==CONF_VERTICALLY_INVERTED_L  ||  conf==CONF_MIRRORED_VERTICALLY_INVERTED_L )
  {
    alpha=atan (abs(matDist[bla][blc]/float(X_RES)*tan(FOV_X)));  
  }
  else if( conf==CONF_L  ||  conf==CONF_MIRRORED_L )
  {
    alpha=atan (abs(matDist[blb][blc]/float(X_RES)* tan(FOV_X)));
  }

  teta1=-alpha+acos(r*(sin(alpha))/float(SENS_X));

  teta2=-(alpha+acos(r*(sin(alpha))/float(SENS_X)));
}


int blobsInfo::horizontalDistance()
{
  if(conf == CONF_VERTICALLY_INVERTED_L || conf == CONF_L) //return distance from the blobs that have the same vertical ordinate
    return (int)(matDist[blb][blc]);
  else
    return (int)(matDist[bla][blc]);
}


void blobsInfo::updateValues()
{
  saveBlobstoPrevious();
  assignBlobsToArray();
  SortArrayByHeight();
  calcDistanceMatrixandNearestBlobs();
  identifyBlobs();
  identifyConfiguration();

  calculateDistances();  //METERS  //Only calculate when needed? resource intensive
  calculateTetaAlpha();            //Only calculate when needed? resource intensive

  markUsableBlobs();
}



