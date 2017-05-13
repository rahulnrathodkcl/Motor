#include <EEPROM.h>
const int ampSetAdd=220;
unsigned short int AMPSET;
unsigned short int OVERLOADPER=125;
unsigned short int NOLOADPER=60;
#define PIN_AMP A0

void setup() {
  // put your setup code here, to run once:
Serial.begin(19200);
EEPROM.get(ampSetAdd,AMPSET);
}

unsigned short int currrentReading()
{
  unsigned short int j=analogRead(PIN_AMP);
  delay(2);
  j=analogRead(PIN_AMP);
  Serial.print("AMP:");
  Serial.print(j);


  j=j+143;
  Serial.print("\tAMP(+0.7):");
  Serial.print(j);


  float t=j*5.0/1024.0;
  float a = t*28.0 / 4.0;

  Serial.print("\tV:");
  Serial.print(t);

  Serial.print("\tA:");
  Serial.println(a);

  if(AMPSET==0xFF) AMPSET = j;
  return j;
}

bool checkAmpere()
{
  float temp = AMPSET * OVERLOADPER / 100.0;
  short o = temp;
  temp = AMPSET * NOLOADPER / 100.0;
  short n = temp;

  // Serial.print("O:");
  // Serial.println(o);
  // Serial.print("N:");
  // Serial.println(n);

  float j = currrentReading();

  if(j<n)
  {
    Serial.println("Free");
    return true;
  }

  if(j>o  && j>243)
    return true;
  
  return false;
}

bool getReading=true;
unsigned long int lastReadingTime=0;
void loop() {
  // put your main code here, to run repeatedly:
  if(millis()-lastReadingTime>500 && getReading)
  {
    lastReadingTime=millis();
     bool b=checkAmpere();
     if(b)
      Serial.println("TRIP");
     // else
      // Serial.println("NO TRIP");
  }


  if(Serial.available())
  {
    String str = Serial.readStringUntil('\n');
    str.toUpperCase();

    if(str.startsWith("SET"))
    {
      AMPSET=currrentReading();
      EEPROM.put(ampSetAdd,AMPSET);
    }
    else if(str=="READ\r")
    {     
      Serial.println(AMPSET);
    }

  }
}
