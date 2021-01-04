#include <Arduino_FreeRTOS.h>
#include <queue.h>

void citireSenzori(void *pvParameters);
void citireButon(void *pvParameters);
void trimitereMesajETH(void *pvParameters);

#include <SPI.h>
#include <Ethernet.h>
#include <dht.h>

struct data
{
  int i;
  char c[20];
};



QueueHandle_t xQueue1 = xQueueCreate( 10, sizeof(data) );

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



int flagButon;


void setup() {
  Serial.begin(115200);
    Ethernet.begin(mac, ip, gateway, subnet);
  // put your setup code here, to run once:

   xTaskCreate(
    citireSenzori
    ,  "Senzori"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

    xTaskCreate(
    trimitereMesajETH
    ,  "ETH"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL ); 
       
     xTaskCreate(
    citireButon
    ,  "Buton"   // A name just for humans
    ,  64  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
   
Serial.println("Setup");
vTaskStartScheduler();

}

void loop() {
  // put your main code here, to run repeatedly:
  

}

void citireButon(void *pvParameters)
{
    (void) pvParameters;
    for(;;)
    {
      //Serial.println("CitireButon");
      if(digitalRead(butonPin) == 0)
      flagButon = 0;
      else
      flagButon = 1;
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
void citireSenzori(void *pvParameters)
{

  (void) pvParameters;
  for(;;)
  {
      Serial.println("CitireSenzori");
    if(flagButon == 0)
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
        Serial.print("Senzori - deTrimis = ");
        Serial.println(deTrimis);
        struct data mesaj;
        
        deTrimis.toCharArray(mesaj.c,deTrimis.length()+1);
        xQueueSend(xQueue1,&mesaj, ( TickType_t )10);
    }
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}

void trimitereMesajETH(void *pvParameters)
{
        (void) pvParameters;
  for(;;)
  {
    String s;
      EthernetClient client;    
      struct data mesaj;
      if(xQueueReceive( xQueue1, &mesaj, (TickType_t)10))
      {
        Serial.print("ETH: ");
        Serial.println(mesaj.c);
      if(client.connect(serverDistant,23))
      {
        Serial.print("ETH - deTrimis = ");
        Serial.println(mesaj.c);
        client.print(mesaj.c); // 
      }
        client.stop();

  }
          vTaskDelay(1000 / portTICK_PERIOD_MS);
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
