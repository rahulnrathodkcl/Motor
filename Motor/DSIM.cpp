#include "DSIM.h"

#ifndef disable_debug
	#ifndef __AVR_ATmega128__
		#ifdef software_SIM
			DSIM::DSIM(HardwareSerial* serial, SoftwareSerial* serial1):PSIM(serial,serial1)
			{}
		#else
			DSIM::DSIM(SoftwareSerial* serial, HardwareSerial* serial1):PSIM(serial,serial1)
			{}
		#endif
	#else
		DSIM::DSIM(HardwareSerial* serial, HardwareSerial* serial1):PSIM(serial,serial1)
		{}
	#endif
#else
	#ifndef __AVR_ATmega128__
		#ifdef software_SIM
		DSIM::DSIM(SoftwareSerial* serial):PSIM(serial)
		{}
		#else
		DSIM::DSIM(HardwareSerial* serial):PSIM(serial)
		{}
		#endif
	#else
		DSIM::DSIM(HardwareSerial* serial):PSIM(serial)
		{}
	#endif
#endif

String DSIM::getHexFileName()
{
	return F("m.hex");
}

String DSIM::getAPN()
{
	return F("bsnlnet");
}


// String DSIM::getActiveNumber()
// {
// 	return eeprom1->getActiveNumber();
// }

void DSIM::enableRingLevelInterrupt()
{
  
}

bool DSIM::makeSMSString(String &responseString,char actionType)
{
	switch (actionType)
    {
      case 'S':
        responseString = STR_MOTOR;
        responseString += STR_ON;
        break;
      case 'O':
      case 'U':
      case 'C':
      case 'F':
        responseString = STR_MOTOR;
        responseString += STR_OFF;
        break;
      default:
        return false;
    }
    return true;
}

void DSIM::operateDTMF(String str)
{
  if (str == "1") //Motor On
  {
    currentOperation = 'S';
    subDTMF();
    motor1->startMotor(true);
  }
  else if (str == "2") //Motor Off
  {
    currentOperation = 'O';
    subDTMF();
    motor1->stopMotor(true);
  }
  else if (str == "3") //Status
  {
    currentOperation = 'T';
    subDTMF();
    motor1->statusOnCall();
  }
  // else if (str == "4") //Set AUTOTIMER ON
  // eeprom1->saveAutoStartSettings(true);  //set AutoStart to True in EEPROM
  // else if (str == "5") //Set AUTOTIMER OFF
  // eeprom1->saveAutoStartSettings(false);  //set AUtoStart to False in EEPROM
}

// byte DSIM::getResponseSetting()		//return eeprom1->RESPONSE
// {
// 	return eeprom1->RESPONSE;
// }

void DSIM::addTextToStatusMsg(String &resp)
{
	   byte t3 = motor1->checkLineSensors();
      bool t5 = motor1->motorState();

      if (eeprom1->AUTOSTART)
        resp = F("AUTOON");
      else
        resp = F("AUTOOFF");

      resp = resp + "\nAC:";
      resp = resp + (t3==AC_3PH ? F(" ON\n") : (t3==AC_2PH ? F(" 2 PHASE\n") : F(" OFF\n")));
      resp = resp + F("Motor:");
      resp = resp + (t5 ? F(" ON\n") : F(" OFF\n"));
}

byte DSIM::dCheckPrgReq(String &str,bool noMsg)
{
	// str="0";		
	return 0;
}

void DSIM::setClassReference(S_EEPROM* e1, Motor_MGR* m1)
{
  eeprom1 = e1;
  motor1 = m1;
}


void DSIM::setMotorMGRResponse(char response)
{
  if (currentOperation == 'S') //start Motor
  {
    // responseToAction = true;
    if (response == 'L')
      playSound('N');  //cannot start motor
    else if (response == 'O')
      playSound('1');  //motor is already on
    else if (response == 'D')
      playSound('S');  //motor has started
    // endCall();
  }
  else if (currentOperation == 'O') //switch off motor
  {
    // responseToAction = true;
    if (response == 'L')
      playSound('P');    //cannot stop motor
    else if (response == 'O')
      playSound('2');  //motor is already off
    else if (response == 'D')
      playSound('O');  //motor has stopped
    // endCall();
  }
  else if (currentOperation = 'T')
  {
    // responseToAction = true;
    if (response == 'L') //motor off, no light
      playSound('L');
    else if (response == 'A') //motor off, light on
      playSound('A');
    // else if (response == 'B') //motor off, light on
      // playSound('B');
    else if (response == 'O') //motor off, light on
      playSound('3');
    else if (response == 'D')
      playSound('1');  //motor is on
  }
}

bool DSIM::addCheckNotInCall()
{
	return true;			//when not in call
}

bool DSIM::addCheckRejectCommands()
{
	return !motor1->ACPowerState();
}

void DSIM::addUpdateStringData(String &str)
{
	return;
}

void DSIM::addUpdate()
{
	return;
}

void DSIM::addOperateOnMsg(String &str,bool &admin,String &tempStr,bool &done,bool &processed)
{
	unsigned short data;

    if (str.startsWith(F("DEFAULT"))) //stringContains(str, F("DEFAULT"), 7, str.length() - 1))
    {
      eeprom1->saveAutoStartSettings(false);
      eeprom1->saveDNDSettings(false);
      eeprom1->saveResponseSettings('C');
      eeprom1->saveAutoStartTimeSettings(50);
      done=true;
    }
    else if (str.startsWith(F("AUTOON")))// stringContains(str, F("AUTOON"), 6, str.length() - 1))
    {
      eeprom1->saveAutoStartSettings(true);  //set AutoStart to True in EEPROM
      motor1->resetAutoStart(true);
      done=true;
    }
    else if (str.startsWith(F("AUTOOFF"))) //stringContains(str, F("AUTOOFF"), 7, str.length() - 1))
    {
      eeprom1->saveAutoStartSettings(false);  //set AUtoStart to False in EEPROM
      motor1->resetAutoStart(true);
      done=true;
    }
    else if (stringContains(str, F("AUTOTIME"), 8, str.length() - 1))
    {
      if (isNumeric(str))
      {
        data = str.toInt();
        if (data < 50) data = 50;
        if (data > 480) data = 480;
        eeprom1->saveAutoStartTimeSettings(data);  //Store in EEPROM the AUTO START TIME
        tempStr = F("AUTOTIME");
        tempStr = tempStr + data;
        done=true;
      }
    }
}