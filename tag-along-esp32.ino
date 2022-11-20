#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "./config.h"


MFRC522 mfrc522(SS_PIN, RST_PIN);

const char* ssid = ESSID;
const char* password = PASSWD;

void LED(int pin, bool state) {
  if (state) {
    digitalWrite(pin, MODE == "ANODE" ? LOW : HIGH);
  }
  else if (! state) {
    digitalWrite(pin, MODE == "ANODE" ? HIGH : LOW);
  }
}

void InitWiFi(bool action)
{
  if (action == 1) {
    WiFi.disconnect();
  }
  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      LED(LED_WiG, 1);
      LED(LED_WiR, 1);
      delay(125);
      LED(LED_WiG, 0);
      LED(LED_WiR, 0);
      delay(125);
    }
    LED(LED_WiG, 1);
}

void doBuzz(int freq, int dur)
{
  if (freq == 0)
  {
    digitalWrite(BUZZ, HIGH);
    delay(dur);
    digitalWrite(BUZZ, LOW);
    return;
  }
  tone(BUZZ, freq);
  delay(dur);
  noTone(BUZZ);
}

void feedback(int res)
{
  switch(res){
      case 200:
        delay(100);
        LED(LED_OK, 1);
        doBuzz(0, 500);
        LED(LED_OK, 0);
        break;
      case 400:
        delay(100);
        LED(LED_NOK, 1);
        doBuzz(100, 500);
        LED(LED_NOK, 0);
        break;
      default:
        LED(LED_OK, 1);
        LED(LED_NOK, 1);
        doBuzz(750, 500);
        LED(LED_OK, 0);
        LED(LED_NOK, 0);
        break;
    }
}

void setup()
{
  pinMode(BUZZ, OUTPUT);
  pinMode(LED_WiR, OUTPUT);
  pinMode(LED_WiG, OUTPUT);
  pinMode(LED_OK, OUTPUT);
  pinMode(LED_NOK, OUTPUT);
  LED(LED_OK, 0);
  LED(LED_NOK, 0);
  
  InitWiFi(0);
  
  SPI.begin();
  mfrc522.PCD_Init();
}


void loop() 
{
  if (WiFi.status() != WL_CONNECTED) { 
      InitWiFi(1);
  }
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  
  String tag = "";
  byte letter;
  String tag255;
  
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    tag255 = String(mfrc522.uid.uidByte[i], HEX);
    tag255.toUpperCase();
    tag += (mfrc522.uid.uidByte[i] < 0x10 ? " 0x0" : " 0x") + tag255;
  }
  
  tag = tag.substring(1);
  
  if (tag.substring(3) != "")
  {
    doBuzz(0, 100);
    WiFiClient client;
    HTTPClient http;
    http.begin(client, String("http://") + IP_ADDRESS + "/log");     
    http.addHeader("uuid", UUID);
    http.addHeader("rfid-tag", tag);
    feedback(http.POST(""));
    http.end();
    delay(1000);
  }
  
} 
