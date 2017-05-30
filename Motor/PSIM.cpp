//Version 2 Dated : 29052016
#include "PSIM.h"

/*
  Gets the Phone Numbers From the EEPROM Class.
*/
#ifndef disable_debug
  #ifndef __AVR_ATmega128__
    #ifdef software_SIM
    PSIM::PSIM(HardwareSerial* serial, SoftwareSerial* serial1)
    {
      _NSerial = serial;
      _NSerial->begin(19200);
      _SSerial = serial1;
      _SSerial->begin(19200);
      anotherConstructor();
    }
    #else
    PSIM::PSIM(SoftwareSerial* serial, HardwareSerial* serial1)
    {
      _NSerial = serial;
      _NSerial->begin(19200);
      _SSerial = serial1;
      _SSerial->begin(19200);
      anotherConstructor();
    }
    #endif
  #else
    PSIM::PSIM(HardwareSerial* serial, HardwareSerial* serial1)
    {
      _NSerial = serial;
      _NSerial->begin(19200);
      _SSerial = serial1;
      _SSerial->begin(19200);
      anotherConstructor();
    }    
  #endif
#else
  #ifndef __AVR_ATmega128__
    #ifdef software_SIM
    PSIM::PSIM(SoftwareSerial* serial)
    {
      _SSerial = serial;
      _SSerial->begin(19200);
      anotherConstructor();
    }
    #else
    PSIM::PSIM(HardwareSerial* serial)
    {
      _SSerial = serial;
      _SSerial->begin(19200);
      anotherConstructor();
    }
    #endif  
  #else
    PSIM::PSIM(HardwareSerial* serial)
    {
      _SSerial = serial;
      _SSerial->begin(19200);
      anotherConstructor();
    }    
  #endif
#endif


void PSIM::anotherConstructor()
{
  // adminNumber = "7041196959";
  initialized = false;
  inCall=false;
  // balStr.reserve(12);

  acceptCommandsTime = 200;
  commandsAccepted = false;

  // pinMode(PIN_RING,INPUT);
  pinMode(PIN_DTR, OUTPUT);
  rejectCommands();

  soundWaitTime = 5;
  bplaySound = false;

  // callDialled = false;
  // callAlerted = false;
  // attemptsToCall=0;

  actionType = 'N';
  // makeResponse = false;
  // responseToAction = false;

  callCutWaitTime = 580;
  nr = 0;
  currentStatus = 'N';
  currentCallStatus = 'N';
  callAccepted = false;
  freezeIncomingCalls = false;
  obtainNewEvent = true;
  isMsgFromAdmin = false;
  // acceptCommands();
}

void PSIM::startSIMAfterUpdate()
{
  sendBlockingATCommand(F("AT+CFUN=1,1\r\n"));
}

void PSIM::sendUpdateStatus(byte updateStatus)
{
  String promptStr;

    if(updateStatus>0 && updateStatus!=0xFF)
    {
      if(updateStatus==0x01)
        promptStr=F("UPDSUCC");
      else if(updateStatus==0x02)
        promptStr=F("CERR");
      else if(updateStatus==0x03)
        promptStr=F("MAXCEDD");
    
      #ifndef disable_debug
        _NSerial->println(promptStr);
      #endif
      isMsgFromAdmin=true;
      sendSMS(promptStr,true);
    }
}

inline void PSIM::delAllMsg()
{
  sendBlockingATCommand(F("AT+CMGDA=\"DEL ALL\"\r\n"));
}

bool PSIM::isNumeric(String &str)
{
  for (byte i = 0; i < str.length(); i++)
  {
    if ( !isDigit(str.charAt(i)))
      return false;
  }
  return true;
}

// bool PSIM::connectToServer(String serverIpAddress)
// {
//   String cmd="AT+CIPSTART=\"TCP\",\"";
//   cmd=cmd+serverIpAddress;
//   cmd = cmd + "\",80";

//   unsigned long int temp = millis();
//   if(sendBlockingATCommand(cmd))
//   {
//     while(millis()-temp<15000)
//     {
//         cmd=readString();
//         if(cmd=="CONNECT OK\r")
//           return true;
//     }
//   }
//   return false;
// }

// bool PSIM::sendHTTPRequest(String req)
// {
//   bool gotresp=false;
//   unsigned long int temp = sizeof(req);
//   String cmd="AT+CIPSEND=" + temp;
//   temp=millis();
//   sendCommand(cmd);
//   while(millis()-t<2000)
//   {
//       cmd=readString();
//       if(cmd==">\r")
//       {
//         gotresp=true;
//         break;
//       }
//   }

//   if(gotresp)
//   {
//     sendCommand(req);
//     return true;
//   }
//   else
//     return false;
// }


// byte PSIM::getProgInfo(String fileName)
// {
//   String cmd="GET ";
//   cmd=cmd+fileName;
//   cmd=cmd + " ";
//   cmd = cmd + "HTTP/1.0";
//   byte retValue;

//   if(sendHTTPRequest(cmd))
//   {
//     unsigned long int temp = millis();
//     bool gotStartSymbol=false;
//     bool gotStopSymbol=false;
//     while(millis()-temp<20000)
//     {
//       if(_SSerial->available())
//       {
//         cmd=_SSerial->readStringUntil('\n');
//         if(cmd=="START\r")
//         {
//           temp=millis();
//           gotStartSymbol=true;
//           continue;
//         }
//         else if(cmd=="STOP\r")
//         {
//             gotStopSymbol=true;
//             break;
//         }
//         if(gotStartSymbol)
//         {        
//           if(stringContains(newData, "MSIGN", 5, str.length() - 1))
//           {
//               if(stringContains(newData,"MEGA328P",8,str.length()-1))
//               {
//                   if(!eeprom1->checkSign(MS_MEGA328P))
//                     return INVALID_SIGN;
//               }
//               else if(stringContains(newData,"MEGA128A",8,str.length()-1))
//               {
//                   if(!eeprom1->checkSign(MS_MEGA128A))
//                       return INVALID_SIGN;
//               }
//           }
//           else if(stringContains(newData, "PROGSIZE", 8, str.length() - 1))
//           { 
//               unsigned long int temp = newData.toInt();
//               if(temp<=0)
//                 return INVALID_SIZE;
//               eeprom1->saveProgSize(temp);
//           }
//         }
//       }
//     }
//     return (INVALID_SIZE+INVALID_SIGN);
//   }
//   return 0;
// }

// bool PSIM::getFile(byte fileType, String fileName,String &firmwareFile)
// {
//   String cmd="GET ";
//   cmd=cmd+fileName;
//   cmd=cmd + " ";
//   cmd = cmd + "HTTP/1.0";

//   if(sendHTTPRequest(cmd))
//   {
//     unsigned long int temp = millis();
//     bool gotStartSymbol=false;
//     bool gotStopSymbol=false;
//     while(millis()-temp<20000)
//     {
//       if(_SSerial->available())
//       {
//         if(!gotStartSymbol)
//         {
//             cmd=_SSerial->readStringUntil('\n');
//             if(cmd=="START\r")
//             {
//               temp=millis();
//               gotStartSymbol=true;
//               continue;
//             }
//         }
//         else if(cmd=="STOP\r")
//         {
//             gotStopSymbol=true;
//             break;
//         }
      
//         if(gotStartSymbol)
//         {        
//           decodeString(cmd);
//           cmd=cmd+"\n";
//           firmwareFile=firmwareFile+cmd;
//         }
//       }
//     }
//     if(gotStartSymbol && gotStopSymbol)
//       return true;
//   }
//   return false;
// }

// bool PSIM::decodeString(byte fileType,String &newData, String &returnFile)
// {
//   switch(fileType)
//   {
//     case FT_INFO:
      
//     break;

//     case FT_PROG:
//         if()
//     break;
//   }
//   return true;
// }

// void PSIM::getProgramSize(String ipaddress,String version)
// {
//   String fileName=ipaddress;
//   fileName=fileName + "/";
//   fileName = fileName+ Version;
//   fileName = fileName + "/";
//   fileName = fileName + "progDesc.txt";

//   String data;
//   if(getFile(fileName,data))
//   {

//     return true;
//   }
//   return false;
// }

// void PSIM::downloadFirmware()
// {
// }

bool PSIM::extendedSendCommand(String cmd,byte timeout)
{
  sendCommand(cmd,false);
  unsigned long temp =millis();
  while(millis()-temp< timeout*100)
  {
    if(_SSerial->available())
    {
      String str = _SSerial->readStringUntil('\n');
      if(str=="OK\r")
        return true;
      else if(str=="ERROR\r")
        return false;
    }
  }
  return false;
}

bool PSIM::startGPRS(String apn)
{
    //AT+SAPBR=3,1,"ConType","GPRS"
    // String m1="AT+SAPBR=3,1,\"";
    // String cmd;
    // cmd=m1;
    // cmd=cmd+"ConType\",\"GPRS\"\r\n";
    String cmd=F(STR_SAPBR_PARAM);
    cmd = cmd + F(STR_SAPBR_GPRS);
    if(extendedSendCommand(cmd,50))
    {
        cmd = F(STR_SAPBR_PARAM);
        cmd = cmd + F(STR_SAPBR_APN);
        cmd = cmd + apn;
        cmd = cmd + "\"\r\n";
        //AT+SAPBR=3,1,"APN","bsnlnet"
        if(extendedSendCommand(cmd,50))
        {
            if(sendBlockingATCommand(F(STR_SAPBR_START),true))
            {
                // unsigned long temp = millis();
                // while(millis() - temp < 2000)
                // {}
              delay(20);
                return true;
            }
        }
    }
  return false;
}

bool PSIM::stopGPRS()
{
    return(sendBlockingATCommand(F(STR_SAPBR_STOP),true));
}

bool PSIM::connectToFTP(String ipaddress)
{
  String cmd;
  //AT+FTPCID=1
  if(sendBlockingATCommand(F("AT+FTPCID=1\r\n"),true))
  {
    //AT+FTPSERV="43.252.117.34"
    cmd=F("AT+FTPSERV=\"");
    cmd=cmd+ipaddress;
    cmd=cmd+"\"\r\n";
    if(sendBlockingATCommand(cmd,true))
    {
      // AT+FTPUN="FTP-User"
      // AT+FTPPW="123456789"
      if(sendBlockingATCommand(F("AT+FTPUN=\"rahulnrathodkcl\"\r\n"),true))
      {
        if(sendBlockingATCommand(F("AT+FTPPW=\"123456789\"\r\n"),true))
        {
            return true;
        }
      }
    }
  }
  return false;
}

bool PSIM::setFile(String filename)
{
    String m1=F("AT+FTPGET");
    String cmd;
    cmd=m1;
    cmd=cmd + "NAME=\"";   //m.bin\"\r\n;
    cmd=cmd + filename;
    cmd= cmd + "\"\r\n";

    if(sendBlockingATCommand(cmd,true))
    {
        cmd=m1;
        cmd=m1+"PATH=\"/\"\r\n";
        if(sendBlockingATCommand(cmd),true)
            return true;  
    }
    return false;
}


bool PSIM::getProgramSize()
{
  // if(setFile("m.hex"))
  if(setFile(getHexFileName()))
  {
    // if(programSizeSet)
    if(eeprom1->programSizeSet)
      return true;
      // AT+FTPSIZE
      // OK
      // +FTPSIZE: 1,0,22876
      // String str ="AT+FTPSIZE\r\n";
      // String str2="+FTPSIZE: 1,0,";

      // if(extendedSendCommand(str,str2,14,60000))
      // {
      //     eeprom1->saveProgramSize(str.toInt());
      //     return true;
      // }

      unsigned long int temp=millis();
      sendCommand(F("AT+FTPSIZE\r\n"),false);
      String str;
      while(millis()-temp<120000L)
      {
          if(_SSerial->available())
          {
              str=_SSerial->readStringUntil('\n');
              #ifndef disable_debug
                _NSerial->println(str);
              #endif
              if(stringContains(str,F("+FTPSIZE: 1,0,"),14,str.length()-1))
              {
                // saveProgramSize(str.toInt());
                eeprom1->saveProgramSize(str.toInt());
                #ifndef disable_debug
                  _NSerial->println(str);
                #endif
                return true;
              }
          }        
      }
  }
  return false;
}

// bool PSIM::extendedSendCommand(String &cmd,String vstr,unsigned short int len,unsigned short int timeout)
// {
//   sendCommand(cmd,false);
//   unsigned long int temp=millis();
//   String str;
//     while(millis()-temp<timeout)
//     {
//         if(_SSerial->available())
//         {
//             str=_SSerial->readStringUntil('\n');
//             if(stringContains(str,vstr,len,cmd.length()-1))
//             {
//                 cmd=str;
//                 return true;
//             }
//         }          
//     }
//     return false;
// }


bool PSIM::downloadFirmware()
{
    // unsigned long int size;
    // EEPROM.get(prgSizeAddress, size);
    unsigned long int size=eeprom1->getProgramSize();
    
    unsigned long int temp;
    
    //  +FTPGETTOFS: 0,2396
    String m1="+FTPGETTOFS";
    String cmd;
    String v1= m1 + ": 0,";
    v1 = v1 + size;
    v1 = v1 + "\r";

    // AT+FTPGETTOFS=0,"m.hex"\r\n
    cmd="AT"; 
    cmd=cmd+m1;
    // cmd=cmd+ "=0,\"m.hex\"\r\n";
    cmd=cmd+ "=0,\"";
    cmd = cmd + getHexFileName();
    cmd = cmd +  "\"\r\n";

    temp=millis();
    sendCommand(cmd,false);
    while(millis()-temp<120000L)
    {
        if(_SSerial->available())
        {
            cmd=_SSerial->readStringUntil('\n');
            #ifndef disable_debug
                _NSerial->println(cmd);
            #endif
            if(cmd==v1)
            {
              #ifndef disable_debug
                _NSerial->println("DC");
              #endif   
                return true;
            }
        }          
    }
    return false;
    // return extendedSendCommand(cmd,v1,25,60000);           
}

bool PSIM::isGPRSConnected()
{
    
  //+SAPBR: 1,1,"0.0.0.0"
  sendCommand(F("AT+SAPBR=2,1\r\n"),false);
  unsigned long int temp =millis();
  while(millis()-temp<5000)
  {
    if(_SSerial->available())
    {
      String str=_SSerial->readStringUntil('\n');
      #ifndef disable_debug
          _NSerial->println(str);
      #endif
      if(stringContains(str,F("+SAPBR: 1,3"),11,str.length()-1))
        return false;
      else if(stringContains(str,F("+SAPBR: 1,1"),11,str.length()-1))
        return true;
    }
  }
  return false;
  //removed code from here to send speicfic command, and wait for the needed response for specified time..
  // String str="AT+SAPBR=2,1\r\n";
  // String str2="+SAPBR: 1,1";
  // return extendedSendCommand(str,str2,11,5000);
}

bool PSIM::prepareForFirmwareUpdate(String &ipaddress)
{
  bool gprs=false;
  if(!(gprs=isGPRSConnected()))
  { 
    // if(startGPRS("bsnlnet"))
    if(startGPRS(getAPN()))
    {
      byte cnt = 12;
      while(--cnt)
      {
        // unsigned long temp = millis();
        // while(millis()-temp<5000)
        // {}
        delay(50);
        if((gprs=isGPRSConnected()))
          break;
      }
    }
  }

  if(gprs)
  {
    if(connectToFTP(ipaddress))
    {
        if(getProgramSize())
        {
          if(downloadFirmware())
          {
            return true;
          }
        }
    }
  }
  return false;
}


// void PSIM::watchdogConfig(uint8_t x) {
//   WDTCSR = _BV(WDCE) | _BV(WDE);
//   WDTCSR = x;
// }

// void PSIM::watchdogReset() {
//   __asm__ __volatile__ (
//     "wdr\n"
//   );
// }

// typedef void (*AppPtr_t)(void) __attribute__ ((noreturn));
// 
// AppPtr_t AppStartPtr = (AppPtr_t)0x0000;
// AppStartPtr();


// #asm("jmp 0xE00") // jump to bootloader

void PSIM::jumpToBootloader() // Restarts program from beginning but does not reset the peripherals and registers
{
asm volatile ("  jmp 0X7800");  //bootloader vector start address set using BOOTSZ fuse. 
}  
// void PSIM::jumpToBootloader()
// {
  // __asm__ __volatile__ (
    // "jmp 0x7800\n"
  // );
// }

// void PSIM::updateFirmware(bool temp,bool verify)
// {
//   EEPROM.put(prgUpdateRequestAddress, (byte)temp); 
//   EEPROM.put(prgUpdateTryAddress, 0x00);
//   EEPROM.put(VerifyStatusAddress, (byte)!verify);
// }

void PSIM::initRestartSeq()
{
    sendBlockingATCommand(F("AT+CSCLK=0\r\n"));
    sendBlockingATCommand(F("AT&W\r\n"));
    sendBlockingATCommand(F("AT+COPS=2\r\n"));
    // isMsgFromAdmin=true;
    // sendSMS("DC",true);
    eeprom1->updateFirmware(true,true);
    // unsigned long int temp= millis();
    delay(100);
    // while(millis()-temp<10000)
    // {}
    jumpToBootloader();
}

bool PSIM::checkPrgReq(String str,bool noMsg)
{
  byte p=0;
  isMsgFromAdmin=true;
  if(stringContains(str,F("PRGUPD"),6,str.length()-1))
  {
      if(currentStatus=='N')
        endCall();
      if(!noMsg) sendSMS("SUP",true);
      // bool verify=false;
      // if(stringContains(str,"V",1,str.length()-1))
      //   verify=true;
      str.toLowerCase();
      if(prepareForFirmwareUpdate(str))
      {
          isMsgFromAdmin=true;
          if(!noMsg) sendSMS("DC",true);
          stopGPRS();
          initRestartSeq();
      }
      else
      {
          stopGPRS();
          // programSizeSet=false;
          // updateFirmware(false,false);
          eeprom1->programSizeSet=false;
          eeprom1->updateFirmware(false,false);
          #ifndef disable_debug
            _NSerial->println("ED");
          #endif
          isMsgFromAdmin=true;
          if(!noMsg) sendSMS("ED",true);
      }
      return true;
  }
  else if(stringContains(str,F("PRGSIZE"),7,str.length()-1))
  {
      p=0x01;
  }
  else if(stringContains(str,F("REUPD"),5,str.length()-1))
  {
    p=0x02;
  }
  else
  {
      p=dCheckPrgReq(str,noMsg);
  }

  if(p>0)
  {
      if((0xF0 & p) == 0x00)
        eeprom1->saveProgramSize(str.toInt());

        // saveProgramSize(str.toInt());
      String tempStr;
      tempStr = F("OK : ");
      tempStr = tempStr + str;
      if(!noMsg) sendSMS(tempStr,true);
      if((0x0F & p)==0x02)
        initRestartSeq();
      return true;
  }
  isMsgFromAdmin=false;
  return false;
}

void PSIM::stopCallWaiting()
{
    sendBlockingATCommand(F("AT+CCWA=0\r\n"));
}


// void(CTestFncPtr::*Switch)(char * charData);

bool PSIM::getBlockingResponse(String &cmd,bool (PSIM::*func)(String &))
{
  unsigned long temp = millis();
  sendCommand(cmd,true);
  while(millis() - temp <4000)
  {
    if(_SSerial->available())
    {
      String str = readString();
      if((this->*func)(str))
      {
          cmd=str;
          return true;
      }
    }
  }
  return false;
}

void PSIM::operateOnMsg(String str, bool admin = false,bool noMsg=false)
{
  isMsgFromAdmin=admin;
  String tempStr=str;
  bool done=false;
  bool processed=false;
  unsigned short int data;
 
    // if (str.startsWith(F("SCWAIT")))// stringContains(str, F("AUTOON"), 6, str.length() - 1))
    // {
      // stopCallWaiting();
      // done=true;
    // }
    // else if(str.startsWith("@"))
    // {
    //     processed=true;
    //     sendCommand(str.substring(1,str.length()),true);
    //     unsigned long temp = millis();
    //     String t2="";
    //     while(millis()-temp<4000)
    //     {
    //       String temp2=readString();
    //         t2=t2+temp2;
    //         if(temp2=="OK\r" || temp2=="ERROR\r")
    //           break;
    //     }
    //     sendSMS(t2,true);
    // }
    if(str.startsWith(F("WAIT")))
    {
      stopCallWaiting();
      done=true;
    }
    else if (str.startsWith(F("RESET")))// stringContains(str, F("RESET"), 5, str.length() - 1))
      jumpToBootloader();   
    else if (str.startsWith(F("CLEARALL")))// (stringContains(str, F("CLEARALL"), 8, str.length() - 1))) //if (str == "CLEARALL" && admin)
    {
      eeprom1->clearNumbers(admin);
      done=true;
    }
    else if (str.startsWith(F("DNDON")))//stringContains(str, F("DNDON"), 5, str.length() - 1))
    {
      eeprom1->saveDNDSettings(true);  //set DND to true in EEPROM
      done=true;
    }
    else if (str.startsWith(F("DNDOFF")))//stringContains(str, F("DNDOFF"), 6, str.length() - 1))
    {
      eeprom1->saveDNDSettings(false);  //set DND to False in EEPROM
      done=true;
    }
    else if (str.startsWith(F("RESPC")))  //stringContains(str, F("RESPC"), 5, str.length() - 1))
    {
      eeprom1->saveResponseSettings('C');  //set DND to False in EEPROM
      done=true;
    }
    else if (str.startsWith(F("RESPA"))) //stringContains(str, F("RESPA"), 5, str.length() - 1))
    {
       eeprom1->saveResponseSettings('A');  //set DND to False in EEPROM
       done=true;
    } 
    else if (str.startsWith(F("STATUS")))//stringContains(str, F("STATUS"), 6, str.length() - 1))
    {
      processed=true;
      
      byte batPer,net;
      batPer = net = 0xFF;
      
      str = F("AT+CSQ");
      if(getBlockingResponse(str,&PSIM::isCSQ))
      {
        net = str.toInt()*3;
      }
      delay(5);
      str = F("AT+CBC");
      if(getBlockingResponse(str,&PSIM::isCBC))
      {
        batPer = (str.substring(0,str.lastIndexOf(","))).toInt();
      }
      
      sendSMS(makeStatusMsg(batPer,net),true);
    }
    else if (str.startsWith(F("AMON")))//stringContains(str, "AMON", 4, str.length() - 1))
    {
      if (eeprom1->alterNumberPresent)
      {
        eeprom1->saveAlterNumberSetting(true);
        done=true;
      }
    }
    else if (str.startsWith(F("AMOFF")))//stringContains(str, "AMOFF", 5, str.length() - 1))
    {
      eeprom1->saveAlterNumberSetting(false);
      done=true;
    }
    else if (stringContains(str, F("BAL"), 3, str.length() - 1))
    {
      processed=true;
      
      String s2;
      s2 = F("AT+CUSD=1,\"");
      s2.concat(str);
      s2.concat("\"");
      sendCUSDResponse = true;
      sendCommand(s2, true);
    }
    else if (stringContains(str, "M-", 2, 12))
    {
      if (str.length() == 10 && isNumeric(str))
      {
        bool t = eeprom1->removeNumber(str);
        if(t) done=true;
#ifndef disable_debug
        _NSerial->print("Rem:");
        _NSerial->println((bool)t);
#endif
      }
    }
    else if (stringContains(str, "M+", 2, 12))
    {
      if (str.length() == 10 && isNumeric(str))
      {
        bool t = eeprom1->addNumber(str);
        if(t) done=true;
#ifndef disable_debug
        _NSerial->print("Add:");
        _NSerial->println((bool)t);
#endif
      }
    }
    else if (stringContains(str, "AM+", 3, 13))
    {
      if (str.length() == 10 && isNumeric(str))
      {
        bool t = eeprom1->addAlternateNumber(str);
        if(t) done=true;
#ifndef disable_debug
        _NSerial->print("A ");
        _NSerial->print("Add:");
        _NSerial->println((bool)t);
#endif
      }
    }
    else 
    {
      addOperateOnMsg(str,admin,tempStr,done,processed);
    }
    // else if (stringContains(str, F("NET"), 3, str.length() - 1))
    // {
    //   processed=true;
    //   sendCommand(F("AT+CSQ"), true);
    //   sendCSQResponse = true;
    // }

    if(!processed && !noMsg)
    {
      String temp2;
      if(done)
        temp2 = F("OK : ");
      else
        temp2 = F("ERROR : ");        
      temp2 = temp2 + tempStr;
      sendSMS(temp2,true);
    }
}


// bool PSIM::getBalance()
// {
      // String s2;
      // s2 = F("AT+CUSD=1,\"");
      // s2.concat(balStr);
      // s2.concat("\"");
      // sendCommand(s2, true);
// }

inline bool PSIM::isCBC(String &str)
{
  return(stringContains(str,"+CBC:",str.indexOf(",")+1,str.length()-1));
}

inline bool PSIM::isCUSD(String &str)
{
  return stringContains(str, "+CUSD:", 6, str.length() - 1);
}

inline bool PSIM::isCSQ(String &str)
{
  return stringContains(str, "+CSQ", 5, str.length() - 3);
}

inline void PSIM::sendReadMsg(String str)
{
  String s;
  s = "AT+CMGR=";
  s.concat(str);
  sendCommand(s, true);
}

inline bool PSIM::isMsgBody(String &str)
{
  return stringContains(str, "+CMGR:", 24, 34);
}

inline bool PSIM::isAdmin(String str)
{
  return (str == adminNumber);
}

void PSIM::gotMsgBody(String &str)
{
  bool admin = isAdmin(str);
  // if (admin || isPrimaryNumber(str))//eeprom1->isPrimaryNumber(str))
  if (admin || eeprom1->isPrimaryNumber(str))
  {
    str = readString(); //_SSerial->readStringUntil('\n');
#ifndef disable_debug
    _NSerial->print("MSG:");
    _NSerial->println(str);
#endif

    bool noMsg=false;
    if(str.startsWith("#"))
    {
      str=str.substring(1,str.length());
      noMsg=true;
    }
    str.toUpperCase();
    if(admin)
    {
      if(!checkPrgReq(str,noMsg))
        operateOnMsg(str,admin,noMsg);
    }
    else
      operateOnMsg(str, admin,noMsg);
  }
  delAllMsg();
}

bool PSIM::isNewMsg(String &str)
{
  return stringContains(str, "+CMTI:", 12, str.length() - 1);
}


bool PSIM::initialize()
{
  byte attempts = 0;
try_again:
  if (sendBlockingATCommand("AT\r\n"),true)
  {
    // if(sendBlockingATCommand(F("AT+COPS=0\r\n"),true))
    // {
      if (sendBlockingATCommand(F("AT+CLIP=1\r\n")))
      {
        if (sendBlockingATCommand(F("AT+CLCC=1\r\n")) && sendBlockingATCommand(F("AT+CMGF=1\r\n")) &&  sendBlockingATCommand(F("AT+CNMI=2,1,0,0,0\r\n")))
        {

          sendBlockingATCommand(F("AT+CSCLK=1\r\n"));
          sendBlockingATCommand(F("AT&W\r\n"));

  #ifndef disable_debug
          _NSerial->println("INIT");
  #endif
          initialized = true;
          sendBlockingATCommand(F("AT+DDET=1\r\n"));
          return true;
        }
      }
    // }
  }
  if (!initialized && attempts == 0)
  {
    attempts++;
    goto try_again;
  }
#ifndef disable_debug
  _NSerial->println("INIT X");
#endif
  return false;
}

bool PSIM::isNumber(String &str)
{
  return (stringContains(str, "+CLIP: \"", 11, 21));
}

bool PSIM::checkNumber(String number)
{
#ifndef disable_debug
  _NSerial->print("no:");
  _NSerial->println(number);
#endif

  // if (isAdmin(number) || (checkExists(number)!=0xFF))//(eeprom1->checkExists(number)!=0xFF))
  if (isAdmin(number) || (eeprom1->checkExists(number)!=0xFF))
    return true;

  return false;
}

void PSIM::acceptCommands()
{
  if (!commandsAccepted)
  {
#ifndef disable_debug
    _NSerial->print("com");
    _NSerial->print("Acc:");
    _NSerial->println(millis());
#endif
    digitalWrite(PIN_DTR,HIGH);
    commandsAccepted = true;
    tempAcceptCommandTime = millis();
    delay(1);
    // while (millis() - tempAcceptCommandTime <= 100)
    // {}
  }
  else
  {
    tempAcceptCommandTime = millis();
  }
}

void PSIM::rejectCommands()
{
#ifndef disable_debug
  _NSerial->print("com");
  _NSerial->print("Rej");
  _NSerial->println(millis());
#endif
  digitalWrite(PIN_DTR,LOW);
  commandsAccepted = false;
}

void PSIM::sendCommand(char cmd, bool newline = false)
{
  acceptCommands();
  if (!newline)
    _SSerial->print(cmd);
  else
    _SSerial->println(cmd);
}

void PSIM::sendCommand(String cmd, bool newline = false)
{
  acceptCommands();
  if (cmd == "")
    _SSerial->println();
  else
  {
    if (!newline)
      _SSerial->print(cmd);
    else
      _SSerial->println(cmd);
  }
}

bool PSIM::sendBlockingATCommand(String cmd,bool extendTime)
{
  sendCommand(cmd);
#ifndef disable_debug
  _NSerial->print(cmd);
#endif

  unsigned long t = millis();
  String str;
  unsigned long int t2=3000L;
  if(extendTime)
    t2=60000L;
  while (millis() - t < t2)
  {
    if (_SSerial->available() > 0)
    {
      str = readString();
      #ifndef disable_debug
        _NSerial->println(str);
      #endif
      if (matchString(str, "OK\r") == true)
        return true;
      else if (matchString(str, "ERROR\r") == true)
        return false;
    }
  }
  return false;
}

String PSIM::readString()
{
  String str = "";
  if (_SSerial->available() >  0)
  {
    acceptCommands();
    str = _SSerial->readStringUntil('\n');
#ifndef disable_debug
    _NSerial->println(str);
#endif
  }
  return str;
}

bool PSIM::matchString(String m1, String m2)
{
  return (m1 == m2);
}

bool PSIM::stringContains(String &sstr, String mstr, byte sstart, byte sstop)
{
  if (sstr.startsWith(mstr))
  {
    sstr = sstr.substring(sstart, sstop);
    return true;
  }
  return false;
}

bool PSIM::isRinging(String str)
{
  return (str == "RING\r");
}

bool PSIM::isDTMF(String &str)
{
  return stringContains(str, "+DTMF: ", 7, 8);
}

bool PSIM::isCut(String str)
{
  if (currentStatus == 'I' && (currentCallStatus == 'I' || currentCallStatus == 'O'))
  {
#ifndef disable_debug
    _NSerial->print("STR:");
    _NSerial->println(str);
    // _NSerial->println("STR END");
#endif
  }

  if (matchString(str, F("NO CARRIER\r")))
    return true;
  else if (matchString(str, F("BUSY\r")))
    return true;
  else if (matchString(str, F("NO ANSWER\r")))
    return true;
  else if (matchString(str, "ERROR\r"))
    return true;
  return false;
}

bool PSIM::isSoundStop(String str)
{
  return (matchString(str, "+CREC: 0\r"));
}

char PSIM::callState(String str)
{
#ifndef disable_debug
  _NSerial->print("str:");
  _NSerial->println(str);
#endif


  if (stringContains(str, F("+CLCC: 1,0,2"), 11, 12))    //dialling
    return 'D'; //call made
  if (stringContains(str, F("+CLCC: 1,0,3"), 11, 12))    //alerting
    return 'R'; //call made
  else if (stringContains(str, F("+CLCC: 1,0,0"), 11, 12)) //active call
    return 'I'; //call accepted
  else if (stringContains(str, F("+CLCC: 1,0,6"), 11, 12)) //call ended
    return 'E'; //call ended
  else
    return 'N';
}


void PSIM::makeCall()
{
  inCall=true;  
  acceptCommands();
  _SSerial->flush();

  String command;
  command = "ATD+91";
  // command.concat(getActiveNumber());
  command.concat(eeprom1->getActiveNumber());
  command.concat(";");
  sendCommand(command, true);

  delay(1);
  // unsigned long temp = millis();
  // while (millis() - temp < 100)
  // {    }
  sendCommand("", true);
#ifndef disable_debug
  _NSerial->println("CallMade");
#endif
  // eeprom1->inCall(true);
  callCutWait = millis();
  currentStatus = 'R';
  currentCallStatus = 'O';
  // attemptsToCall++;
}

void PSIM::endCall()
{
  nr = 0;
  inCall=false;

  _SSerial->flush();
  sendCommand("AT+CHUP", true);
  unsigned long temp = millis();
  delay(1);
  // while (millis() - temp < 100)
  // {  }
  sendCommand("", true);
  _SSerial->flush();
  freezeIncomingCalls = false;
  // callDialled = false;
  // callAlerted = false;
  // attemptsToCall=0;

  // eeprom1->inCall(false);
  callAccepted = false;
  // responseToAction = false;
  currentStatus = 'N';
  currentCallStatus = 'N';

  obtainEventTimer = millis();
  obtainNewEvent = false;
  //  starPresent=false;
#ifndef disable_debug
  _NSerial->println("CallEnd");
#endif
}

void PSIM::setObtainEvent()
{
  if (!obtainNewEvent  && millis() - obtainEventTimer > 1000)
    obtainNewEvent = true;
}

void PSIM::acceptCall()
{
  callAccepted = true;
  _SSerial->flush();
  sendCommand("ATA", true);
  _SSerial->flush();
  currentStatus = 'I';
  currentCallStatus = 'I';
  playSound('M');
}

void PSIM::sendSMS(String msg = "", bool predefMsg = false)
{
  inCall=true;
  _SSerial->flush();
  String responseString;

  if (!predefMsg)
  {
    if(!makeSMSString(responseString,actionType))
    {
      return;
    }      
  }
  else
    responseString = msg;

// #ifndef disable_debug
  // _NSerial->println("SMS");
// #endif
  String command;
  command = "AT+CMGS=\"+91";
  if (isMsgFromAdmin)
    command.concat(adminNumber);
  else
    command.concat(eeprom1->getActiveNumber());
    
    // command.concat(getActiveNumber());

  command.concat("\"");

  _SSerial->flush();
  sendCommand(command, true);
  _SSerial->flush();
  // unsigned long int temp = millis();
  // while (millis() - temp < 1000)
  // {}
  delay(10);
  sendCommand(responseString, true);
  _SSerial->flush();
  delay(10);
  // temp = millis();
  // while (millis() - temp < 1000)
  // {}
  _SSerial->flush();
  sendCommand((char)26, true);
  _SSerial->flush();
  // temp = millis();
  // while (millis() - temp < 1000)
  // {}
  delay(10);
  isMsgFromAdmin = false;
  inCall=false;
}

void PSIM::delay(byte time)
{
  unsigned long temp = millis();
  while (millis() - temp < time*100)
  {}
}

void PSIM::subDTMF()
{
  //    // starPresent=true;
  callCutWait = millis();
  stopSound();
}

void PSIM::operateRing()
{
  nr++;
  if (nr <= 2)
  {

    if (nr == 1)
    {
      sendCommand(F("AT+DDET=1\r\n"));
      _SSerial->flush();
    }

    String str;
    do
    {
      str = readString();
    }
    while (isNumber(str) == false);

    callCutWait = millis();

    if (str.length() >= 10 && isNumeric(str))
    {
      if (nr > 1 && !checkNumber(str))
        endCall();
    }
  }
  else if (nr == 3)
  {
    callCutWait = millis();
    acceptCall();
  }
}

bool PSIM::playSoundElligible()
{
  return (bplaySound && ((millis() - soundWait) > (soundWaitTime * 100)));
}

void PSIM::triggerPlaySound()
{
  _SSerial->flush();
  sendCommand(F("AT+CREC=4,\""));
  sendCommand("C:\\User\\FTP\\");
  sendCommand(playFile);
  sendCommand(".amr\",0,100,1\r", true);
  _SSerial->flush();
  bplaySound = false;
}

// void PSIM::playSoundAgain(String str)
// {
//   if (isSoundStop(str))
//   {
////       if (starPresent)
//       {
//           if(playFile==actionType)
//             playFile='N';
//           else
//             playFile=actionType;
//       }
//       playSound(playFile);
//   }
// }

void PSIM::playSound(char actionType, bool newAction)
{
  _SSerial->flush();
  stopSound();

  soundWait = millis();
  bplaySound = true;
  if (newAction)
  {
    this->actionType = actionType;
  }
  playFile = actionType;
}

void PSIM::stopSound()
{
  _SSerial->flush();
  sendCommand(F("AT+CREC=5\r"), true);
  _SSerial->flush();
}

bool PSIM::callTimerExpire()
{
  return ((millis() - callCutWait) >= (callCutWaitTime * 100));
}

void PSIM::makeResponseAction()
{
  // makeResponse = false;
  // byte b= getResponseSetting();


  // if (b == 'A' || b == 'C')
  if (eeprom1->RESPONSE == 'A' || eeprom1->RESPONSE == 'C')
    makeCall();
}

bool PSIM::registerEvent(char eventType)
{
  if (!initialized)
  {
#ifndef disable_debug
    _NSerial->println("NO PSIM");
#endif
    return true;
  }

  if (currentStatus == 'N' && currentCallStatus == 'N' && obtainNewEvent)
  {
    freezeIncomingCalls = true;
    acceptCommands();

#ifndef disable_debug
    _NSerial->print("E:");
    _NSerial->print(eventType);
#endif

    actionType = eventType;
    makeResponseAction();
    return true;
  }
  else
    return false;
}


bool PSIM::rejectCommandsElligible()
{
  return (commandsAccepted && millis() - tempAcceptCommandTime >= (acceptCommandsTime * 100));
}

void PSIM::checkNetwork(String str)
{
  if (str == F("+CPIN: NOT READY\r"))
  {
    endCall();
    startSIMAfterUpdate();
  }
}

// void PSIM::networkCounterMeasures()
// {
//   // _SSerial->flush();
//   // sendBlockingATCommand(F("AT+CFUN=1,1\r\n"));
//   // _SSerial->flush();
//   // unsigned long t = millis();
//   // delay(1);
//   // t=millis();
//   // while (millis() - t < 100)
//   // {}
// }



inline bool PSIM::isCallReady(String str)
{
  return matchString(str, "Call Ready\r");
}

bool PSIM::checkSleepElligible()
{
  return(!commandsAccepted  && checkNotInCall());
}

bool PSIM::checkNotInCall()
{
 return ( !sendCUSDResponse     &&  currentStatus=='N'  
  &&  currentCallStatus=='N'  &&  obtainNewEvent
  &&  !freezeIncomingCalls    && addCheckNotInCall());

  // &&  !makeResponse           &&  !freezeIncomingCalls 

}

// void PSIM::setNetLight(byte light)
// {
//   bool isSleep=commandsAccepted;
//     if(light==L_REGULAR)
//       sendBlockingATCommand(F("AT+SLEDS=2,64,3000\r\n"));
//     else
//       sendBlockingATCommand(F("AT+SLEDS=2,64,10000\r\n"));
//     if(!isSleep)
//       rejectCommands();
// }

bool PSIM::busy()
{
  return (inCall || inInterrupt);
}

inline bool PSIM::checkEventGone()
{
  return (millis() - tempInterruptTime > 2000);

}

String PSIM::makeStatusMsg(byte battery, byte network)
{
      String resp;
      addTextToStatusMsg(resp);
      if(battery!=0xFF)
      {
        resp = resp + F("\nBat%:");
        resp = resp + battery;        
      }
      if(network!=0xFF)
      {
        resp = resp + F("\nNetwork:");
        resp = resp + network;        
      }
      return resp;
}

void PSIM::checkRespSMS(char t1)
{
  if (!callAccepted && eeprom1->RESPONSE=='A')
  {
#ifndef disable_debug
        _NSerial->print("DIAL");
        _NSerial->println("OFF");
#endif
          actionType = t1;
          sendSMS();
  }
}

// void enableRingLevelInterrupt()
// {
  
// }

void PSIM::update()
{
  if(inInterrupt && checkEventGone())
  {
    enableRingLevelInterrupt();
    inInterrupt=false;
  }

  // if (rejectCommandsElligible() && !motor1->ACPowerState())
  if (rejectCommandsElligible() && addCheckRejectCommands())
  {
    rejectCommands();
  }

  addUpdate();

  if (currentStatus == 'N')
  {
    setObtainEvent();

    // if (makeResponse)
      // makeResponseAction();
  }
  else if (currentStatus == 'I' || currentStatus == 'R')
  {
    if (callTimerExpire())
    {
      char t1 = actionType;
      endCall();
      checkRespSMS(t1);
    }

    if (playSoundElligible())
      triggerPlaySound();
  }

  while (_SSerial->available() > 0)
  {
    String str;
    str = readString();

    addUpdateStringData(str);

    if(isCUSD(str) && sendCUSDResponse)    //bal
    {
       sendSMS(str,true);
       sendCUSDResponse=false;
    }

    if (isNewMsg(str))
    {
      sendReadMsg(str);
    }
    else if (isMsgBody(str))
    {
      gotMsgBody(str);
    }
    else if (isCallReady(str))
    {
#ifndef disable_debug
      _NSerial->println("INIT");
#endif
      initialized = true;
    }
    else
      checkNetwork(str);

    if (!freezeIncomingCalls &&  (currentStatus == 'N' || currentStatus == 'R') && (currentCallStatus == 'N' || currentCallStatus == 'I')) //Ringing Incoming Call
    {
      if (isRinging(str)) //  chk_ringing(str) == true)
      {
        inCall=true;
        currentStatus = 'R';
        currentCallStatus = 'I';
        operateRing();
      }
      else if (isCut(str))
      {
        endCall();
      }
    }
    else if (!freezeIncomingCalls && currentStatus == 'I' && currentCallStatus == 'I') //IN CALL INCOMING CALL
    {
      if (isCut(str)) //chk_cut(str) == true)
      {
        endCall();
      }
      else if (isDTMF(str)) //chk_DTMF(str) == true)
      {
        operateDTMF(str);
      }
    }
    else if ((currentStatus == 'N' || currentStatus == 'R') && currentCallStatus == 'O') // OUTGOING CALL
    {
      // if (callState(str) == 'D')
      // {
        // #ifndef disable_debug
        // _NSerial->print("DIAL");
        // _NSerial->println("ON");
        // #endif
        // callDialled=true;
      // }
      if (callState(str) == 'R')
      {
        // #ifndef disable_debug
        // _NSerial->print("ALRT");
        // _NSerial->println("ON");
        // #endif
        // callAlerted = true;
        callCutWait = millis();
        currentStatus = 'R';
        currentCallStatus = 'O';
      }
      else if (isCut(str) || callState(str) == 'E') //
      {
        char t1 = actionType;
        endCall();
        checkRespSMS(t1);
      }
      else if (callState(str) == 'I') //else if (stringContains(str, "+CLCC: 1,0,0", 11, 12) == true)
      {
        #ifndef disable_debug
        _NSerial->println("Accept");
        #endif
        callCutWait = millis();
        currentStatus = 'I';
        currentCallStatus = 'O';
        callAccepted = true;
        playSound(actionType);
      }
    }
    else if (currentStatus == 'I' && currentCallStatus == 'O') //IN CALL OUTGOING CALL
    {
      if (isCut(str) || callState(str) == 'E')
        endCall();
      else if (isDTMF(str)) //chk_DTMF(str) == true)
      {
        operateDTMF(str);
      }
      else
      {
        // playSoundAgain(str);
      }
    }
  }
}
