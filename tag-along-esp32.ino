#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "./config.h"
#include <Buzzer.h>

MFRC522 mfrc522(SS_PIN, RST_PIN);
Buzzer buzz(BUZZ);

const char* ssid = ESSID;
const char* password = PASSWD;

void LED(int pin, bool state);
void InitWiFi(bool action);
void feedback(int res);

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

  //Serial.begin(115200);
}


void loop()
{
  if (WiFi.status() != WL_CONNECTED)
    InitWiFi(1);

  if (!mfrc522.PICC_IsNewCardPresent())
    return;

  if (!mfrc522.PICC_ReadCardSerial())
    return;

  String tag(""), tag255;
  byte letter;

  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    tag255 = String(mfrc522.uid.uidByte[i], HEX);
    tag255.toUpperCase();
    tag += (mfrc522.uid.uidByte[i] < 0x10 ? " 0x0" : " 0x") + tag255;
  }

  tag = tag.substring(1);

  if (tag.substring(3) != "")
  {
    //Serial.println(tag);
    buzz.ok(100);
    WiFiClient client;
    HTTPClient http;
    http.begin(client, "http://192.168.168.183:6868/log");
    http.addHeader("uuid", UUID);
    http.addHeader("rfid-tag", tag);
    feedback(http.POST(""));
    http.end();
    delay(1000);
  }

}


void LED(int pin, bool state) {
  digitalWrite(pin, MODE == "ANODE" ? !state : state);
}

void InitWiFi(bool action)
{
  if (action) {
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

void feedback(int res)
{
  switch (res) {
    case 200:
      delay(100);
      LED(LED_OK, 1);
      buzz.ok(500);
      LED(LED_OK, 0);
      break;
      
    case 400:
      delay(100);
      LED(LED_NOK, 1);
      buzz.error(500, BUZZER_ERROR);
      LED(LED_NOK, 0);
      break;
      
    default:
      LED(LED_OK, 1);
      LED(LED_NOK, 1);
      for (int i = 0; i < 3 ; i++)
      {
        buzz.passive(400, BUZZER_TIMEOUT);
        delay(200);
      }
      LED(LED_OK, 0);
      LED(LED_NOK, 0);
      break;
  }
}
