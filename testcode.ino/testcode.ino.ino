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

enum class Mode : char{idle,ramp,pulse,findZeroAmp,calGain, read};

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
//unsigned int calVal[]={0x5000, 0x6800, 0x8000, 0x9800, 0xB000};
//unsigned int calVal[numCalElements]={0x0000, 0x1000, 0x2000, 0x3000, 0x4000, 0x5000, 0x6000, 0x7000, 0x8000, 0x9000, 0xA000, 0xB000, 0xC000, 0xD000,0xE000, 0xF000,0xFFFF};
unsigned int calVal[numCalElements]={0x0000, 0x0800, 0x1000, 0x1800, 0x2000, 0x2800, 0x3000, 0x3800, 0x4000, 0x4800, 0x5000, 0x5800, 0x6000, 0x6800, 0x7000, 0x7800, 0x8000,
  0x8800, 0x9000, 0x9800, 0xA000, 0xA800, 0xB000, 0xB800, 0xC000, 0xC800, 0xD000,0xD800, 0xE000, 0xE800, 0xF000, 0xF800, 0xFFFF};
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

void initDAC()
{
  byte dataToSend[3];
  dataToSend[0]=0x03; // config register
  dataToSend[1]=0x01; // disable internal reference
  dataToSend[2]=0x00;
  digitalWrite(SS,LOW); 
  SPI.transfer(dataToSend,3);
  digitalWrite(SS,HIGH);   
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

  /*
  // not necessary, is already default
  dataToSend[0]=0x05; // set all channels to power up state
  dataToSend[1]=0x00;
  dataToSend[2]=0x00;

  digitalWrite(SS,HIGH); 
  delay(10);
  SPI.transfer(dataToSend,3);
  delay(10);
  digitalWrite(SS,LOW);  
  */
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
  initDAC();
 
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

float DACToVolt(const unsigned int val)
{
  return static_cast<float>(val)/static_cast<float>(0xFFFF)*5.0;
}

float DACToAmpere(const unsigned int val)
{
  unsigned int leftIndex=0;
  unsigned int rightIndex=numCalElements-1;
  calVal[(numCalElements-1)/2]=zeroAmpVal[channel];
  for(unsigned int n=0;n<numCalElements;n++)
  {
    if(calVal[n]<=val)
      leftIndex=n;
    if(calVal[n]>=val)
    {
      rightIndex=n;
      break;
    }
  }  
  if(leftIndex==rightIndex)
    return OPAmpVoltageToCurrentFactor[channel][leftIndex];
  return (OPAmpVoltageToCurrentFactor[channel][rightIndex]-OPAmpVoltageToCurrentFactor[channel][leftIndex])/(calVal[rightIndex]-calVal[leftIndex])
    *(val-calVal[leftIndex])+OPAmpVoltageToCurrentFactor[channel][leftIndex];
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
  if(leftIndex==rightIndex)
    return calVal[leftIndex];
  return (calVal[rightIndex]-calVal[leftIndex])/(OPAmpVoltageToCurrentFactor[channel][rightIndex]-OPAmpVoltageToCurrentFactor[channel][leftIndex])
    *(ampere-OPAmpVoltageToCurrentFactor[channel][leftIndex])+calVal[leftIndex];
}

void writeDACValue(const unsigned int val)
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

unsigned int readADC()
{
  byte dataToSend[4];
  dataToSend[0]=0xC0 | (channel << 2);
  dataToSend[1]=0x00;
  dataToSend[2]=0x00;
  dataToSend[3]=0x00;
  digitalWrite(SS,HIGH); 
  SPI.transfer(dataToSend,4);
  digitalWrite(SS,LOW); 
  dataToSend[0]=0xC0;
  dataToSend[1]=0x00;
  dataToSend[2]=0x00;
  dataToSend[3]=0x00;  
  digitalWrite(SS,HIGH); 
  SPI.transfer(dataToSend,4);
  digitalWrite(SS,LOW);  
  return static_cast<unsigned int>(dataToSend[2])<<8 | static_cast<unsigned int>(dataToSend[3]);  
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
    writeDACValue(Vhigh);
  else
    writeDACValue(Vlow);

  delay(1);
  /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
  const unsigned int ADCCode = readADC();
  char str_temp[20];
  dtostrf(ADCToAmpere(ADCCode), 10, 7, str_temp);
  if(displayHexValues)
    sprintf(data,"%d\t%04x %s A\t\r\n",loopCount,ADCCode,str_temp);
  else
    sprintf(data,"%d\t%s A\r\n",loopCount,str_temp);
  Serial.print(data);
}

unsigned int rampVal = 0;
static unsigned int rampLoopCount = 1;


void initRamp()
{
  rampLoopCount = 1;
  rampVal = AmpereToDAC(rampMinAmp);
  char data[100];
  sprintf(data,"RAMP BEGIN\tchannel %i\r\n",channel+1);
  Serial.print(data);        
  sprintf(data,"DAC code\tcalculated current [A]\tADC code\tmeasured current [A]\r\n");
  Serial.print(data);

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
    {
      sprintf(data,"RAMP END\r\n");
      Serial.print(data);      
      mode = Mode::idle;
      return;
    }
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
    writeDACValue(rampVal);
  }
  delay(1); // wait 1 ms

  char str_temp[20];
  const unsigned int ADCVal= readADC();
  dtostrf(ADCToAmpere(ADCVal), 10, 7, str_temp);
  char str_temp2[20];
  dtostrf(DACToAmpere(rampVal), 10, 7, str_temp2);
  sprintf(data,"0x%04x\t%s\t0x%04x\t%s\r\n",rampVal,str_temp2,ADCVal,str_temp);
  Serial.print(data);
}

void determineGainValues()
{
  char data[100];
  char str_temp[20];
  byte dataToSend[4];

  calVal[(numCalElements-1)/2]=zeroAmpVal[channel];

  sprintf(data,"DAC code\toutput current [A]\r\n");
  Serial.print(data);
  for(unsigned int loopCount=0;loopCount<numCalElements;loopCount++)
  {
    writeDACValue(calVal[loopCount]);
    delayMicroseconds(100);
    float ampere = ADCToAmpere(readADC());
    OPAmpVoltageToCurrentFactor[channel][loopCount]=ampere;
    dtostrf(OPAmpVoltageToCurrentFactor[channel][loopCount], 10, 7, str_temp);
    sprintf(data,"0x%02x%02x\t%s\r\n",(byte)(calVal[loopCount]>>8),(byte)calVal[loopCount],str_temp);
    Serial.print(data);
  }
  saveCalibration();
  writeDACValue(AmpereToDAC(0));
}


static unsigned int currentDACCode = 0x8000;
static unsigned int lastDACCode=0;
  
void initFindZeroAmpVoltage()
{
  currentDACCode = 0x8000;
  lastDACCode = 0;
}

/*
 * This routine only functions properly if the coil is connected all the time.
 */
void findZeroAmpVoltageLoop()
{
  char data[100];
  char str_temp2[20];
  char str_temp[20];
  
  writeDACValue(currentDACCode);    
  delay(1);
  const float amp = ADCToAmpere(readADC());
  const float dacVolt = DACToVolt(currentDACCode);
  dtostrf(dacVolt, 10, 7, str_temp2);      
  dtostrf(amp, 10, 7, str_temp);
  sprintf(data,"0x%04x %s V -> %s A\r\n",currentDACCode,str_temp2,str_temp);
  Serial.print(data);

  const unsigned int temp=currentDACCode;
  if(amp>0)
    currentDACCode--;
  else
    currentDACCode++;

  if(currentDACCode==lastDACCode)
  {
    zeroAmpVal[channel] = currentDACCode;
    EEPROM.put(0x00+((channel+1)<<4),zeroAmpVal[channel]);
    Serial.print("Zero current voltage found\r\n");
    saveCalibration();  
    mode=Mode::idle;
  }
  else
    lastDACCode=temp;
}

void readLoop()
{
  char str_temp[20];
  const unsigned int ADCCode= readADC();
  dtostrf(ADCToAmpere(ADCCode), 10, 7, str_temp);
  char data[100];
  sprintf(data,"0x%04x %s\r\n",ADCCode,str_temp);  
  Serial.print(data);
  delay(1);
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
        initFindZeroAmpVoltage();
      }
      break;
      case 'g':
      {
        mode=Mode::calGain;
      }
      break;
      case 'd':
      {
        mode=Mode::read;
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
        if(inByte>='0' && inByte<='9')
        {
          float ampere=inByte-'0';
          if(neg)
            ampere*=-1;
          writeDACValue(AmpereToDAC(ampere));
          char data[50];
          sprintf(data,"Set channel %d to %d A\r\n",channel+1,(int)ampere);
          Serial.print(data);    
        }
      }
      break;      
      case 'q':
      {
        char data[50];
        sprintf(data,"active channel %d\r\n",channel+1);
        Serial.print(data); 
        for(unsigned char n=0;n<4;n++)
        {
          char str_temp[20];
          dtostrf(DACToVolt(zeroAmpVal[n]), 10, 7, str_temp);
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
      case 'x':
      {
        writeDACValue(0xFFFF); 
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
        writeDACValue(AmpereToDAC(0));      
        delay(rampCyclePause);
      }
    }
    break;
    case Mode::pulse:
    {
      pulseLoop();
    }
    break;
    case Mode::findZeroAmp:
    {
      findZeroAmpVoltageLoop();
    }
    break;
    case Mode::read:
    {
      readLoop();    
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
