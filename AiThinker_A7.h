//
//    FILE: AiThinker_A7.h
//  AUTHOR: Flyrainning
// VERSION: 0.1.1
// PURPOSE: Ai-Thinker A6 A7 A20 lib for Arduino
//     URL: http://www.fengpiao.net
//
//


#include <Arduino.h>
#include <SoftwareSerial.h>

#ifndef AiThinker_A7_h
#define AiThinker_A7_h


#define AT_OK 1
#define AT_NO 0
#define AT_TIMEOUT 3
#define AT_RST 2

class AiThinker_A7{

private:
  int PIN_Power;
  int PIN_Reset;
  int PIN_RX;
  int PIN_TX;


  char buffer[100];


  bool debug_on=false;

public:
  SoftwareSerial* BoardSerial=NULL;
  AiThinker_A7();
  ~AiThinker_A7();
  AiThinker_A7(int RX,int TX,int Power=0,int Reset=0);

  // Get the ccid of the SIM card
  String CCID();
  // pin set
  void setup(int RX,int TX,int Power=0,int Reset=0);
  /*
  Start the module
  Baudrate baud rate with module communication
  Reset Whether to call _start () to reset the module settings
  */
  void begin(long baudrate=9600,bool reset=true);
  // start the module, need to set the power pin to be effective
  void power_on();
   /*
  Reset the module, need to set the Reset pin to be effective
  After reset, call bigin to reinitialize the module
  */
  void reset();
  /*
  Into the serial port through the debug mode, you can directly through the serial port through the command to the module
  Used for debugging, no exit after entering
  */
  void serial_debug();
  // start debugging information output, output to the serial port
  void debug(bool is_on=true);
  // execute a command
  byte cmd(String command, String response1, String response2, int timeOut=500, int repetitions=2);
  byte waitFor(String response1, String response2, unsigned long timeOut);
  String BoardRead();
  // initialize the A6 module and prepare the at environment
  bool _start();

  bool Operator();

  bool SetAPN(String APN);
  
  // initialize the GPRS environment and start GPRS
  bool GPRS_Start(String APN);
  // Create a TCP link
  bool TCP(String host,String port);
  // Create a UDP link
  bool UDP(String host,String port);
  // After creating a link (TCP or UDP), send a piece of data
  bool Send(String data);
  // disconnect the link (TCP or UDP)
  bool Close();
  // send data at once, including the following actions: link, send, close
  bool Send_once(String host,String port,String data);
  // simply send an at command to the module without waiting for a return
  void at(String cmd);
  /*
  Start the heartbeat pack
  Time heartbeat interval, unit seconds, time = 0 stop heartbeat pack
  Send heartbeat packet to send the contents of the hexadecimal mark, the length can not be less than 3 bytes
  Get set to accept the contents of the response packet, the length can not be less than 3 bytes, hexadecimal format

  This command can only be used after the server is connected
  */
  bool heartbeat(String time="30",String send="FFFAAFFF",String get="FFFAAFFF");
  /*
  Turn on transparent transmission
  Times indicates the maximum number of failed attempts to send 0-5
  Delay is the retransmission delay in milliseconds 0-3000
  Max_size is the packet length that is triggered to send 10-100
  Wait_timeout is the trigger send time in milliseconds 1000-8000, from the last input of the characters, extended to more than the system will automatically send data

  This command can only be used after the server is connected
  More than 1 minute no data transmission, the gateway may automatically cut off the connection, if the transmission data frequency is too low, you should first open the heartbeat package
  */
  bool TC_Start(String times="3",String delay="300",int max_size=100,int wait_timeout=1000);
  //Stop the pass
  bool TC_Stop();
  // Transparent send data
  bool TC_Send(String data);
  // Transfers send data, wraps version
  // Note: the line feed is only an additional carriage return waiting to send, and will not directly trigger the send, still need to wait for TC_Start in the trigger conditions will be sent after the data
  bool TC_Sendln(String data);

   // Send SMS
  bool SendTextMessage(String Number,String msg);
  //dial number
  bool Call_Number(String Number);
  //hang up the phone
  bool Call_Number_Off();


  // Advanced function


  // Through the domain name query out IP, domain name query also need traffic, the need for multiple connections, you can first query out ip, through the ip to establish a connection
  String NameToIP(String ServerName, unsigned long timeOut);
  // Through Ali cloud HTTPDNS get server ip
  String AliHTTPDNS(String ServerName);
   // first try to get from the Ali cloud HTTPDNS, after the failure to use regular DNS to get
  String NameToIP_Plus(String ServerName);
  /*
  HTTP get, return to the server to send the content
  A one-time connection will disconnect the previous connection
  */
  String HTTP_GET(String URL);
  /*
  HTTP post, back to the server to send the content
  A one-time connection will disconnect the previous connection
  */
  String HTTP_POST(String URL,String data);

  bool debug_command(String command, unsigned long timeOut);

};



#endif
