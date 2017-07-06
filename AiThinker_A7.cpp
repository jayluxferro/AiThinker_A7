#include "AiThinker_A7.h"


AiThinker_A7::AiThinker_A7(){

}
AiThinker_A7::~AiThinker_A7(){
  if(BoardSerial!=NULL){
    delete BoardSerial;
    BoardSerial=NULL;
  }
}
AiThinker_A7::AiThinker_A7(int RX,int TX,int Power,int Reset){
  AiThinker_A7();
  setup(RX, TX, Power, Reset);
}
void AiThinker_A7::setup(int RX,int TX,int Power,int Reset){
  PIN_Power=Power;
  PIN_Reset=Reset;
  PIN_RX=RX;
  PIN_TX=TX;

  //board=t;
  if(BoardSerial!=NULL){
    delete BoardSerial;
    BoardSerial=NULL;
  }
  BoardSerial=new SoftwareSerial(RX,TX);

}

void AiThinker_A7::begin(long baudrate,bool reset){
  BoardSerial->begin(baudrate);
  power_on();
  if (reset){
    _start();
    Operator();
    SetAPN("TM");    
  }

}
void AiThinker_A7::power_on(){
  if (PIN_Power>0){
    pinMode(PIN_Power, OUTPUT);
    digitalWrite(PIN_Power, HIGH);
  }
  if (PIN_Reset>0){
    pinMode(PIN_Reset, OUTPUT);
    digitalWrite(PIN_Reset, HIGH);
  }
  delay(500);
}
void AiThinker_A7::reset(){
  if (PIN_Reset>0){
    pinMode(PIN_Reset, OUTPUT);
    digitalWrite(PIN_Reset, LOW);
    delay(1000);
    digitalWrite(PIN_Reset, HIGH);
  }
  delay(500);
}
void AiThinker_A7::serial_debug(){
  Serial.println("Serial Debug:");
  while (1) {
    if (Serial.available()) {
      BoardSerial->write(Serial.read());
    }
    if (BoardSerial->available()) {
      Serial.write(BoardSerial->read());
    }
  }

}
void AiThinker_A7::debug(bool is_on){
  debug_on=is_on;
}
byte AiThinker_A7::cmd(String command, String response1, String response2, int timeOut, int repetitions) {
  byte returnValue = AT_NO;
  byte count = 0;
  while (count < repetitions && returnValue != AT_OK) {
    BoardSerial->println(command);

    if (debug_on){
      Serial.print("Command: ");
      Serial.println(command);
    }

    if (waitFor(response1, response2, timeOut) == AT_OK) {
      //     Serial.println("OK");
      returnValue = AT_OK;
    } else returnValue = AT_NO;
    count++;
  }
  return returnValue;
}
byte AiThinker_A7::waitFor(String response1, String response2, unsigned long timeOut) {
  unsigned long entry = millis();
  String reply = "";//BoardRead();
  byte retVal = 99;
  do {
    reply = BoardRead();
    if ((debug_on)&&(reply != "")) {
      Serial.print((millis() - entry));
      Serial.print(" ms ");
      Serial.print("data = ");
      Serial.println(reply);
    }
  } while ((reply.indexOf(response1) + reply.indexOf(response2) == -2) && millis() - entry < timeOut );
  if ((millis() - entry) >= timeOut) {
    retVal = AT_TIMEOUT;
  } else {
    if (reply.indexOf(response1) + reply.indexOf(response2) > -2) retVal = AT_OK;
    else retVal = AT_NO;
  }

  if (debug_on){
    Serial.print("retVal = ");
    Serial.println(retVal);
    Serial.print("data = ");
    Serial.println(reply);
  }
  return retVal;
}
String AiThinker_A7::BoardRead() {
  String reply = "";
  if (BoardSerial->available())  {
    reply = BoardSerial->readString();
  }
  if ((debug_on)&&(reply != "")) {
    Serial.print("Reply: ");
    Serial.println(reply);
  }

  return reply;
}
bool AiThinker_A7::_start() {
  BoardSerial->println("AT+CREG?");
  byte hi = waitFor("1,", "5,", 1500);  // 1: registered, home network ; 5: registered, roaming

  while ( hi != AT_OK) {
    BoardSerial->println("AT+CREG?");
    hi = waitFor("1,", "5,", 1500);
  }

  if (cmd("AT&F0", "OK", "YES", 5000, 2) == AT_OK) {   // Reset to factory settings
    if (cmd("ATE0", "OK", "YES", 5000, 2) == AT_OK) {  // disable Echo
      if (cmd("AT+CMEE=2", "OK", "YES", 5000, 2) == AT_OK){return AT_OK;  } // enable better error messages
      else return AT_NO;
    }else return AT_NO;
  }else return AT_NO;
}

bool AiThinker_A7::Operator() {
  if (cmd("AT+COPS=1,0,\"Turkcell\"", "OK", "YES", 8000, 2) == AT_OK) {
        return AT_OK;
  }
  else {
        return AT_NO;
  }

}

bool AiThinker_A7::SetAPN(String APN) {
  if (cmd("AT+CSTT=\""+APN+"\",\"\",\"\"", "OK", "YES", 8000, 2) == AT_OK) {
        return AT_OK;
  }
  else {
        return AT_NO;
  }

}

bool AiThinker_A7::GPRS_Start(String APN) {
  if (cmd("AT+CGATT=1", "OK", "YES", 8000, 2) == AT_OK) {    
    if (cmd("AT+CGDCONT=1, \"IP\", \""+APN+"\"", "OK", "YES", 8000, 2) == AT_OK) {
      if (cmd("AT+CGACT=1,1", "OK", "YES", 8000, 2) == AT_OK) {
        if (cmd("ATD*99***1#", "CONNECT", "YES", 8000, 2) == AT_OK) {
          return AT_OK;
        }else return AT_NO;
      }else return AT_NO;
    }else return AT_NO;
  }else return AT_NO;
}

bool AiThinker_A7::TCP(String host,String port) {
  Close();

  debug_command("AT+CIPSTATUS", 800);

  String at="AT+CIPSTART=\"TCP\",\"";
  at=at+host+"\","+port;
  //if (cmd(at, "CONNECT OK", "YES", 16000, 2) == AT_OK) {
  if (cmd(at, "OK", "YES", 16000, 2) == AT_OK) {
      return AT_OK;
  }else return AT_NO;
}

bool AiThinker_A7::UDP(String host,String port) {
  Close();
  String at="AT+CIPSTART=\"UDP\",\"";
  at=at+host+"\","+port;
  if (cmd(at, "OK", "YES", 16000, 2) == AT_OK) {
      return AT_OK;
  }else return AT_NO;
}
bool AiThinker_A7::Send(String data){
  int len=data.length();
  String sd="AT+CIPSEND=";
  sd=sd+len;
  sd=sd+",\""+data+"\"";
//  cmd("AT+CIPSEND", ">", "YES", 10000, 1); //begin send data to remote server
  bool r=cmd(sd, "OK", "YES", 10000, 1); //begin send data to remote server

  // delay(500);
  // BoardSerial->print(data);
  // BoardSerial->print(0x1A);
  delay(600);
  return r;
}
bool AiThinker_A7::Close(){
  BoardSerial->print("\r\n");
  if (cmd("AT+CIPCLOSE", "OK", "YES", 3000, 1) == AT_OK) {
      return AT_OK;
  }else return AT_NO;
}
bool AiThinker_A7::Send_once(String host,String port,String data){
  if (TCP(host, port)) {
    if (Send(data)) {
      Close();
      return AT_OK;
    }else return AT_NO;
  }else return AT_NO;
}
void AiThinker_A7::at(String cmd){
  BoardSerial->println(cmd);
}



bool AiThinker_A7::SendTextMessage(String Number,String msg){
  if (cmd("AT+CMGF=1", "OK", "YES", 3000, 2) == AT_OK){
    delay(100);
    if (cmd("AT+CMGS=\""+Number+"\"", ">", "YES", 5000, 2) == AT_OK){
      delay(100);
      BoardSerial->println(msg);//the content of the message

      BoardSerial->write(0x1A);//the ASCII code of the ctrl+z is 26

      delay(100);
      return AT_OK;
    }else return AT_NO;
  }else return AT_NO;

}
bool AiThinker_A7::Call_Number(String Number){
  cmd("AT+SNFS=0", "OK", "YES", 20000, 2);//0,切换到耳机 1,切换到听筒
  if (cmd("ATD"+Number, "OK", "YES", 20000, 2) == AT_OK){
    return AT_OK;
  }else return AT_NO;
}
bool AiThinker_A7::Call_Number_Off(){
  if (cmd("ATH", "OK", "YES", 20000, 2) == AT_OK){
    return AT_OK;
  }else return AT_NO;
}


// ****************** The following is a function that has been completed but has not yet been tested

bool AiThinker_A7::heartbeat(String time,String send,String get){
  if (time!="0"){
    if (cmd("AT+CIPHCFG=0,"+time, "OK", "YES", 5000, 2) == AT_OK) {
      if (cmd("AT+CIPHCFG=1,"+send, "OK", "YES", 5000, 2) == AT_OK) {
        if (cmd("AT+CIPHCFG=2,"+get, "OK", "YES", 5000, 2) == AT_OK){
          if (cmd("AT+CIPHMODE=1", "OK", "YES", 5000, 2) == AT_OK){return AT_OK; }
          else return AT_NO;
        }else return AT_NO;
      }else return AT_NO;
    }else return AT_NO;

  }else{
    if (cmd("AT+CIPHMODE=0", "OK", "YES", 5000, 2) == AT_OK){return AT_OK; }  //停止心跳包
    else return AT_NO;

  }

}
bool AiThinker_A7::TC_Start(String times,String delay,int max_size,int wait_timeout){
  if (cmd("AT+CIPTCFG=0,"+times, "OK", "YES", 5000, 2) == AT_OK) {
    if (cmd("AT+CIPTCFG=1,"+delay, "OK", "YES", 5000, 2) == AT_OK) {
      if (cmd("AT+CIPTCFG=2,"+String(max_size), "OK", "YES", 5000, 2) == AT_OK){
        if (cmd("AT+CIPTCFG=3,"+String(wait_timeout), "OK", "YES", 5000, 2) == AT_OK){
          if (cmd("AT+CIPTMODE=1", "OK", "YES", 5000, 2) == AT_OK){return AT_OK; }
          else return AT_NO;
        }else return AT_NO;
      }else return AT_NO;
    }else return AT_NO;
  }else return AT_NO;
}
bool AiThinker_A7::TC_Stop(){
  BoardSerial->println();
  BoardSerial->println("+++");
  delay(500);
  return true;
}
bool AiThinker_A7::TC_Send(String cmd){
  BoardSerial->print(cmd);
  return true;
}
bool AiThinker_A7::TC_Sendln(String cmd){
  BoardSerial->println(cmd);
  return true;
}



//****************** The bottom is reserved but not yet fully implemented
String AiThinker_A7::CCID(){

}
String AiThinker_A7::NameToIP(String ServerName, unsigned long timeOut){
  //String command = "AT+CDNSGIP=\""+ServerName+"\"";
  String command = "AT+CDNSGIP?";

  debug_command(command, timeOut);

  return command;    
}
String AiThinker_A7::AliHTTPDNS(String ServerName){

}
String AiThinker_A7::NameToIP_Plus(String ServerName){

}
String AiThinker_A7::HTTP_GET(String URL){

}
String AiThinker_A7::HTTP_POST(String URL,String data){

}

bool AiThinker_A7::debug_command(String command, unsigned long timeOut) {

  BoardSerial->println(command);
  if (debug_on){
      Serial.print("Command: ");
      Serial.println(command);
  }

  unsigned long entry = millis();
  String reply = "";
  byte retVal = 99;
 
  reply = BoardRead();
  if (debug_on){
    Serial.print("retVal = ");
    Serial.println(retVal);
    Serial.print("data = ");
    Serial.println(reply);
  }

  do {
    reply = BoardRead();
    if ((debug_on)&&(reply != "")) {
      Serial.print((millis() - entry));
      Serial.print(" ms ");
      Serial.print("data = ");
      Serial.println(reply);
    }
  } while ( millis() - entry < timeOut );
 


} 
