/*
 * This is a basic example on how to use Espalexa and its device declaration methods.
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
void secondLightChanged(uint8_t brightness);
void thirdLightChanged(uint8_t brightness);

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
void loop()
{
  // irsend.sendNEC(0xF7B04F,32); 
   espalexa.loop();
  //  Serial.println("loop");
   delay(1);
}

//our callback functions
void firstLightChanged(uint8_t brightness, u32_t rgb) {
  Serial.print("Device 1 changed to ");
  
  //do what you need to do here

  //EXAMPLE
  if (brightness==255) {
    Serial.print("ON, brightness ");
    Serial.println(brightness);
    irsend.sendNEC(0xFF807F,32); 
    Serial.println("Wavelight On");
  }
  if (brightness==0) {
    Serial.print("OFF, brightness ");
    Serial.println(brightness);
    irsend.sendNEC(0xFF807F,32); 
    Serial.println("Wavelight Off");
  }
  else if (rgb==65280) {
    irsend.sendNEC(0xF740BF,32); 
    Serial.println("Wavelight Grün");
  }
  else if (rgb==16711680) {
    irsend.sendNEC(0xF720DF,32); 
    Serial.println("Wavelight Red");
  }
  else if (rgb==255) {
    irsend.sendNEC(0xF7609F,32); 
    Serial.println("Wavelight Blau");
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

  if (rgb == 255){
    Serial.println("rot");
  }
}

void secondLightChanged(uint8_t brightness) {
  //do what you need to do here
}

void thirdLightChanged(uint8_t brightness) {
  //do what you need to do here
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