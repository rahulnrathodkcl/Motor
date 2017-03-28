#include "Motor_MGR.h"

/*
simEventTemp[0] 	: _motor not started							N
simEventTemp[1] 	: _cannot turn off motor due to some problem	P
simEventTemp[2]		: _motor has turned off due to unknown reason.	U
simEventTemp[3]		: _single phasing has occured,motor off			F
simEventTemp[4]		: _AC power On									G
simEventTemp[5]		: _AC power off									L
simEventTemp[6]		: _motor has turned off due to power cut off.	C
simEventTemp[7]		: _motor has started  							S
simEventTemp[8]		: _motor has turned off							O
simEventTemp[9]		: lost power in 2 phase, motor off				D
simEventTemp[10]	: got AC Power in 1 phase						A
simEventTemp[11]	: got AC power in 2 phase						B
*/

#ifndef disable_debug
  #ifdef software_SIM
    Motor_MGR::Motor_MGR(HardwareSerial *serial,SIM* sim1,S_EEPROM* eeprom1)
    {
      	_NSerial = serial;
      	_NSerial->begin(19200);
		anotherConstructor(sim1,eeprom1);
    }
  #else
    Motor_MGR::Motor_MGR(SoftwareSerial *serial,SIM* sim1,S_EEPROM* eeprom1)
    {
		_NSerial = serial;
		_NSerial->begin(19200);
		anotherConstructor(sim1,eeprom1);
    }
  #endif
#else
  Motor_MGR::Motor_MGR(SIM* sim1,S_EEPROM* eeprom1)
  {
    anotherConstructor(sim1,eeprom1);
  }
#endif

void Motor_MGR::anotherConstructor(SIM* sim1,S_EEPROM* eeprom1)
{
	this->sim1=sim1;
	this->eeprom1=eeprom1;

	pinMode(PIN_START,OUTPUT);
	digitalWrite(PIN_START,HIGH);
	pinMode(PIN_STOP,OUTPUT);
	digitalWrite(PIN_STOP,HIGH);					// to connect stop Relay output to the NC , relay is OFF

	pinMode(PIN_3PHASE,INPUT);
	pinMode(PIN_MFEEDBACK,INPUT_PULLUP);
	pinMode(PIN_ACFEEDBACK,INPUT_PULLUP);	
	pinMode(PIN_ACPHASE,INPUT);

	gotOffCommand=false;
	gotOnCommand=false;
	
	eventOccured=false;
	startTimerOn=false;

	stopTimerTime=600;
	stopTimerOn=false;

	waitCheckACTime=50;
	waitCheckACTimerOn=false;

	singlePhasingTime=70;
	singlePhasingTimerOn=false;

	startSequenceTimerTime=20;
	startSequenceOn=false;

	stopSequenceTimerTime=20;
	stopSequenceOn=false;

	waitStableLineTime=50;
	waitStableLineOn=false;

	allPhase=false;
	mFeedback=false;
	acFeedback=false;
	phaseAC=false;

	for(byte i=0;i<12;i++)
		simEventTemp[i]=true;

	simEvent[0]='N';
	simEvent[1]='P';
	simEvent[2]='U';
	simEvent[3]='F';
	simEvent[4]='G';
	simEvent[5]='L';
	simEvent[6]='C';
	simEvent[7]='S';
	simEvent[8]='O';	
	simEvent[9]='D';	
	simEvent[10]='A';	
	simEvent[11]='B';	
	
	// resetAutoStart();
}

bool Motor_MGR::getMotorState()
{
	bool p1,p2,p3,p4;
	readSensorState(p1,p2,p3,p4);
	// ACPowerState(p4);

	// if(p3 && !phaseAC)
	// 	// battery1->gotACPower();
	// else if(!p3 && phaseAC)
	// 	// battery1->lostACPower();
	// motorState(p2);
	updateSensorState(p1,p2,p3,p4);
	return p2;
}

void Motor_MGR::readSensorState(bool &p1,bool &p2,bool &p3,bool &p4)
{
	eventOccured=false;
	noInterrupts();
	p1=digitalRead(PIN_3PHASE);
	p2=!digitalRead(PIN_MFEEDBACK);
	p3=!digitalRead(PIN_ACFEEDBACK);
	p4=digitalRead(PIN_ACPHASE);
	interrupts();

	#ifndef disable_debug
		_NSerial->print("3P:");
		_NSerial->println(p1);
		_NSerial->print("MF:");
		_NSerial->println(p2);
		_NSerial->print("ACF:");
		_NSerial->println(p3);
		
		_NSerial->print("PAC:");
		_NSerial->println(p4);
	#endif
}

void Motor_MGR::updateSensorState(bool &p1,bool &p2,bool &p3,bool &p4)
{
	allPhase=p1;
	mFeedback=p2;
	acFeedback=p3;
	phaseAC=p4;

	if(!phaseAC || !allPhase)
		startTimerOn=false;

	if(!(bool)eeprom1->AUTOSTART)
	{
		if(phaseAC)
		{
			#ifndef disable_debug
				_NSerial->println("AC ON");
			#endif
			if(!mFeedback)
			{
				#ifndef disable_debug
					_NSerial->println("M OFF");
				#endif
				digitalWrite(PIN_STOP,LOW);
			}
		}
		else
		{
				#ifndef disable_debug
					_NSerial->println("ON BAT");
				#endif
			digitalWrite(PIN_STOP,HIGH);
		}
	}
}

void Motor_MGR::resetAutoStart(bool setChange)
{
	if(!(bool)eeprom1->AUTOSTART && !motorState() && ACPowerState())
    	digitalWrite(PIN_STOP,LOW);
  	else if ((bool)eeprom1->AUTOSTART && !stopSequenceOn)
  	{
    	digitalWrite(PIN_STOP,HIGH);
   		if(setChange)// && eeprom1->ACPowerState() && !eeprom1->motorState())
  		{
  			triggerAutoStart();
  		}
  	}
}

void Motor_MGR::triggerAutoStart()
{
	if(!motorState())
	{
		if(AllPhaseState() && ACPowerState())
		{
	  			startTimerOn=true;
				tempStartTimer=millis();
		}
	}
}

inline bool Motor_MGR::motorState()
{
  return mFeedback;
}

inline void Motor_MGR::motorState(bool b)
{
  mFeedback=b;
}

inline bool Motor_MGR::ACFeedbackState()
{
	return acFeedback;
}

inline void Motor_MGR::ACFeedbackState(bool b)
{
	acFeedback=b;
}

inline bool Motor_MGR::ACPowerState()
{
  return phaseAC;
}

inline void Motor_MGR::ACPowerState(bool b)
{
  phaseAC=b;
}

inline bool Motor_MGR::AllPhaseState()
{
  return allPhase;
}

inline void Motor_MGR::AllPhaseState(bool b)
{
  allPhase=b;
}

void Motor_MGR::operateOnEvent()
{
	bool t3Phase,tMotor,tacFeedback,tacPhase;
	readSensorState(t3Phase,tMotor,tacFeedback,tacPhase);

	if(motorState())		//motorOn
	{
		if(!t3Phase && !tMotor && !tacPhase)	//acPower Cut Off
		{
			waitCheckACTimerOn=false;		//stop any unknown reason of motor off event
			stopMotor();
			simEventTemp[6]=sim1->registerEvent('C'); //report To SIM Motor Off due to POWER CUT OFF
			// _NSerial->println("Got C");
		}
		else if((tacPhase && ACPowerState()) &&
			(t3Phase && AllPhaseState()) && 
			(!tMotor))		//motor switched off due to any reason
		{
			tempWaitCheckACTimer=millis();
			waitCheckACTimerOn=true;
			#ifndef disable_debug
				_NSerial->println("Got U");
			#endif
			//report to SIM Motor Off due to Unknown Reason
			// simEventTemp[2]=sim1->registerEvent('U');
			// stopMotor();
		}
		else if(!t3Phase && tacPhase && mFeedback) ////single phasing occured
		{
			// operateOnSinglePhasing();
			tempSinglePhasingTimer=millis();
			singlePhasingTimerOn=true;
			#ifndef disable_debug
				_NSerial->println("Got F");
			#endif
			semiState=true;
		}
		else if(!mFeedback && (!tacPhase && ACPowerState()) && (t3Phase && AllPhaseState()))	//2 Phases Got Off
		{
			waitCheckACTimerOn=false;		//stop any unknown reason of motor off event
			stopMotor();
			simEventTemp[9] = sim1->registerEvent('D');
			semiState=true;
		}
		// No need to check if single phasing had occured, but all phase present now.
	}
	else				//motorOff
	{	
		if(mFeedback)		// motor turn on manually
		{
			if(t3Phase && tacPhase)
			{
				semiState=false;
				if(startTimerOn)
					startTimerOn=false;
				simEventTemp[7] = sim1->registerEvent('S');	//register To SIM Motor has started
			}
			else
			{
				stopMotor();				
			}
		}
		else	// changes in AC lines, i.e. got power in 3 phase, 2phase or 1 phase, or lost ACpower.
		{
			waitStableLineOn=true;
			waitStableLineTimer=millis();			
		}
	}
	updateSensorState(t3Phase,tMotor,tacFeedback,tacPhase);
}

byte Motor_MGR::checkLineSensors()
{
	if(ACPowerState() && AllPhaseState())		//all phase present
		return AC_3PH;
	else if((ACPowerState() || ACFeedbackState())  && !AllPhaseState())	//2 phase present
		return AC_2PH;
	else if(!ACPowerState() && AllPhaseState())	//1 phase present
		return AC_1PH;
	else if(!ACPowerState() && !AllPhaseState())	//1 phase present
		return AC_OFF;
}

bool Motor_MGR::waitStableLineOver()
{
	return(waitStableLineOn && millis()-waitStableLineTimer>=(waitStableLineTime*100));
}

void Motor_MGR::operateOnStableLine()
{
	waitStableLineOn=false;
	
	byte temp =checkLineSensors();
	if(temp == AC_3PH)
	{
		semiState=false;
		#ifndef disable_debug
			_NSerial->print("AUTO:");
			_NSerial->println(eeprom1->AUTOSTART);
			_NSerial->print("TIME:");
			_NSerial->println(eeprom1->AUTOSTARTTIME);
		#endif
		if(eeprom1->AUTOSTART)			//startMotor if autoStart=ON
			triggerAutoStart();
		else
		{
			if(!eeprom1->DND)	//if DND off then
				simEventTemp[4]= sim1->registerEvent('G'); //register TO SIM AC power ON				
		}
	}
	else if(!semiState && temp==AC_2PH)//Got Power in 2 phase
	{
		semiState=true;
		if(!eeprom1->DND)	//if DND off then
			simEventTemp[11]= sim1->registerEvent('B'); //register TO SIM 2 phase power ON				

	}
	else if(!semiState && temp==AC_1PH)		//Got Power in 1 phase
	{
		semiState=true;
		if(!eeprom1->DND)	//if DND off then
			simEventTemp[10]= sim1->registerEvent('A'); //register TO SIM 1 phase power ON				
	}
	else if(temp==AC_OFF)	//Lost Power in All Phase
	{
		semiState=false;
		if(!eeprom1->DND)		//if DND off
			simEventTemp[5]= sim1->registerEvent('L'); //register To SIM AC Power OFF
	}
}

bool Motor_MGR::startMotorTimerOver()
{
	return (millis()-tempStartTimer>=(((unsigned long int)eeprom1->AUTOSTARTTIME*1000)));
}

bool Motor_MGR::stopMotorTimerOver()
{
	return (millis()-tempStopTimer>=(stopTimerTime*100));
}

// void Motor_MGR::setACPowerState(bool temp)
// {
// 	eeprom1->ACPowerState(temp);
// }

bool Motor_MGR::waitCheckACTimerOver()
{
	return (waitCheckACTimerOn && (millis()-tempWaitCheckACTimer>(waitCheckACTime*100)));
}

void Motor_MGR::unknownMotorOff()
{
	waitCheckACTimerOn=false;
	//report to SIM Motor Off due to Unknown Reason
	stopMotor();
	simEventTemp[2]=sim1->registerEvent('U');
}

bool Motor_MGR::singlePhasingTimerOver()
{
	return (singlePhasingTimerOn && millis()-tempSinglePhasingTimer>(singlePhasingTime*100));
}

inline void Motor_MGR::operateOnSinglePhasing()
{
	stopMotor();
	simEventTemp[3] = sim1->registerEvent('F');
	//reportSinglePhasing TO SIM..
}

void Motor_MGR::startMotor(bool commanded)
{
	#ifndef disable_debug
		_NSerial->print("Start");
		_NSerial->println("Motor ");
	#endif
	if(ACPowerState() && AllPhaseState())
	{
		if(!motorState())
		{
			startTimerOn=false;
			if(!(bool)eeprom1->AUTOSTART)
				digitalWrite(PIN_STOP,HIGH);
			digitalWrite(PIN_START,LOW);
			tempStartSequenceTimer=millis();
			startSequenceOn=true;
			motorState(true);
			gotOnCommand=commanded;
		}
		else
		{
			if(commanded)
				sim1->setMotorMGRResponse('O');
			#ifndef disable_debug
				_NSerial->print("Motor ");
				_NSerial->println("ON");
			#endif
		}
	}
	else
	{
		if(commanded)
			sim1->setMotorMGRResponse('L');	//cannot start motor due to some problem
	}
}
	
void Motor_MGR::stopMotor(bool commanded,bool forceStop)
{
	#ifndef disable_debug
		_NSerial->print("Stop");
		_NSerial->println("Motor ");
	#endif
	if(forceStop || motorState())
	{
		startTimerOn=false;
		singlePhasingTimerOn=false;
		digitalWrite(PIN_STOP,LOW);
		tempStopSequenceTimer=millis();
		stopSequenceOn=true;
		motorState(false);
		gotOffCommand=commanded;
	}
	else
	{
		if(commanded)
			sim1->setMotorMGRResponse('O');
		#ifndef disable_debug
			_NSerial->print("Motor ");
			_NSerial->println("OFF");
		#endif
	}
}

void Motor_MGR::terminateStopRelay()
{
	if(stopSequenceOn && millis() - tempStopSequenceTimer > (stopSequenceTimerTime*100))	
	{
		if((bool)eeprom1->AUTOSTART || !ACPowerState())
			digitalWrite(PIN_STOP,HIGH);
		
		stopSequenceOn=false;

		#ifndef disable_debug
			_NSerial->print("Stop");
			 _NSerial->println("Over");
		#endif
		
		if(!getMotorState())		//motor has turned off
		{
			if(gotOffCommand)
			{
				gotOffCommand=false;
				sim1->setMotorMGRResponse('D');		//motor has stopped
			}
				// else
				// simEventTemp[8] = sim1->registerEvent('O'); //register TO SIM motor has turned off
		}	
		else	//motor is still on, with either phase 1 or phase 2 feedback on with ACPhase on
		{
			if(gotOffCommand)
			{
				gotOffCommand=false;
				sim1->setMotorMGRResponse('L');		//cannot turn off motor
			}	
			else
				simEventTemp[1] = sim1->registerEvent('P');
			//register to SIM cannot turn off motor due to some problem
		}
	}
}

void Motor_MGR::terminateStartRelay()
{
	if(startSequenceOn &&  millis() - tempStartSequenceTimer > (startSequenceTimerTime*100))
	{
		digitalWrite(PIN_START,HIGH);
		startSequenceOn=false;

		#ifndef disable_debug
			_NSerial->print("Start");
			 _NSerial->println("Over");
		#endif

		if(gotOnCommand)
		{
			gotOnCommand=false;
			if(getMotorState())
			{
				sim1->setMotorMGRResponse('D'); // motor has started
			}
			else
			{
				stopMotor(false,true);
				sim1->setMotorMGRResponse('L');	//cannot start motor due to some problem
			}
		}
		else
		{
			if(getMotorState())
				simEventTemp[7] = sim1->registerEvent('S');// ;//register To SIM Motor has started
			else
			{
				stopMotor(false,true);
				simEventTemp[0] = sim1->registerEvent('N');//register To SIM motor not started due to phase failure
			}			
		}
	}
}

void Motor_MGR::statusOnCall()
{
	bool temp = getMotorState();
	if(temp)
		sim1->setMotorMGRResponse('D');	//motor is on
	else
	{
		byte b= checkLineSensors();

		if(b==AC_OFF)
			sim1->setMotorMGRResponse('L');	//motor off, no light
		else if(b==AC_1PH)	// power only in 1 phase
			sim1->setMotorMGRResponse('A');	//motor off, no light
		else if(b==AC_2PH)	//power only in 2 phase
			sim1->setMotorMGRResponse('B');	//motor off, no light
		else if(b==AC_3PH)
			sim1->setMotorMGRResponse('O');	//motor off, light on			
	}
}

void Motor_MGR::SIMEventManager()
{
	for(byte i=0;i<12;i++)
	{
		if(!simEventTemp[i])
			simEventTemp[i]= sim1->registerEvent(simEvent[i]);
	}
}
void Motor_MGR::update()
{
	if(!startSequenceOn && !stopSequenceOn && eventOccured)
		operateOnEvent();

	if(waitStableLineOn)
		if(waitStableLineOver())
			operateOnStableLine();

	if(waitCheckACTimerOn)
		if(waitCheckACTimerOver())
			unknownMotorOff();

	if(singlePhasingTimerOn)
		if(singlePhasingTimerOver())
			operateOnSinglePhasing();

	if(startTimerOn)
		if(startMotorTimerOver())
			startMotor();
	
	if(stopTimerOn)
		if(stopMotorTimerOver())
			stopMotor();

	if(startSequenceOn)
		terminateStartRelay();

	if(stopSequenceOn)
		terminateStopRelay();

	SIMEventManager();
}
