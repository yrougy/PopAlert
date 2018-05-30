// LoRa Beacon
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Arduino9x_TX
 
#include <SPI.h>
#include <RH_RF95.h>
 
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
 
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 868.0
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
 
// Blinky on receipt
//#define LED 13

/*
  PopAlert Balise

  Code d'exploitation d'une balise PopAlert

*/



// Pinage
const int button_victim = 5;
const int button_safe =  6;
const int led_alert = 4;
const int led_victim = 8;
const int led_safe = 3;

// variables will change:
int state = 0;         // Etat de la balise. 0 NSPP, 1 victim, 2 safe
int button_victim_state = 0;
int button_safe_state = 0;
int alert_state = 0;
int led_alert_last_blink = 0;
int led_alert_blink_rank = 0;
char* msg;
 
void setup() 
{
  //pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  while (!Serial);
  Serial.begin(9600);
  delay(100);
 
  Serial.println("Arduino LoRa RX Test!");
  
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
  rf95.setTxPower(23, false);
    // Les boutons sont des entrées
  pinMode(button_victim, INPUT);
  pinMode(button_safe, INPUT);
  
  // Les leds sont des sorties
  pinMode(led_alert, OUTPUT);
  pinMode(led_victim, OUTPUT);
  pinMode(led_safe, OUTPUT);
  
  digitalWrite(led_alert, LOW);
  digitalWrite(led_safe, LOW);
  digitalWrite(led_victim, LOW);
  
  //alert_state = 1;
}
 
void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {
      //digitalWrite(LED, HIGH);
      //RH_RF95::printBuffer("Received: ", buf, len);
      //Serial.print("Got: ");
      //Serial.println((char*)buf);
      buf[5]=0;
      msg = (char*) buf;
      Serial.println(msg);
      if (msg[0] == 'A' )
      {
          alert_state = 1;
      }
      //Serial.print("RSSI: ");
      //Serial.println(rf95.lastRssi(), DEC);
      
      // Send a reply
      //uint8_t data[] = "And hello back to you";
      //rf95.send(data, sizeof(data));
      //rf95.waitPacketSent();
      //Serial.println("Sent a ACK");
      //digitalWrite(LED, LOW);
      
      
    }
    else
    {
      Serial.println("Receive failed");
    }
  }

  // Gestion du front
  // Gestion des boutons
  button_victim_state = digitalRead(button_victim);
  button_safe_state = digitalRead(button_safe);
  
  if (button_victim_state == HIGH){
    uint8_t data[] = "RESCUE";
    rf95.send(data, sizeof(data));
    rf95.waitPacketSent();
    Serial.println("Sent a ACK");
    state = 1;
  }else if (button_safe_state == HIGH){
    uint8_t data[] = "SAFE";
    rf95.send(data, sizeof(data));
    rf95.waitPacketSent();
    Serial.println("Sent a ACK");
    state = 2;
  }
  
  if (state == 0){
    digitalWrite(led_victim, LOW);
    digitalWrite(led_safe, LOW);
  }else if (state == 1){
    digitalWrite(led_victim, HIGH);
    digitalWrite(led_safe, LOW);
  }else if (state == 2){
    digitalWrite(led_victim, LOW);
    digitalWrite(led_safe, HIGH);
  }
  
  if (alert_state == 1){
    if((millis() - led_alert_last_blink) >= 800){
      if(led_alert_blink_rank % 2 == 0){
        digitalWrite(led_alert, LOW);
      }else{
        digitalWrite(led_alert, HIGH);
      }
      led_alert_last_blink = millis();
      led_alert_blink_rank++;
    }
  }else{
    digitalWrite(led_alert, LOW);
  }
}
