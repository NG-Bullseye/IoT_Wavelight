/*
 * Test
 */ 
#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
// #define ESPALEXA_DEBUG
#include <Espalexa.h>
#include <IRsend.h>
#include <IRremoteESP8266.h>
// protos
boolean connectWifi();

//callback functions
void firstLightChanged(uint8_t brightness, u32_t rgb);

// Change this!!
const char* ssid = "Free Wifi";
const char* password = "Citizin1570";
const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

boolean wifiConnected = false;

Espalexa espalexa;

// EspalexaDevice* device3; //this is optional

void setup()
{
  irsend.begin();

  Serial.begin(115200);
  // Initialise wifi connection
  wifiConnected = connectWifi();
  
  if(wifiConnected){
    // Define your devices here. 
    // espalexa.addDevice("omega", firstLightChanged, EspalexaDeviceType::onoff); //simplest definition, default state off
    espalexa.addDevice("Wavelight", firstLightChanged); //color device
    // espalexa.addDevice("Light 2", secondLightChanged, 255); //third parameter is beginning state (here fully on)
    // device3 = new EspalexaDevice("Light 3", thirdLightChanged); //you can also create the Device objects yourself like here
    // espalexa.addDevice(device3); //and then add them
    // device3->setValue(128); //this allows you to e.g. update their state value at any time!

    espalexa.begin();
    Serial.println("espalexa begin");
  } else
  {
    while (1) {
      Serial.println("Cannot connect to WiFi. Please check data and reset the ESP.");
      delay(2500);
    }
  }
}
 
uint8_t  oldBrightness;
int counter=0;

int sollZustand=0;
int istZustand=0;
boolean istAn=false;
boolean sollAn=false;

void loop()
{
  espalexa.loop();
  Serial.println("count");
  counter++;
  if (counter>300){
    counter=0;
    if(sollZustand!=istZustand){
      irsend.sendNEC(0xFF40BF,32); 
      istZustand++;
      if(istZustand==8)istZustand=0;
    }
  }
  delay(1);
}

//our callback functions
void firstLightChanged(uint8_t brightness, u32_t rgb) {
  Serial.print("Device 1 changed to ");
  
  //Alexa Wavelight on
  if (brightness==255) {
    if(!istAn){
      irsend.sendNEC(0xFF807F,32); 
      Serial.println("Wavelight Toogle");
      istAn=true;
    }
  }
  //Alexa Wavelight off
  if (brightness==0) {
      irsend.sendNEC(0xFF807F,32); 
      Serial.println("Wavelight Toogle");
      istAn=false;
  }
  switch(rgb){
    case 65280://Alexa Grün
      sollZustand=4;//Lichtmodus Grün
      break;
    case 16711680://Alexa Rot
      sollZustand=2;//Lichtmodus Rot
      break;
    case 255://Alexa Blau
      sollZustand=3;//Lichtmodus Blau
      break;
    case 16777215://Alexa Schwarz
      sollZustand=6; //Lichtmodus Party
      break;
    case 16760972://Alexa Weiß
      if(istAn) {
         irsend.sendNEC(0xFF807F,32); //Lichtmodus Party
         istAn=false;
      }
  }

  Serial.print("Brightness: ");
  Serial.print(brightness);
  Serial.print(", Red: ");
  Serial.print((rgb >> 16) & 0xFF); //get red component
  Serial.print(", Green: ");
  Serial.print((rgb >>  8) & 0xFF); //get green
  Serial.print(", Blue: ");
  Serial.println(rgb & 0xFF); //get blue
  Serial.println(rgb);
  Serial.printf("HEX: %x", rgb);

}

// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false; break;
    }
    i++;
  }
  Serial.println("");
  if (state){
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("Connection failed.");
  }
  return state;
}