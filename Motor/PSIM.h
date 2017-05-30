
#ifndef PSIM_h
#define PSIM_h

#include <Arduino.h>

#include "S_EEPROM.h"
#include "Definitions.h"

class PSIM
{
	protected:
	    S_EEPROM* eeprom1;
	    char currentOperation;
	    bool isMsgFromAdmin;
    	
	    void subDTMF();
		void playSound(char c, bool x = true);
	    void delay(byte);

	    bool sendBlockingATCommand(String,bool =false);
	    String readString();
	    bool matchString(String, String);
	    bool stringContains(String &sstr, String mstr, byte sstart, byte sstop);
	    bool isAdmin(String str);
	    bool isNumeric(String &str);

	    bool isNumber(String &str);
	    bool checkNumber(String);
	    bool getBlockingResponse(String &cmd,bool (PSIM::*func)(String &));
		    
	    char callState(String);
	    void stopSound();


	#ifndef disable_debug
    	#ifndef __AVR_ATmega128__
			#ifdef software_SIM
			    HardwareSerial* _NSerial;
			    SoftwareSerial* _SSerial;
			#else
			    SoftwareSerial* _NSerial;
			    HardwareSerial* _SSerial;
			#endif
		#else
			HardwareSerial* _NSerial;
			HardwareSerial* _SSerial;
    	#endif
	#else
    	#ifndef __AVR_ATmega128__
			#ifdef software_SIM
			    SoftwareSerial* _SSerial;
			#else
			    HardwareSerial* _SSerial;
			#endif
		#else
			    HardwareSerial* _SSerial;
		#endif
	#endif

  private:

    // bool responseToAction;

    byte soundWaitTime; //x100 = mSec
    unsigned long soundWait;
    bool bplaySound;
    char playFile;
    bool inCall;

    //bool starPresent;

    unsigned short int callCutWaitTime;  //x100 = mSec
    unsigned long callCutWait;

    bool sendCUSDResponse;

    bool commandsAccepted;
    unsigned short  acceptCommandsTime;
    unsigned long tempAcceptCommandTime;

    char currentStatus;
    char currentCallStatus;

    byte nr;
    bool callAccepted;

    // bool makeResponse;
    char actionType;

    bool freezeIncomingCalls;

    bool obtainNewEvent;
    unsigned long obtainEventTimer;


    void anotherConstructor();

    void delAllMsg();
    void sendReadMsg(String str);
    bool isMsgBody(String &str);
    // virtual bool isPrimaryNumber(String &str) = 0;
    // bool isPrimaryNumber(String str);	//virtual	use in gotMsgBody
    void gotMsgBody(String &str);
    bool isNewMsg(String &str);

    // virtual byte checkExists(String str) = 0;
    //bool checkExists(String) virtual to be used in checkNumber

    void stopCallWaiting();
    void acceptCommands();
    void rejectCommands();
    bool extendedSendCommand(String cmd,byte timeout);

    bool startGPRS(String);
    bool stopGPRS();
    
    // bool extendedSendCommand(String &cmd,String vstr,unsigned short int len,unsigned short int timeout);
    bool connectToFTP(String);
    bool setFile(String);
    virtual String getHexFileName() = 0;
	// void saveProgramSize(unsigned long int temp);

    bool getProgramSize(); //add variable programSizeSet as local variable //also some other changes needed 
    bool downloadFirmware();		//needs change for eeprom
    bool isGPRSConnected();
    bool prepareForFirmwareUpdate(String &ftpServerName);
    virtual String getAPN() = 0;
    
   	bool isCUSD(String &str);
   	bool isCBC(String &);
   	bool isCSQ(String &);
    bool isRinging(String);
    bool isDTMF(String &str);
    bool isCut(String);
    bool isSoundStop(String);

    // virtual String getActiveNumber() = 0; //virtual
    void makeCall();
    void endCall();
    void acceptCall();
    virtual bool makeSMSString(String &responseString,char actionType) = 0;
    void sendSMS(String, bool);
    virtual void operateDTMF(String str) = 0;		//virtual protected function
    void operateRing();
    bool playSoundElligible();
    void triggerPlaySound();
    // void playSoundAgain(String);

    bool callTimerExpire();
    // virtual byte getResponseSetting() = 0;		//return eeprom1->RESPONSE
    void makeResponseAction();
    bool rejectCommandsElligible();
    void checkNetwork(String);//(String str);
    String makeStatusMsg(byte batPer,byte network);  //add option to add Custom Data in derived class, so define new virtual function , which can add data to STATUs msg.
    virtual void addTextToStatusMsg(String &) = 0;

    bool checkEventGone();
    // void networkCounterMeasures();
    // void __attribute__((noinline)) watchdogConfig(uint8_t x);
    // static inline void watchdogReset();

    bool checkPrgReq(String,bool nomsg);//add other virtual function, to add any Derived class specific functionalities.
    virtual byte dCheckPrgReq(String &str,bool noMsg) = 0;
    // void updateFirmware(bool temp,bool verify);
    void initRestartSeq(); // define updateFirmware in this class
    virtual void jumpToBootloader() __attribute__ ((naked));
    void setObtainEvent();
    inline bool isCallReady(String);
    void checkRespSMS(char t1);	//add eeprom RESPONSE read function



    virtual void addOperateOnMsg(String &inpString,bool &admin,String &smsString,bool &done,bool &processed) = 0;
    virtual bool addCheckNotInCall() = 0;
    virtual bool addCheckRejectCommands() = 0;
    virtual void addUpdateStringData(String &str)=0;
    virtual void addUpdate() =0 ;
    virtual void enableRingLevelInterrupt()=0;


  public:

    bool initialized;
    // -- Used with interrupt RING
    bool inInterrupt;
    unsigned long int tempInterruptTime;
    //---



#ifndef disable_debug
    #ifndef __AVR_ATmega128__
		#ifdef software_SIM
		    PSIM(HardwareSerial* serial, SoftwareSerial* serial1);
		#else
		    PSIM(SoftwareSerial* serial, HardwareSerial* serial1);
		#endif
	#else
		    PSIM(HardwareSerial* serial, HardwareSerial* serial1);		    
    #endif
#else
    #ifndef __AVR_ATmega128__
		#ifdef software_SIM
		    PSIM(SoftwareSerial* serial);
		#else
		    PSIM(HardwareSerial* serial);
		#endif
	#else
		PSIM(HardwareSerial* serial);
	#endif
#endif

    void startSIMAfterUpdate();
    void sendUpdateStatus(byte);
    void sendCommand(char cmd, bool newline);
    void sendCommand(String cmd, bool newline);

    bool initialize();

    bool registerEvent(char eventType);

    void operateOnMsg(String str, bool admin,bool noMsg);	//add virtual function call, and define it, so that Derived class can add functionality

    // void setCallBackFunctions(void (*ImdEvent)(bool));
    bool busy();
    // void setNetLight(byte);

    bool checkSleepElligible();
    bool checkNotInCall();		//add virtual function to add functionality check by Derived Class, will work for checkSleepElligible() also.
    void update();

    //add virtual function to allow rejecting COmmands, i.e. sleeping.
    //add virtual function to operate on Serial Data in update()
    // add virtual function to operate on Derived fucntionality in update() 
};
#endif