import processing.serial.*;
import javax.swing.*;   
import javax.swing.JOptionPane;

//geometrics

int bar_width=6;

int X_res=1024+bar_width;
int Y_res=384;

float blob_scale=6.0;
float ard_procc_res_ratio=768.0/float(Y_res); //ratio between resolution of pixart sensor and of the processing window
float overhead_scale_ratio=200.0;

//

Serial myPort;  // Create object from Serial class
int op1,op2;
int i=0, j=0;
boolean cont=false;

String val=" ";     // Data received from the serial port
String val1="nada";
String val2;
String val3;
String portName;
int selected_port;
float numbers[];

float coord_x1=0.0, coord_y1=0.0, coord_x2=0.0, coord_y2=0.0, x_off=0.0, y_off=0.0;
float teta1, teta2, r, lambda;

Object[] opcoes;
Object resposta;

//struct blob
public class Blob{
  int X;
  int Y;
  int Size;
  Byte number;
};
//

//vector with all the blobs
Blob[] blobs = new Blob[4];

void setup()
{
  size(X_res, Y_res); //size of the window
  background(0); //black background
  fill(255); //fill black background
  rect(512, 0, bar_width, 384); //vertical divisory
    
  if(Serial.list().length==0) //if there is no COM port connected
  {
    javax.swing.JOptionPane.showMessageDialog(null, "Arduino does not appear to be connected to the computer!");
    exit();
  }
  else //if there is something connected
  {
  opcoes=Serial.list(); //list of the active COM ports

  //input dialog to select the correct COM port 
  resposta = JOptionPane.showInputDialog(
  null,
  "Select the COM port",
  "",
  JOptionPane.PLAIN_MESSAGE,
  null,
  opcoes,
  "");


  if(resposta!=null) //if the user selected a COM port
  { 

  //match the selected COM port name with the proper COM port object, in the ports vector
    while(!resposta.equals(Serial.list()[i]))
    {
      i++;
    }
    selected_port=i;
    //        
    //initialize the port and discard the first batch of data
    portName = Serial.list()[selected_port];
    myPort = new Serial(this, portName, 9600); 
       
    myPort.clear();
    val = myPort.readStringUntil('\n');
    val = " ";
    //
  }  
  else //if the user pressed cancel, exit the program
    exit();
    
}
      
      //initialize the blobs vector
  for(int i= 0; i<4;i++)
  {
    blobs[i]= new Blob();
  }
    
  blobs[0].X=0;
  blobs[1].X=0;
  blobs[2].X=0;
  blobs[3].X=0;

  blobs[0].Y=0;
  blobs[1].Y=0;
  blobs[2].Y=0;
  blobs[3].Y=0;

  blobs[0].Size=0;
  blobs[1].Size=0;
  blobs[2].Size=0;
  blobs[3].Size=0;
  //
  }

void draw()
{

  
  if(myPort.available()>0) //if there is incoming data
    {
    val=myPort.readStringUntil('\n'); //\n is the terminating character
    if(val!=null) //if there is something in the data
      { 
      background(0); //black background
      fill(255);  //clear image from previows cycle
      rect(X_res/2, 0, bar_width, Y_res); //vertical divisor
      
      numbers = float(split(val, ',')); //fill the numbers vector with all the incoming data, which is partitioned  with the ',' character
      
      if(numbers[0]==2000) //2000 is the beginning marker
         { 
           //get the data for all the blobs
         blobs[0].X=int(numbers[2]);
         blobs[0].Y=int(numbers[3]);
         blobs[0].Size=int(numbers[4]);
   
         blobs[1].X=int(numbers[6]);
         blobs[1].Y=int(numbers[7]);
         blobs[1].Size=int(numbers[8]);
  
         blobs[2].X=int(numbers[10]);
         blobs[2].Y=int(numbers[11]);
         blobs[2].Size=int(numbers[12]);
         
         blobs[3].X=int(numbers[14]);
         blobs[3].Y=int(numbers[15]);
         blobs[3].Size=int(numbers[16]);


  println(val);//print the incoming data string
  
 
    //show input string
    text("input: ", 4,380);
    text(val,38,380);
    //
    
    //write blobl A data  
    fill(0,255,255);
    text("Blob A, X, Y, SIZE", 4,50);
    text(blobs[0].X, 4, 70);
    text(blobs[0].Y, 34,70);
    text(blobs[0].Size, 64, 70);
    //
    
    //write blobl B data  
    fill(255,255,0);
    text("Blob B, X, Y, SIZE", 4,100);
    text(blobs[1].X, 4, 120);
    text(blobs[1].Y, 34, 120);
    text(blobs[1].Size, 64, 120);
    //
    
    //write blobl C data  
    fill(255,0,255);
    text("Blob C, X, Y, SIZE", 4,150);
    text(blobs[2].X, 4, 170);
    text(blobs[2].Y, 34, 170);
    text(blobs[3].Size, 64, 170);
    //
    
    //write led configuration
    fill(255);
    text("Conf:", 4,200);
    text(numbers[21], 35, 200);
    //
    
    //draw blobs
    fill(0,255,255);
    ellipse(blobs[0].X/ard_procc_res_ratio, Y_res-blobs[0].Y/ard_procc_res_ratio, blobs[0].Size*blob_scale, blobs[0].Size*blob_scale);
    fill(255,255,0);
    ellipse(blobs[1].X/ard_procc_res_ratio, Y_res-blobs[1].Y/ard_procc_res_ratio, blobs[1].Size*blob_scale, blobs[1].Size*blob_scale);
    fill(255,0,255);
    ellipse(blobs[2].X/ard_procc_res_ratio, Y_res-blobs[2].Y/ard_procc_res_ratio, blobs[2].Size*blob_scale, blobs[2].Size*blob_scale);    
    fill(0,255,0);
    ellipse(blobs[3].X/ard_procc_res_ratio, Y_res-blobs[3].Y/ard_procc_res_ratio, blobs[3].Size*blob_scale, blobs[3].Size*blob_scale);
    //
    
    //lines between the visible blobs
    stroke(255); //white line
    line(blobs[0].X/ard_procc_res_ratio, Y_res-blobs[0].Y/ard_procc_res_ratio,blobs[1].X/ard_procc_res_ratio, Y_res-blobs[1].Y/2);
    line(blobs[0].X/ard_procc_res_ratio, Y_res-blobs[0].Y/ard_procc_res_ratio,blobs[2].X/ard_procc_res_ratio, Y_res-blobs[2].Y/2);
    line(blobs[1].X/ard_procc_res_ratio, Y_res-blobs[1].Y/ard_procc_res_ratio,blobs[2].X/ard_procc_res_ratio, Y_res-blobs[2].Y/2);
    //
    
    
    
    //coord_x calculation
    teta1=numbers[17];
    teta2=numbers[18];
    r=numbers[19];
    lambda=numbers[20];
    
    //coord_x calculation
    coord_x1=(-1)*r*sin(teta1)*overhead_scale_ratio+X_res/ard_procc_res_ratio+bar_width+X_res/(ard_procc_res_ratio*2);
    coord_x2=(-1)*r*sin(teta2)*overhead_scale_ratio+X_res/ard_procc_res_ratio+bar_width+X_res/(ard_procc_res_ratio*2);
    //coord_y calculation
    coord_y1= r*cos(teta1)*overhead_scale_ratio;
    coord_y2= r*cos(teta2)*overhead_scale_ratio;  
    //draw robot location
    fill(255,0,0);
    ellipse(coord_x1,coord_y1, 15, 15);
    fill(0,255,0);
     ellipse(coord_x2,coord_y2,15, 15);
    
    //draw docking station location
    fill(0,0,255); //blue color
      ellipse(X_res/ard_procc_res_ratio+bar_width+X_res/(ard_procc_res_ratio*2),0, 20, 20);
      
      //draw line with the direction the robot is facing
      x_off=X_res/ard_procc_res_ratio+bar_width+X_res/(ard_procc_res_ratio*2)-coord_x1; //x distance from the docking station
      y_off=coord_y1;  //y distance from the docking station
      
      line(coord_x1, coord_y1,coord_x1+(cos(lambda+atan(y_off/x_off))*30), coord_y1+(sin(lambda+atan(y_off/x_off))*30));
      
            x_off=X_res/ard_procc_res_ratio+bar_width+X_res/(ard_procc_res_ratio*2)-coord_x2; //x distance from the docking station
      y_off=coord_y2;  //y distance from the docking station
      
      line(coord_x2, coord_y2,coord_x2+(cos(lambda+atan(y_off/x_off))*30), coord_y2+(sin(lambda+atan(y_off/x_off))*30));
    line(coord_x2, coord_y2,coord_x2+(cos(atan(y_off/x_off))*30), coord_y2+(sin(atan(y_off/x_off))*30));
      //
      println(r*sin(teta2));
               }
      }
    }
  }

