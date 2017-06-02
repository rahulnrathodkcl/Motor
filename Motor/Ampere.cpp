
//200-16 A3
//120-12 A2
//80-8 A1
//40-4 A0

PIN_A3
PIN_A2
PIN_A1
PIN_AMPERE
overLoadBuffer
noLoadBuffer
savedAmpere

void setRange(byte rangeIndex)
{
	digitalWrite(PIN_A3,LOW);
	digitalWrite(PIN_A2,LOW);
	digitalWrite(PIN_A1,LOW);

	if(rangeIndex==3)
		digitalWrite(PIN_A3,HIGH);
	else if(rangeIndex==2)
		digitalWrite(PIN_A2,HIGH);
	else if(rangeIndex==1)
		digitalWrite(PIN_A1,HIGH);
}

unsigned short int getAmpereReading()
{
	analogRead(PIN_AMPERE);
	unsigned short int temp = millis();
	while(millis()-temp<2)
	{}
	temp = analogRead(PIN_AMPERE);
	return temp;
}

bool autoAmpere()
{
	byte range[4] = {40,80,120,200};
	byte index =3;
	do
	{
		setRange(index);
		// analogRead(PIN_AMPERE);
		// delay(2);
		// unsigned short int t = analogRead(PIN_AMPERE);
		t = getAmpereReading();
		float temp1 = range[--index] * (100 - overLoadBuffer) / 100;
		temp1 = temp1 * 4.0/1024.0;
	}while(t<t1 && index>0);
    eeprom_write_byte((uint8_t *)AUTOAMPADDRESS,index);
}

byte getByteAmpere()
{

}

byte checkAmpere()
{
	unsigned short int amp = getAmpereReading();

	if(amp > (savedAmpere * (100+overLoadBuffer)/100))
	{
		overLoadEventCount++;
		if(overLoadEventCount>overLoadThresoldReadings)
		{
			//registerEvent, as in motorMGr
		}			
	}
	else if (amp < (savedAmpere * (100 - noLoadBuffer)/100))
	{
		noLoadEventCount++;
		if(noLoadEventCount>noLoadThresholdReadings)
		{
			//registerEvent, as in motorMGr
		}			
	}
	else
	{
		noLoadEventCount=overLoadEventCount=0;
	}
}
