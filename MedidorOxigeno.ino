#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "MAX30100.h"

MAX30100* pulseOxymeter;

// ========== CREDENCIALES WIFI ==========
const char ssid[] = "iPhone de Juan";  // Id de red WiFi
const char pass[] = "4n9xz4dxko30z";   // Password de red WiFi
// ---------------------------------------

void setup() {
  
    Wire.begin();
    Serial.begin(115200);
    Serial.println("Pulse oxymeter test!");

    pulseOxymeter = new MAX30100();
    pinMode(2, OUTPUT);

    Serial.print("Conectando a ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.print("Dirección IP asignada: ");
    Serial.println(WiFi.localIP());
    
}

void loop() {
  
    //You have to call update with frequency at least 37Hz. But the closer you call it to 100Hz the better, the filter will work.
    pulseoxymeter_t result = pulseOxymeter->update();
    
    if( result.pulseDetected == true ){
      
        Serial.println("BEAT");
        
        Serial.print( "BPM: " );
        Serial.print( result.heartBPM );
      
        Serial.print( "  -  SaO2: " );
        Serial.print( result.SaO2 );
        Serial.println( "%" );
      
    }
  
}
