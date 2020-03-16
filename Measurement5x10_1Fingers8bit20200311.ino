/****************************************************************
  ESP32 sample code for Mektron pressure sensor with
  5 x 10 pressure and 8 thermal sensing elements, 3 units for 3 fingers.
  2018 Dec 26 Hiroyuki Kajimoto (kajimoto@kaji-lab.jp)
  PC -> ESP32
  1-4: set range
  0xFE: request data
  ESP32 -> pc
  all 8bit data + terminating character(0xFF)

  Currently, all data is sent with 8bit mode. For more precise measurement, 12bit mode can be made.
*****************************************************************/
#include <SPI.h>
//ADS7953 with 16 analog inputs.
#define ADS7953_CS1 33
#define ADS7953_CS2 25
#define ADS7953_CS3 13
#define SCLK 18
#define MOSI 23
#define MISO 19
#define PC_ESP32_MEASURE_REQUEST 0xFE
#define ESP32_PC_MEASURE_RESULT 0xFF
#define FINGER_CHANGE 0xFD

//variables for sensor
const int COL_NUM = 10;
const int ROW_NUM = 5;
const int THERMAL_NUM = 8;
const int FINGER_NUM = 3;
int PressureRange = 3; //1 to 4. Defines sensing PressureRange.
short Conductance[FINGER_NUM][COL_NUM][ROW_NUM] = { 0 };
short Thermal_Conductance[FINGER_NUM][THERMAL_NUM] = { 0 };
//int ColumnBus[COL_NUM] = {1, 2, 3, 4, 5, 12, 26, 14, 15, 16}; //B Bus
int ColumnBus[COL_NUM] = {17, 2, 32, 4, 5, 12, 26, 14, 15, 16}; //B Bus
//int ColumnBus[COL_NUM] =  {16,15,14,26,12,5,4,3,2,1}; //B Bus

//variables for AD converter. Refer schematic of the board. The first two are repeated due to AD chip feature.
#define ADS7953_MODE 1
#define ADS7953_PROG 0
#define ADS7953_RANGE 0
int ADChannel[ROW_NUM + 2] = {0, 1, 2, 3, 4, 0, 1};
int ThermalADChannel[THERMAL_NUM + 2] = { 5, 6, 7, 8, 9, 10, 11, 12, 5, 6 };
int FingerSelect = 0;
long prevTime=0;


void ColumnClear()
{
  int i;
  for (i = 0; i < COL_NUM; i++) {
    digitalWrite(ColumnBus[i], LOW);
  }
}

//Initialize ADS7953
void Sensor_Init()
{
  digitalWrite(ADS7953_CS1, HIGH);
  digitalWrite(ADS7953_CS2, HIGH);
  digitalWrite(ADS7953_CS3, HIGH);

  digitalWrite(ADS7953_CS1, LOW);
  SPI.transfer16((ADS7953_MODE << 12) | (ADS7953_PROG << 11) | (0 & 0x0F) << 7 | (ADS7953_RANGE << 6));
  digitalWrite(ADS7953_CS1, HIGH);

  digitalWrite(ADS7953_CS2, LOW);
  SPI.transfer16((ADS7953_MODE << 12) | (ADS7953_PROG << 11) | (0 & 0x0F) << 7 | (ADS7953_RANGE << 6));
  digitalWrite(ADS7953_CS2, HIGH);

  digitalWrite(ADS7953_CS3, LOW);
  SPI.transfer16((ADS7953_MODE << 12) | (ADS7953_PROG << 11) | (0 & 0x0F) << 7 | (ADS7953_RANGE << 6));
  digitalWrite(ADS7953_CS3, HIGH);
  ColumnClear();
}

//Data aquisition by manual mode.
//Automatic mode might be another option for faster transfer. See AD7953 catalog.
void Sensor_Get_Row(int CS_port, short data[ROW_NUM])
{
  digitalWrite(CS_port, LOW);
  SPI.transfer16((ADS7953_MODE << 12) | (ADS7953_PROG << 11) | ((ADChannel[0]) & 0x0F) << 7 | (ADS7953_RANGE << 6));
  digitalWrite(CS_port, HIGH);

  digitalWrite(CS_port, LOW);
  SPI.transfer16((ADS7953_MODE << 12) | (ADS7953_PROG << 11) | ((ADChannel[1]) & 0x0F) << 7 | (ADS7953_RANGE << 6));
  digitalWrite(CS_port, HIGH);
  
  for (int row = 0; row < ROW_NUM; row++) {
    digitalWrite(CS_port, LOW);
    data[row] = (0x0FFF & SPI.transfer16((ADS7953_MODE << 12) | (ADS7953_PROG << 11) | (ADChannel[row + 2] & 0x0F) << 7 | (ADS7953_RANGE << 6)));
    digitalWrite(CS_port, HIGH);
  }
}

//Data aquisition by manual mode.
//Automatic mode might be another option for faster transfer. See AD7953 catalog.
void Sensor_Get_Row_Thermal(int CS_port, short data[THERMAL_NUM])
{
  digitalWrite(CS_port, LOW);
  SPI.transfer16((ADS7953_MODE << 12) | (ADS7953_PROG << 11) | ((ThermalADChannel[0]) & 0x0F) << 7 | (ADS7953_RANGE << 6));
  digitalWrite(CS_port, HIGH);
  digitalWrite(CS_port, LOW);
  SPI.transfer16((ADS7953_MODE << 12) | (ADS7953_PROG << 11) | ((ThermalADChannel[1]) & 0x0F) << 7 | (ADS7953_RANGE << 6));
  digitalWrite(CS_port, HIGH);
  for (int row = 0; row < THERMAL_NUM; row++) {
    digitalWrite(CS_port, LOW);
    data[row] = (0x0FFF & SPI.transfer16((ADS7953_MODE << 12) | (ADS7953_PROG << 11) | (ThermalADChannel[row + 2] & 0x0F) << 7 | (ADS7953_RANGE << 6)));
    digitalWrite(CS_port, HIGH);
  }
}

void loopMicroseconds(int us)
{
  long t;
  t = micros();
  if(t - prevTime < us){
    delayMicroseconds(us - (t - prevTime)+2); //2 is a magic number...
  }
  prevTime = micros();
}

void setup() {
  int i;
  pinMode(ADS7953_CS1, OUTPUT);
  pinMode(ADS7953_CS2, OUTPUT);
  pinMode(ADS7953_CS3, OUTPUT);
  for (i = 0; i < COL_NUM; i++) {
    pinMode(ColumnBus[i], OUTPUT);
  }
  Serial.begin(921600);
  SPI.begin(SCLK, MISO, MOSI);
  SPI.setFrequency(20000000);
  SPI.setDataMode(SPI_MODE0);
//  SPI.setHwCs(true);
  Sensor_Init();
  prevTime = micros(); //initialize timer
}


void loop() {
  char rcv,snd;
  int val, x1, x2, y1, y2;
  long t;

  if (Serial.available() > 0) {
    rcv = Serial.read();
    if (rcv == PC_ESP32_MEASURE_REQUEST) {
      for (int col = 0; col < COL_NUM; col++) {
        //ColumnSelect = 1 << col;
        digitalWrite(ColumnBus[col], HIGH);
        //delayMicroseconds(100);
        //wait_us(100); //Check if this wait is necessary.
        switch(FingerSelect){
          case 0:
            Sensor_Get_Row(ADS7953_CS1, Conductance[0][col]); //selected finger
            break;
          case 1:
            Sensor_Get_Row(ADS7953_CS2, Conductance[1][col]); //selected finger
            break;
          case 2:
            Sensor_Get_Row(ADS7953_CS3, Conductance[2][col]); //selected finger
            break;
        }
        digitalWrite(ColumnBus[col], LOW);
      }
      //For thermal sensing.
      switch(FingerSelect){
        case 0:
          Sensor_Get_Row_Thermal(ADS7953_CS1, Thermal_Conductance[0]);//selected finger
          break;
        case 1:
          Sensor_Get_Row_Thermal(ADS7953_CS2, Thermal_Conductance[1]);//selected finger
          break;
        case 2:
          Sensor_Get_Row_Thermal(ADS7953_CS3, Thermal_Conductance[2]);//selected finger
          break;
      }
 
      for (int col = 0; col < COL_NUM; col++) {
        for (int row = 0; row < ROW_NUM; row++) {
          val = Conductance[FingerSelect][col][row] >> PressureRange;//upper 8bits
          if (val >= 0xFE) {
            val = 0xFE;
          }
          Serial.write(val);
        }
      }
      for (int row = 0; row < THERMAL_NUM; row++) {
        val = Thermal_Conductance[FingerSelect][row] >> 4;//upper 8bits. Range is fixed.
        if (val >= 0xFE) {
          val = 0xFE;
        }
        Serial.write(val);
      }
      t = micros(); 
      snd = (t/10) & 0xFF;
      if(snd == 0xFE){//avoid sending footer.
        snd = 0xFD;
      }
      Serial.write(snd); //send 8bit time in milliseconds
      Serial.write(ESP32_PC_MEASURE_RESULT); //send terminating character
      
      //Set the loop exactly at 1kHz
      loopMicroseconds(1000);
      
    } else if (rcv > 0 && rcv < 5) { //change range of the sensor
      PressureRange = rcv;
    } else if (rcv == FINGER_CHANGE){
      FingerSelect = (FingerSelect + 1)%FINGER_NUM;
    }
  }

}
