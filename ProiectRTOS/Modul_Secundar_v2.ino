#include <Arduino_FreeRTOS.h>

#include <SPI.h>
#include <Ethernet.h>
#include <dht.h>


class FIFO //memorie stack fifo
{
 private:
          int index;
          String s[10];

 public:
          String pop()
          {
            if(!isEmpty())
            {
            index--;
            return s[index+1];
            }
            else
            return "FIFO_EMPTY";
          }
          bool push(String q)
          {
            if(index<9)
            {
            index++;
            s[index]=q;
            return true;
            }
            else 
            return false;
          }
          bool isEmpty()
          {
            if(index==-1)
            return true;
            else
            return false;
          }
          FIFO() //constructor
          {
            index = 0;
            for(int i = 0;i<10;i++)
            s[i] = "";
          }       
};

#define DHT11_PIN 7
#define gazPin A0
#define IR_SWPin 3
#define butonPin 4
byte serverDistant[] = { 10, 0, 0, 1 }; //definire IP server distant (modul principal)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xE2 };  
byte ip[] = { 10, 0, 0, 3 };    
byte gateway[] = { 10, 0, 0, 1 };
byte subnet[] = { 255, 255, 255, 0 };
dht DHT;

void citireSenzori(void *pvParameters);
void citireButon(void *pvParameters);
int flagButon;
FIFO coada_de_mesaje;


void setup() {
    Ethernet.begin(mac, ip, gateway, subnet);
  // put your setup code here, to run once:
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
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
    
    xTaskCreate(
    trimitereMesajETH
    ,  "ETH"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
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
      if(digitalRead(butonPin) == 0)
      flagButon = 0;
      else
      flagButon = 1;
      vTaskDelay(100);
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
      deTrimis += getSenzorGaz();
      deTrimis += "*";
      deTrimis += getUmiditate();
      deTrimis += "*";
      if(getIR_SW() == true)
        deTrimis += "1#";
      else
        deTrimis += "0#";
      coada_de_mesaje.push(deTrimis);
    }
    vTaskDelay(2000);
  }
}

void trimitereMesajETH(void *pvParameters)
{
  (void) pvParameters;
  for(;;)
  {
    if(!coada_de_mesaje.isEmpty())
    {
      EthernetClient client;                  
      if(client.connect(serverDistant,23))
        client.print(coada_de_mesaje.pop()); // mesajdeTrimis variabila globala
        client.stop();
    }
  }
}
String getSenzorGaz()
{
    int gaz = analogRead(gazPin);
    String ret;
    ret = String(gaz);
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
