#include <Arduino_FreeRTOS.h>

#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define gazMinim 100
#define gazMaxim 300
#define umiditateMinim 100
#define umiditateMaxim 300
#define IR_SWNormal 0
#define alarmaPin 9
//umiditate, fum, ir

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xE1 };  
byte ip[] = { 10, 0, 0, 1 };    
byte gateway[] = { 10, 0, 0, 10 };
byte subnet[] = { 255, 255, 255, 0 };
EthernetServer server(23);
LiquidCrystal_I2C lcd(0x3F, 16, 2); //declararea variabilei pentru afis LCD
  


void setup() {
  Serial.begin(115200);
  pinMode(alarmaPin,OUTPUT);
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();

  lcd.begin();
  lcd.backlight();

    xTaskCreate(
    checkMessage
    ,  "Verificare Mesaje"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

}

void loop() {


}

void checkMessage(void *pvParameters)
{
      Serial.println("test00");
      (void) pvParameters;
  for(;;)
  {

    String senzorUmiditate = "";
    String senzorGaz = "";
    String senzorIR_SW = "";
    EthernetClient client = server.available();  //Declarare variabila client pentru verificarea mesajelor trimise de module in mod aleator
  if (client) {
    String s;
    while(client.available())
    {
      char c = client.read();
      if((int)c>=33 && (int)c<=126)
      s+=c;
    }
    client.stop();
  Serial.println(s); 
      int i = 1;
  while(s[i]!='*' && i<s.length()){
    senzorGaz+=s[i];i++;}
  i++;
  while(s[i]!='*' && i<s.length()){
    senzorUmiditate+=s[i];i++;}
  i++;  
  while(s[i]!='#' && i<s.length()){
    senzorIR_SW+=s[i];i++;}

    bool val = false;
    if(senzorGaz.toInt()<gazMinim || senzorGaz.toInt()>gazMaxim)
    val = true;
    if(senzorUmiditate.toInt()<umiditateMinim || senzorUmiditate.toInt()>umiditateMaxim)
    val = true;
    if(senzorIR_SW.toInt() != IR_SWNormal)
    val = true;
    if(val == true)
    analogWrite(alarmaPin,255);
    else
    analogWrite(alarmaPin,0);
    lcd.clear();
    lcd.print(senzorGaz);lcd.print(" ");
    lcd.print(senzorUmiditate);lcd.print(" ");
    lcd.print(senzorIR_SW);lcd.print(" ");  
  }
  vTaskDelay(100/ portTICK_PERIOD_MS);
}
}
