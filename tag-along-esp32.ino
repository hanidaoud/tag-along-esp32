#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "./config.h"


MFRC522 mfrc522(SS_PIN, RST_PIN);

const char* ssid = ESSID;
const char* password = PASSWD;


void InitWiFi(bool action)
{
  if (action == 1) {
    WiFi.disconnect();
  }
  WiFi.begin(ssid, password);
    Serial.println("Reconnecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED_WiG, MODE == "ANODE" ? LOW : HIGH);
      digitalWrite(LED_WiR, MODE == "ANODE" ? LOW : HIGH);
      delay(125);
      digitalWrite(LED_WiG, MODE == "ANODE" ? HIGH : LOW);
      digitalWrite(LED_WiR, MODE == "ANODE" ? HIGH : LOW);
      delay(125);
    }
    digitalWrite(LED_WiG, MODE == "ANODE" ? LOW : HIGH);
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
        digitalWrite(LED_OK, MODE == "ANODE" ? LOW : HIGH);
        doBuzz(0, 500); //900);
        digitalWrite(LED_OK, MODE == "ANODE" ? HIGH : LOW);
        break;
      case 400:
        delay(100);
        digitalWrite(LED_NOK, MODE == "ANODE" ? LOW : HIGH);
        doBuzz(100, 500);
        digitalWrite(LED_NOK, MODE == "ANODE" ? HIGH : LOW);
        break;
      default:
        digitalWrite(LED_OK, MODE == "ANODE" ? LOW : HIGH);
        digitalWrite(LED_NOK, MODE == "ANODE" ? LOW : HIGH);
        doBuzz(750, 500);
        digitalWrite(LED_OK, MODE == "ANODE" ? HIGH : LOW);
        digitalWrite(LED_NOK, MODE == "ANODE" ? HIGH : LOW);
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
  digitalWrite(LED_OK, MODE == "ANODE" ? HIGH : LOW);
  digitalWrite(LED_NOK, MODE == "ANODE" ? HIGH : LOW);
  
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
