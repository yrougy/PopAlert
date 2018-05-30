
// LoRa Base Station
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example LoRa9x_RX
 
#include <SPI.h>
#include <RH_RF95.h>
 
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
 
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 868.0
 
// Blinky on receipt
#define LED 13

// Pinage;
const int led_victim = 5;
const int led_safe = 4;
const int button_alert =  6;
int button_alert_state = 0;

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
 
void setup() 
{
  pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  while (!Serial);
  Serial.begin(9600);
  delay(100);
 
  Serial.println("Arduino LoRa TX Test!");
 
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(20, false);
  pinMode(led_victim, OUTPUT);
  pinMode(led_safe, OUTPUT);
  
  digitalWrite(led_safe, LOW);
  digitalWrite(led_victim, LOW);
  pinMode(button_alert, INPUT);
}
 
int16_t packetnum = 0;  // packet counter, we increment per xmission
 
void loop()
{
  button_alert_state = digitalRead(button_alert);
  if (button_alert_state == HIGH)
    { 
    Serial.println("Sending Alert Message");
    // Send a message to rf95_server
    
    char radiopacket[20] = "ALERT";
    itoa(packetnum++, radiopacket+13, 10);
    Serial.print("Sending "); Serial.println(radiopacket);
    radiopacket[19] = 0;
    
    Serial.println("Sending..."); delay(10);
    rf95.send((uint8_t *)radiopacket, 20);
   
    //Serial.println("Waiting for packet to complete..."); delay(10);
    rf95.waitPacketSent();
    }  
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
 
  //Serial.println("Waiting for ACK from beacon..."); //delay(10);
  //if (rf95.waitAvailableTimeout(100))
  //{ 
    // Should be a reply message for us now   
    //if (rf95.recv(buf, &len))
   //{
     // Serial.print("Got ACK: ");
    //  Serial.println((char*)buf);
    //  Serial.print("RSSI: ");
    //  Serial.println(rf95.lastRssi(), DEC);    
    //}
    //else
    //{
    //  Serial.println("Receive failed");
    //}
  //}
  //else
  //{
  //  Serial.println("No reply, is there a listener around?");
  //}
  //delay(1000);

  // ALERT Send
  // Waiting for feedback from beacon
  //pinMode(RFM95_RST, OUTPUT);
  //digitalWrite(RFM95_RST, HIGH);
  //if (rf95.available())
  //{
    // Should be a message for us now   
//    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
//    uint8_t len = sizeof(buf);
      if (rf95.recv(buf, &len))
      {
        digitalWrite(LED, HIGH);
        //RH_RF95::printBuffer("Received: ", buf, len);
        //Serial.print("Got: ");
        //Serial.println((char*)buf);
        //Serial.print("RSSI: ");
        //Serial.println(rf95.lastRssi(), DEC);
      if ((char*)buf[0] == 'R' )
      {
          digitalWrite(led_safe, LOW);
          digitalWrite(led_victim, HIGH);
      }
      if ((char*)buf[0] == 'S' )
      {
          digitalWrite(led_safe, HIGH);
          digitalWrite(led_victim, LOW);
      }
        
        
      }
      //else
      //{
      //  Serial.println("Receive failed");
      //}
    //}
    //else
    //{
    //    Serial.println("BUSY");
    //  }
    //delay(1000);
    
}

