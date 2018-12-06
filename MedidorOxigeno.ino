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
    const char ssid[] = "*******";   // Id de red WiFi
    const char pass[] = "*******";   // Password de red WiFi
// ---------------------------------------

void setup() {
  
    Wire.begin();
    Serial.begin(115200);
    Serial.println("Pulsi oxímetro arrancado!");

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

    if( http.begin("http://192.168.0.155:8080/v1/mediciones") ){
      Serial.println( "Iniciada la conexión http" );
      http.addHeader("Content-Type", "application/json");
    } else {
      Serial.println( "No se pudo iniciar la conexión con el servidor" );
    }
    

    delay(5000);
}

void loop() {

  time_t now = time(nullptr);
  
    //You have to call update with frequency at least 37Hz. But the closer you call it to 100Hz the better, the filter will work.
    pulseoxymeter_t result = pulseOxymeter->update();
    
    if( result.pulseDetected == true ){
      
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

    // Montamos el objeto JSON
    String body = "{\"usuario_id\": \"12345678A\",";
    body += "\"tiempo\": \"";
    body += "2018-12-06 10:00:00";
    body += "\",";
    body += "\"pulso\": -1,";
    body += "\"oxigeno\":";
    body += int(result.SaO2);
    body += "}";

    // Realiza la petición POST al servidor    
    int httpCode = http.POST( body );
    Serial.println( httpCode );
    http.writeToStream( &Serial );
    http.end();

    tiempoAnterior=millis();
    lectura = false;
  }
  
}
