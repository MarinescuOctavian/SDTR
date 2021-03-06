#include <Arduino_FreeRTOS.h>

#include <SPI.h>
#include <Ethernet.h>
#include <dht.h>

#define DHT11_PIN 7
#define fumPin A0
#define IR_SWPin 3
byte serverDistant[] = { 10, 0, 0, 1 }; //definire IP server distant (modul principal)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xE2 };  
byte ip[] = { 10, 0, 0, 3 };    
byte gateway[] = { 10, 0, 0, 1 };
byte subnet[] = { 255, 255, 255, 0 };
dht DHT;

void citireSenzori(void *pvParameters);
void citireButon(void *pvParameters);
int flagButon;

void setup() {
    Ethernet.begin(mac, ip, gateway, subnet);
    xTaskCreate(
    citireSenzori
    ,  "Senzori"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

    xTaskCreate(
    citireButon
    ,  "Buton"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

}

void loop() {
  // put your main code here, to run repeatedly:
}

void citireButon(void *pvParameters)
{
    (void) pvParameters;
    for(;;)
    {
      delay(100);
    }
}
void citireSenzori(void *pvParameters)
{
  (void) pvParameters;
  for(;;)
  {
    if(flagButon == 1)
    {
      String deTrimis = "*";
      deTrimis += getSenzorFum();
      deTrimis += "*";
      deTrimis += getUmiditate();
      deTrimis += "*";
      if(getIR_SW() == true)
        deTrimis += "1#";
      else
        deTrimis += "0#";
      EthernetClient client;                  
      if(client.connect(serverDistant,23))
        client.print(deTrimis);
        client.stop();
        delay(5000);
    }
  }
}

String getSenzorFum()
{
    int fum = analogRead(fumPin);
    String ret;
    ret = String(fum);
    return ret;
}

String getUmiditate()
{
  int chk = DHT.read11(DHT11_PIN);
  float r = DHT.humidity;
  String ret;
  ret = String(r);
  return ret;
}

bool getIR_SW()
{
  return digitalRead(IR_SWPin);
}
