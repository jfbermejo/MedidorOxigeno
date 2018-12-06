#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include "MAX30100.h"
#include <time.h>

MAX30100* pulseOxymeter;

int periodo = 20000;                  // tiempo entre envío de medidas
unsigned long tiempoAnterior = 0;     //guarda tiempo de referencia para comparar
bool lectura;

int timezone = 1;
int dst = 0;

HTTPClient http;


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
    lectura = false;

    Serial.print("Conectando a ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.print("Dirección IP asignada: ");
    Serial.println(WiFi.localIP());

    configTime( timezone * 3600 , dst , "pool.ntp.org" , "time.nist.gov" );

    http.begin("https://istic-api.herokuapp.com/v1/mediciones");      //Specify request destination
    http.addHeader("Content-Type", "application/json");

    delay(5000);
}

void loop() {

  time_t now = time(nullptr);
  
    //You have to call update with frequency at least 37Hz. But the closer you call it to 100Hz the better, the filter will work.
    pulseoxymeter_t result = pulseOxymeter->update();
    
    if( result.pulseDetected == true ){
        
//        Serial.print( "BPM: " );
//        Serial.print( result.heartBPM );
      
        Serial.print( "SaO2: " );
        Serial.print( result.SaO2 );
        Serial.println( "%" );
            lectura = true; 
      
    }
    
  if(millis()-tiempoAnterior>=periodo && lectura){     // Si ha transcurrido el periodo programado
    Serial.println(ctime(&now));
    Serial.print( "Oxígeno: " );
    Serial.println( result.SaO2 );
    Serial.println( "Enviando datos" );
    Serial.println();

    String body = "{\"usuarioId\": \"1\",";
    body += "\"tiempo\": \"";
    body += "25-11-2018 10:00";
    body += "\",";
    body += "\"pulso\": 0,";
    body += "\"oxigeno\":";
    body += int(result.SaO2);
    body += "}";

    Serial.println( body );
    
    int httpCode = http.POST( body );
    String payload = http.getString(); //Get the response payload
    Serial.println(httpCode); //Print HTTP return code
    Serial.println(payload); //Print request response payload
    http.writeToStream(&Serial);
    http.end(); //Close connection

    tiempoAnterior=millis();                // Guarda el tiempo actual como referencia
    lectura = false;
  }
  
}
