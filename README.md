# AiThinker_A7
An ESP8266/Arduino library for communicating with the Ai-Thinker A7 ( A6 ) GSM module

Arduino's GSM mobile phone module library for Ai-Thinker A6 A7 A20 series chips, theoretically also support other devices using AT commands.

Provide SMS, call, GPRS networking (TCP UDP TCP data transmission) and other functions.

```
#include "AiThinker_A7.h"

#define GPRS_Serial_RX 7
#define GPRS_Serial_TX 8
#define GPRS_Power_pin 9
#define GPRS_Reset_pin 10

AiThinker_A7 Board(GPRS_Serial_RX, GPRS_Serial_TX,GPRS_Power_pin,GPRS_Reset_pin); // RX, TX

void setup() {
  Serial.begin(115200);

  //Board.debug();
  Board.begin(9600);
  // Start the network
  Board.GPRS_Start();

  // 1, send the data once:
  Board.Send_once("www.fengpiao.net","9001","I'm Data");
  
  // 2, send multiple data:
  // establish connection
  Board.TCP("www.fengpiao.net","9001");
  for (size_t i = 0; i < 10; i++) {
    Board.Send("I'm Data");
    delay(2000);
  }
  // close the connection
  Board.Close();
  
  // 3, using transparent mode:
  //establish connection
  Board.TCP("www.fengpiao.net","9001");
  // Enable heartbeat pack once every 60 seconds
  Board.heartbeat("60");
  // open through
  Board.TC_Start();

  for (size_t i = 0; i < 10; i++) {
    Board.TC_Send("I'm Data");
    delay(2000);
  }
  // exit transparent mode
  Board.TC_Stop();
  
  
  // 4, send UDP data
  Board.UDP("www.fengpiao.net","9001");
  for (size_t i = 0; i < 10; i++) {
    Board.Send("I'm Data");
    delay(2000);
  }
  //shut down
  Board.Close();
  
}


```
