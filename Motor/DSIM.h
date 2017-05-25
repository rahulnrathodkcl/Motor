//Version 2 Dated 29052016
#ifndef DSIM_h
#define DSIM_h

#include "Motor_MGR.h"
#include "S_EEPROM.h"
#include "PSIM.h"
#include "Definitions.h"
#include <Arduino.h>

class Motor_MGR;

class DSIM:public PSIM
{
	private:
	
    Motor_MGR* motor1;

	String getHexFileName();
    String getAPN();
    bool makeSMSString(String &responseString,char actionType);
    void operateDTMF(String str);
    void addTextToStatusMsg(String &);
    byte dCheckPrgReq(String &str,bool noMsg);
    void addOperateOnMsg(String &inpString,bool &admin,String &smsString,bool &done,bool &processed);
    bool addCheckNotInCall();
    bool addCheckRejectCommands();
    void addUpdateStringData(String &str);
    void addUpdate();
	void enableRingLevelInterrupt();
	
	public:

#ifndef disable_debug
    #ifndef __AVR_ATmega128__
		#ifdef software_SIM
		    DSIM(HardwareSerial* serial, SoftwareSerial* serial1);
		#else
		    DSIM(SoftwareSerial* serial, HardwareSerial* serial1);
		#endif
	#else
		    DSIM(HardwareSerial* serial, HardwareSerial* serial1);		    
    #endif
#else
    #ifndef __AVR_ATmega128__
		#ifdef software_SIM
		    DSIM(SoftwareSerial* serial);
		#else
		    DSIM(HardwareSerial* serial);
		#endif
	#else
		DSIM(HardwareSerial* serial);
	#endif
#endif

    void setClassReference(S_EEPROM* e1, Motor_MGR* m1);  //derived class public
    void setMotorMGRResponse(char status);		//derivedClass
};
#endif