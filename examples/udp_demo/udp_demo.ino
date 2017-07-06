#include "AiThinker_A7.h"

#define GPRS_Serial_RX 7
#define GPRS_Serial_TX 8
#define GPRS_Power_pin 9
#define GPRS_Reset_pin 10

AiThinker_A7 Board(GPRS_Serial_RX, GPRS_Serial_TX,GPRS_Power_pin,GPRS_Reset_pin); // RX, TX

void setup() {
  Serial.begin(115200);

  Board.debug();
  Board.begin(9600);
  // Start the network
  Board.GPRS_Start();

  // 4, send UDP data
  Board.UDP("www.fengpiao.net","9001");
  for (size_t i = 0; i < 10; i++) {
    Board.Send("I'm Data");
    delay(2000);
  }


  //shut down
  Board.Close();
  
}
void loop(void) {


}
