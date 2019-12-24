/*
    <Arduino testcode for GPA-FHDO>
    Copyright (C) <2019>  <Benjamin Menkuec>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <EEPROM.h>
#include <SPI.h>

enum class Mode : char{idle,ramp,pulse,findZeroAmp,calGain};

// eeprom saved variables
const unsigned int numCalElements = 33;
unsigned int zeroAmpVal[4] = {0x8000, 0x8000, 0x8000, 0x8000};
float OPAmpVoltageToCurrentFactor[4][numCalElements];
unsigned char channel=3;
Mode mode = Mode::idle;

// settings
const bool displayHexValues=false;
const byte SPImode = SPI_MODE0;
//unsigned int calVal[]={0x0000, 0x8000, 0xFFFF};
//unsigned int calVal[]={0x5000, 0x6500, 0x8000, 0x9500, 0xB000};
//unsigned int calVal[numCalElements]={0x0000, 0x1000, 0x2000, 0x3000, 0x4000, 0x5000, 0x6000, 0x7000, 0x8000, 0x9000, 0xA000, 0xB000, 0xC000, 0xD000,0xE000, 0xF000,0xFFFF};
unsigned int calVal[numCalElements]={0x0000, 0x0500, 0x1000, 0x1500, 0x2000, 0x2500, 0x3000, 0x3500, 0x4000, 0x4500, 0x5000, 0x5500, 0x6000, 0x6500, 0x7000, 0x7500, 0x8000,
  0x8500, 0x9000, 0x9500, 0xA000, 0xA500, 0xB000, 0xB500, 0xC000, 0xC500, 0xD000,0xD500, 0xE000, 0xE500, 0xF000, 0xF500, 0xFFFF};
const float rampMinAmp = -2.0;
const float rampMaxAmp = 2.0;
const float rampStepSize = 1;
const unsigned int rampCyclePause = 0; // in milliseconds

const float pulseHighAmp = 5;
const float pulseLowAmp = 0;
const unsigned int pulseOnCycles = 2;

void loadCalibration()
{
  unsigned int addr = 0x10;
  for(unsigned int channel = 0;channel<4;channel++)
  {
    EEPROM.get(addr,zeroAmpVal[channel]);
    addr+=4;
    for(unsigned int i=0;i<numCalElements;i++)
    {
      EEPROM.get(addr,OPAmpVoltageToCurrentFactor[channel][i]);
      addr+=4;      
    }
  }
}

void saveCalibration()
{
  unsigned int addr = 0x10;
  for(unsigned int channel = 0;channel<4;channel++)
  {
    EEPROM.put(addr,zeroAmpVal[channel]);
    addr+=4;
    for(unsigned int i=0;i<numCalElements;i++)
    {
      EEPROM.put(addr,OPAmpVoltageToCurrentFactor[channel][i]);
      addr+=4;      
    }
  }
}

void initADC(){
  byte dataToSend[4];

  dataToSend[0]=0x85; // reset
  dataToSend[1]=0x00;
  dataToSend[2]=0x00;
  dataToSend[3]=0x00;

  digitalWrite(SS,HIGH); 
  delay(10);
  SPI.transfer(dataToSend,4);
  delay(10);
  digitalWrite(SS,LOW); 
  delay(10);

  dataToSend[0]=(0x08 << 1) | 1; // set channel 3 input range
  dataToSend[1]=0x06;
  dataToSend[2]=0x00;

  digitalWrite(SS,HIGH); 
  delay(10);
  SPI.transfer(dataToSend,3);
  delay(10);
  digitalWrite(SS,LOW);   

  dataToSend[0]=(0x07 << 1) | 1; // set channel 2 input range
  dataToSend[1]=0x06;
  dataToSend[2]=0x00;

  digitalWrite(SS,HIGH); 
  delay(10);
  SPI.transfer(dataToSend,3);
  delay(10);
  digitalWrite(SS,LOW);   

  dataToSend[0]=(0x06 << 1) | 1; // set channel 1 input range
  dataToSend[1]=0x06;
  dataToSend[2]=0x00;

  digitalWrite(SS,HIGH); 
  delay(10);
  SPI.transfer(dataToSend,3);
  delay(10);
  digitalWrite(SS,LOW);   


  dataToSend[0]=(0x05 << 1) | 1; // set channel 0 input range
  dataToSend[1]=0x06;
  dataToSend[2]=0x00;

  digitalWrite(SS,HIGH); 
  delay(10);
  SPI.transfer(dataToSend,3);
  delay(10);
  digitalWrite(SS,LOW);   

  dataToSend[0]=0x05; // set all channels to power up state
  dataToSend[1]=0x00;
  dataToSend[2]=0x00;

  digitalWrite(SS,HIGH); 
  delay(10);
  SPI.transfer(dataToSend,3);
  delay(10);
  digitalWrite(SS,LOW);  
    
  dataToSend[0]=0x03; // enable all channels for autoscan
  dataToSend[1]=0x0f;
  dataToSend[2]=0x00;

  digitalWrite(SS,HIGH); 
  delay(10);
  SPI.transfer(dataToSend,3);
  delay(10);
  digitalWrite(SS,LOW);   

  
  dataToSend[0]=0xA0;
  dataToSend[1]=0x00;
  dataToSend[2]=0x00;
  dataToSend[3]=0x00;

  digitalWrite(SS,HIGH); 
  delay(10);
  SPI.transfer(dataToSend,4);
  delay(10);
  digitalWrite(SS,LOW); 
  delay(10);
}


void setup() 
{
  Serial.begin(115200);
  Serial.setTimeout(100);
  digitalWrite(SS, LOW);  
  SPI.begin (); //Verbindung starten
  SPI.setClockDivider(SPI_CLOCK_DIV8); //Geschwindigkeit verlangsamen
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPImode);
  initADC();
 
  EEPROM.get(0x00,mode);  
  EEPROM.get(0x01,channel);  
  loadCalibration();

  if(channel<0 || channel >3)
    channel=3;
}

float ADCToVolt(const unsigned int val)
{
  return static_cast<float>(val)/static_cast<float>(0xFFFF)*4.096*1.25;
}

float ADCToAmpere(const unsigned int val)
{
  const float maxVoltage=1.25*4.096;
  float resistance=0.2; // 200 mOhms
  return (ADCToVolt(val)-2.5)/resistance;
}


float DACToAmpere(const unsigned int val)
{
  return 0;
}


float DACToVolt(const unsigned int val)
{
  return static_cast<float>(val)/static_cast<float>(0xFFFF)*5.0;
}

unsigned int AmpereToDAC(const float ampere)
{
  unsigned int leftIndex=0;
  unsigned int rightIndex=numCalElements-1;
  calVal[(numCalElements-1)/2]=zeroAmpVal[channel];
  for(unsigned int n=0;n<numCalElements;n++)
  {
    if(OPAmpVoltageToCurrentFactor[channel][n]<=ampere)
      leftIndex=n;
    if(OPAmpVoltageToCurrentFactor[channel][n]>=ampere)
    {
      rightIndex=n;
      break;
    }
  }  
  //char data[100];
  //sprintf(data,"left %d right %d\n", leftIndex,rightIndex);
  //Serial.print(data);  
  if(leftIndex==rightIndex)
  {
    Serial.print("Ampere value outside calibration range!");
    delay(1000);
    return calVal[leftIndex];
  }
  return (calVal[rightIndex]-calVal[leftIndex])/(OPAmpVoltageToCurrentFactor[channel][rightIndex]-OPAmpVoltageToCurrentFactor[channel][leftIndex])
    *(ampere-OPAmpVoltageToCurrentFactor[channel][leftIndex])+calVal[leftIndex];
}

void writeDACValue(const unsigned int val, const char channel)
{
  byte dataToSend[4];
  dataToSend[0]=0x08 + channel; 
  dataToSend[1]=static_cast<byte>(val>>8); 
  dataToSend[2]=static_cast<byte>(val);
  dataToSend[3]=0;
  digitalWrite(SS,LOW); 
  SPI.transfer(dataToSend,3);
  digitalWrite(SS,HIGH); 
  //char data[100];
  //sprintf(data,"write %04x\n", val);
  //Serial.print(data);
}

void pulseLoop()
{
  char data[100];
  
  static int loopCount = 0;
  const int maxLoopCount = 100;

  const unsigned int Vhigh = AmpereToDAC(pulseHighAmp);
  const unsigned int Vlow = AmpereToDAC(pulseLowAmp);

  if (loopCount == maxLoopCount)
    loopCount = 0;
  else
    loopCount++;
    
  if(loopCount <pulseOnCycles)
    writeDACValue(Vhigh, channel);
  else
    writeDACValue(Vlow, channel);

  Serial.print(loopCount);
  Serial.print("\t");
  for(int n=0;n<4;n++)
  {
    byte dataToSend[4];
    dataToSend[0]=0x00;
    dataToSend[1]=0x00;
    dataToSend[2]=0x00;
    dataToSend[3]=0x00;
  
    digitalWrite(SS,HIGH); 
    SPI.transfer(dataToSend,4);
    digitalWrite(SS,LOW); 
    char str_temp[20];
    /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
    dtostrf(ADCToAmpere(static_cast<unsigned long>(dataToSend[2])<<8 | static_cast<unsigned long>(dataToSend[3])), 10, 7, str_temp);
    if(displayHexValues)
      sprintf(data,"%02x%02x %s\t",dataToSend[2],dataToSend[3],str_temp);
    else
      sprintf(data,"%s A\t",str_temp);
    Serial.print(data);
  }
  Serial.print("\r\n");  
  delay(1);
}

unsigned int rampVal = 0;
static unsigned int rampLoopCount = 1;


void initRamp()
{
  rampLoopCount = 1;
  rampVal = AmpereToDAC(rampMinAmp);
}

void rampLoop()
{
  char data[100];
  const bool plotDataOnly=true;
  const unsigned int maxLoopCount = 1;
  if (rampLoopCount == maxLoopCount)
  {
    rampLoopCount = 1;
    if(rampVal<AmpereToDAC(rampMaxAmp))
      rampVal+=rampStepSize;
    else
      rampVal=AmpereToDAC(rampMinAmp);
  }
  else
    rampLoopCount++;

  if(rampLoopCount == 1)
  {
    if(!plotDataOnly)
    {
      char str_temp[8];
      /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
      dtostrf(DACToAmpere(rampVal), 7, 4, str_temp);
      sprintf(data,"*** set current to %s A ***\r\n",str_temp);
      Serial.print(data);    
    }
    writeDACValue(rampVal,channel);
  }
  delay(1); // wait 1 ms

  if(!plotDataOnly)
  {
    Serial.print(rampLoopCount);
    Serial.print("\t");
  }

  for(int n=0;n<4;n++)
  {
    byte dataToSend[4];
    dataToSend[0]=0x00;
    dataToSend[1]=0x00;
    dataToSend[2]=0x00;
    dataToSend[3]=0x00;
  
    digitalWrite(SS,HIGH); 
    SPI.transfer(dataToSend,4);
    digitalWrite(SS,LOW); 
    char str_temp[20];
    const unsigned int ADCVal= static_cast<unsigned long>(dataToSend[2])<<8 | static_cast<unsigned long>(dataToSend[3]);
    dtostrf(ADCToAmpere(ADCVal), 10, 7, str_temp);
    if(plotDataOnly)
    {
      if(channel == n)
      {
        char str_temp2[20];
        dtostrf(DACToAmpere(rampVal), 10, 7, str_temp2);
        sprintf(data,"0x%04x %s 0x%04x %s",rampVal,str_temp2,ADCVal,str_temp);
        Serial.print(data);
      }
    }
    else
    {
      if(displayHexValues)
        sprintf(data,"%02x%02x %s\t",ADCVal,str_temp);
      else
        sprintf(data,"%s A\t",str_temp);
      Serial.print(data);
    }
  }
  Serial.print("\r\n");      
}

void determineGainValues()
{
  char data[100];
  char str_temp[20];
  char str_temp2[20];
  byte dataToSend[4];

  calVal[(numCalElements-1)/2]=zeroAmpVal[channel];

  for(unsigned int loopCount=0;loopCount<numCalElements;loopCount++)
  {
    writeDACValue(calVal[loopCount],channel);
    delayMicroseconds(100);
    for(int n=0;n<4;n++)
    {  
      dataToSend[0]=0x00;
      dataToSend[1]=0x00;
      dataToSend[2]=0x00;
      dataToSend[3]=0x00;
      digitalWrite(SS,HIGH); 
      SPI.transfer(dataToSend,4);
      digitalWrite(SS,LOW);   
      if(channel == n)
      {
        float ampere = ADCToAmpere(static_cast<unsigned long>(dataToSend[2])<<8 | static_cast<unsigned long>(dataToSend[3]));
        dtostrf(ADCToVolt(static_cast<unsigned long>(dataToSend[2])<<8 | static_cast<unsigned long>(dataToSend[3])), 10, 7, str_temp);
        OPAmpVoltageToCurrentFactor[channel][loopCount]=ampere;
        dtostrf(OPAmpVoltageToCurrentFactor[channel][loopCount], 10, 7, str_temp2);
        sprintf(data,"%02x%02x -> %s V  %s A\r\n",(byte)(calVal[loopCount]>>8),(byte)calVal[loopCount],str_temp,str_temp2);
        Serial.print(data);
      }    
    }
  }
  saveCalibration();
  writeDACValue(AmpereToDAC(0),channel);
}

/*
 * This routine only functions properly if the coil is connected all the time.
 */
void findZeroAmpVoltage()
{
  char data[100];
  unsigned long currentV = 0x8000;
  unsigned long lastV=0;
  byte dataToSend[4];
  char str_temp2[15];
  char str_temp[15];
  
  bool continueLoop=true;
  while(continueLoop)
  {   
    writeDACValue(currentV,channel);    
  
    for(int n=0;n<4;n++)
    {  
      dataToSend[0]=0x00;
      dataToSend[1]=0x00;
      dataToSend[2]=0x00;
      dataToSend[3]=0x00;
      digitalWrite(SS,HIGH); 
      SPI.transfer(dataToSend,4);
      digitalWrite(SS,LOW);   
      if(channel == n)
      {
        const float dacVolt = DACToVolt(currentV);
        const float volt = ADCToVolt(static_cast<unsigned int>(dataToSend[2])<<8 | static_cast<unsigned int>(dataToSend[3]));
        dtostrf(dacVolt, 10, 7, str_temp2);      
        dtostrf(volt, 10, 7, str_temp);
        sprintf(data,"%02x%02x %s -> %s V\r\n",(byte)(currentV>>8),(byte)currentV,str_temp2,str_temp);
        Serial.print(data);
  
        const unsigned long temp=currentV;
        if(volt>2.5)
          currentV--;
        else
          currentV++;
  
        if(currentV==lastV)
        {
          zeroAmpVal[channel] = currentV;
          EEPROM.put(0x00+((channel+1)<<4),zeroAmpVal[channel]);
          Serial.print("Zero current voltage found\r\n");
          continueLoop=false;
        }
        else
          lastV=temp;
      }    
    }
  }
  saveCalibration();  
}


void loop() 
{
  if(Serial.available() > 0)
  {
    int inByte=Serial.read();
    char data[50];
    switch((char)inByte)
    {
      case 'c':
      {
        delay(100);
        inByte=Serial.read();
        if(inByte>'0' && inByte<'5')
        {
          channel=inByte-'1';
          EEPROM.write(0x01,channel); 
          char data[50];
          sprintf(data,"Changed active channel to %d\r\n",channel+1);
          Serial.print(data);    
        }
      }
      break;
      case 'i':
      {
        mode=Mode::idle;
      }
      break;
      case 'r':
      {
        initRamp();
        mode=Mode::ramp;
      }
      break;
      case 'p':
      {
        mode=Mode::pulse;
      }
      break;
      case 'f':
      {
        mode=Mode::findZeroAmp;
      }
      break;
      case 'g':
      {
        mode=Mode::calGain;
      }
      break;
      case 's':
      {
        delay(100);
        inByte=Serial.read();
        bool neg=false;
        if(inByte=='-')
        {
          neg=true;
          delay(100);
          inByte=Serial.read();
        }
        if(inByte>='0' && inByte<'9')
        {
          float ampere=inByte-'0';
          if(neg)
            ampere*=-1;
          writeDACValue(AmpereToDAC(ampere),channel);
          char data[50];
          sprintf(data,"Set channel %d to %d A\r\n",channel+1,(int)ampere);
          Serial.print(data);    
        }
      }
      break;      
      case 'q':
      {
        for(unsigned char n=0;n<4;n++)
        {
          char str_temp[20];
          dtostrf(DACToVolt(zeroAmpVal[n]), 10, 7, str_temp);
          char data[50];
          sprintf(data,"channel %d\r\n   zeroAmpVoltage: %s V \r\n",n+1,str_temp);
          Serial.print(data);
          for(unsigned int i=0;i<numCalElements;i++)
          {
            dtostrf(OPAmpVoltageToCurrentFactor[n][i], 10, 7, str_temp);
            sprintf(data,"   cal: 0x%04x -> %s \r\n",calVal[i],str_temp);
            Serial.print(data);
          }
        }
      }
      break;
      case '\n':
      break;
      default:
      Serial.print("unknown command\r\n");  
      delay(100);      
      break;
    }
    EEPROM.put(0x00,mode);  
  }
  
  switch(mode)
  {
    case Mode::idle:
    break;
    case Mode::ramp:
    {
      rampLoop();
      if(rampCyclePause>0)
      {
        writeDACValue(AmpereToDAC(0),channel);      
        delay(rampCyclePause);
      }
    }
    break;
    case Mode::pulse:
    pulseLoop();
    break;
    case Mode::findZeroAmp:
    {
      findZeroAmpVoltage();
      mode=Mode::idle;
    }
    break;
    case Mode::calGain:
    {
      determineGainValues();    
      mode=Mode::idle;
    }
    break;
    default:
    Serial.print("invalid mode\r\n");  
    delay(100);
    break;
  }
}
