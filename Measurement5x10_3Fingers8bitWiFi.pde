
// 2 fingers measurement, 8bit, fast scan
// 2018 Dec. 06 Hiroyuki Kajimoto

import processing.net.*;
//import processing.serial.*;

// TCPIP:
//Serial myPort;       
Client myClient; 
int dataIn; 

 
String[] arraySTR;
String s;

//user definitions
//String COM_PORT="COM9"; //change this!

//software definitions
final int PC_MBED_MEASURE_REQUEST=0xFE;
final int MBED_PC_MEASURE_RESULT=0xFF;

//graphical attributes
final int WINDOW_SIZE_X=1200;
final int WINDOW_SIZE_Y=600;
final int SENSOR_X_NUM=10;
final int SENSOR_Y_NUM=5;
final int THERMAL_NUM=8;
final int FINGER_NUM=3;
final int[] FOFFESET = {0,2,1};
final int PIXEL_SIZE_X=WINDOW_SIZE_X/3/(SENSOR_X_NUM+1);
final int PIXEL_SIZE_Y=WINDOW_SIZE_Y/2/(SENSOR_Y_NUM);
final float[] THERMAL_XCOORD = {3.0, 1.0, 3.0, 7.0, 9.0, 7.0, 5.0, 5.0}; 
final float[] THERMAL_YCOORD = {  3, 2, 1, 1, 2, 3, 1, 3}; 
final float THERMAL_OFFSET = 28.0; // degree-C, Temperature of the room.
final float THERMAL_MAX = 35.0; // degree-C, Temperature of the room.
final float THERMAL_AMP = 30.0; 
int DrawMode = RECT;
int PressureRange  = 3;
int u_timer=0, prev_t=0;
boolean SerialDataSendRequest = false, SaveDataFlag= false;;
PrintWriter output;



int[][][]PressureDistribution = new int[FINGER_NUM][SENSOR_X_NUM][SENSOR_Y_NUM];
float[][] ThermalDistribution = new float[FINGER_NUM][THERMAL_NUM];

void settings() {
  size(WINDOW_SIZE_X, WINDOW_SIZE_Y, P2D);
}

   
void setup() {  
  //serial setting
  //myPort = new Serial(this, COM_PORT, 115200);
  //myPort.clear();
  //myPort.bufferUntil(MBED_PC_MEASURE_RESULT); 
  //myPort.write(PC_MBED_MEASURE_REQUEST); //send initial request.
  
  textSize(32);
  noStroke();
  print("Welcome to sensor sample code.\n");
  print("1-4:sensing range\n");
  myClient = new Client(this, "192.168.0.33", 5204); 
//  myClient = new Client(this, "192.168.1.10", 5204); 
  myClient.clear();
  myClient.write(PC_MBED_MEASURE_REQUEST); 
  thread("receiveData");
}

void draw() { 
  int x, y, finger, xoffset;


  //clear screen
  background(20);


  for (finger=0; finger<FINGER_NUM; finger++) {
    xoffset =  (SENSOR_X_NUM+1)*PIXEL_SIZE_X*FOFFESET[finger];
    for (x=0; x<SENSOR_X_NUM; x++) {
      for (y=0; y<SENSOR_Y_NUM; y++) {
        fill(20, PressureDistribution[finger][x][y], 20);
        rect((SENSOR_X_NUM-x)*PIXEL_SIZE_X + xoffset, y*PIXEL_SIZE_Y, PIXEL_SIZE_X, PIXEL_SIZE_Y);
      }
    }
    for (x=0; x<THERMAL_NUM; x++) {
      fill((int)((ThermalDistribution[finger][x]-THERMAL_OFFSET)*THERMAL_AMP), 20, (THERMAL_MAX - ThermalDistribution[finger][x])*THERMAL_AMP);
      rect((int)(THERMAL_XCOORD[x]*(float)PIXEL_SIZE_X) + xoffset, WINDOW_SIZE_Y/2 + (int)(THERMAL_YCOORD[x]*(float)PIXEL_SIZE_Y), PIXEL_SIZE_X, PIXEL_SIZE_Y);
      fill(0, 100, 150);
      text((int)ThermalDistribution[finger][x], (int)(THERMAL_XCOORD[x]*(float)PIXEL_SIZE_X) + xoffset, WINDOW_SIZE_Y/2 + (int)(THERMAL_YCOORD[x]*(float)PIXEL_SIZE_Y));
    }
  }
  
}

//Request and receive serial data.
//Returns -1 if the data is not prepared.
//Returns 8 bit time in milliseconds, measured in mbed. 
void receiveData()
{
  int rcv, x, y, finger, t,ii;
  byte[] byteBuffer = new byte[(SENSOR_X_NUM*SENSOR_Y_NUM+THERMAL_NUM)*FINGER_NUM+2];
  
  while(true){
    if(myClient.available()>=(SENSOR_X_NUM*SENSOR_Y_NUM+THERMAL_NUM)*FINGER_NUM+2){
      myClient.readBytes(byteBuffer);
 //     myClient.write(PC_MBED_MEASURE_REQUEST); //next request.
      ii=0;
      //data reading.This is the data associated with previous request.
      for (finger=0; finger<FINGER_NUM; finger++) {
        for (x = 0; x<SENSOR_X_NUM; x++) {
          for (y = 0; y<SENSOR_Y_NUM; y++) {
            //PressureDistribution[finger][x][y]= myClient.read(); //upper 8bits
            PressureDistribution[finger][x][y]= int(byteBuffer[ii]);
            ii++;
          }
        }
        for (x = 0; x<THERMAL_NUM; x++) {
          //rcv = myClient.read(); 
          rcv = int(byteBuffer[ii]);
          ii++;
          ThermalDistribution[finger][x] =0.248 * rcv - 16.557; //Calculation by theoretical formula.
       }
      }
      
      //t = myClient.read();  //time (0-255) in milliseconds (mbed)
      t = int(byteBuffer[ii]);
      if(t>=prev_t){
        u_timer = u_timer + t - prev_t;
      }else{ //overflow management
        u_timer = u_timer + t + 255  - prev_t;
      }
      prev_t = t;
      
      //myClient.read(); //remove the terminating character
      if(SaveDataFlag == true){
        output.print((float)u_timer/10.0 + ",");
        //int m = millis(); output.print(m+","); //Check with PC timer

        for (finger=0; finger<FINGER_NUM; finger++) {
          for (x = 0; x<SENSOR_X_NUM; x++) {
            for (y = 0; y<SENSOR_Y_NUM; y++) {
              output.print(PressureDistribution[finger][x][y]+",");
            }
          }
          for (x = 0; x<THERMAL_NUM; x++) {
              output.print(ThermalDistribution[finger][x]+",");
          }
        }  
        output.println();
      }
    }
  
  }
}

void keyTyped() {
  int finger, x,y;

  switch (key) {
    case '1':  myClient.write(1); print("Pressure Range was set to 1\n"); break; 
    case '2':  myClient.write(2); print("Pressure Range was set to 2\n"); break; 
    case '3':  myClient.write(3); print("Pressure Range was set to 3\n"); break; 
    case '4':  myClient.write(4); print("Pressure Range was set to 4\n"); break; 
    case 's':  
      if(SaveDataFlag == false){
        String filename = nf(year(), 2) + nf(month(), 2) + nf(day(), 2) +"-"+ nf(hour(), 2) + nf(minute(), 2) + nf(second(), 2) + ".csv";
        output = createWriter(filename); 
        output.print("time(us),");
        for (finger=0; finger<FINGER_NUM; finger++) {
          for (x = 0; x<SENSOR_X_NUM; x++) {
            for (y = 0; y<SENSOR_Y_NUM; y++) {
              output.print("P"+finger+x+y+",");
            }
          }
          for (x = 0; x<THERMAL_NUM; x++) {
              output.print("T"+finger+x+",");
          }
        }   
        output.println();
        SaveDataFlag = true;
        u_timer=0;
        prev_t=0;
      }else{
        SaveDataFlag=false;
        output.close();
      }
      break;

    default : 
    break;
  }
}
