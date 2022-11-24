#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

HTTPClient sender;
WiFiClient wifiClient;

// WLAN-Daten
const char *ssid = "";               // Wifi-Name
const char *password = "";           // Wifi-Passwort
const unsigned int LED_PIN = 5;      // GPIO Pin-Nummer
const unsigned int DELAY_TIME = 5;   // Millisekunden
const unsigned int MIN_LEVEL = 0;    // Minimum-Wert
const unsigned int MAX_LEVEL = 255;  // Maximum-Wert
unsigned int CURRENT_PWM_VALUE = 0;  // Aktueller PWM-Wert (0-255)


void setup() {
  // Legt die Datenrate in Bit pro Sekunde (Baud) für die serielle Datenübertragung fest.
  Serial.begin(115200);
  
  //Verbindung zum Wifi aufbauen
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }

  Serial.println("Verbunden!");

  // LED-PIN als Output spezifizieren
  pinMode(LED_PIN,OUTPUT); 
}

void loop() {
  // Exemplarisch wird der Gesamt-PV-Überschuss von der Steuerung des Soyosource Wechselrichters abgegriffen
  // Kann von jedem anderen Wlan fähigem Gerät bezogen werden (zB Shelly3em)
  const char dataUrl = "http://192.168.0.55/data";
  if (sender.begin(wifiClient, dataUrl)) {

      // HTTP-Code der Response speichern
      int httpCode = sender.GET();

      if (httpCode > 0) {
        
        // Anfrage wurde gesendet und Server hat geantwortet
        // Info: Der HTTP-Code für 'OK' ist 200
        if (httpCode == HTTP_CODE_OK) {

          // Hier wurden die Daten vom Server empfangen

          // String vom Webseiteninhalt speichern
          String payload = sender.getString();

          // Parse response
          DynamicJsonDocument doc(2048);
          deserializeJson(doc, payload);

          //total-power in Watt (tp) aus der json response ziehen
          float tP = doc["L1L2L3"];
          
          Serial.println("Total Power in Watt");
          Serial.println(tP);

          Serial.println("Aktueller PWM-Wert");
          Serial.println(CURRENT_PWM_VALUE);

          unsigned int i = CURRENT_PWM_VALUE;

          unsigned int j;

          /**
          Fall 1
          total-power kleiner gleich -50 Watt
          Erhöhe den PWM-Wert um 10
          */
          if(tP <= -50){
            if(i <= MAX_LEVEL){
              j = i + 10;
              for(i; i < j; i++) // [0,255]
                analogWriteDelay(LED_PIN,i,DELAY_TIME);
                CURRENT_PWM_VALUE = j;
            }
          /**
          Fall 2
          total-power zwischen 0 und -50 Watt
          Verringere den PWM-Wert um 10
          */
          } else if(tP < 0 && tP > -50){
            j = i - 10;
            if(i < 10){
              j = 0; 
            }
            if(i != 0){
              for(i; i > j; i--){
                  analogWriteDelay(LED_PIN,i,DELAY_TIME);
                  CURRENT_PWM_VALUE = j;
              }
            }
          /**
          Fall 3
          total-power 0 Watt oder größer 0 Watt
          Verringere den PWM-Wert sofort bis auf 0
          */
          } else {
            j = 0;
            if(i != 0){
              for(i; i > j; i--){
                  analogWriteDelay(LED_PIN,i,DELAY_TIME);
                  CURRENT_PWM_VALUE = j;
              }
            }
          }
         
          Serial.println("--- Durchlauf ende ---");
        
      }else{
        // Falls HTTP-Error
        Serial.printf("HTTP-Error: ", sender.errorToString(httpCode).c_str());
      }

    }else {
      Serial.printf("HTTP-Verbindung konnte nicht hergestellt werden!");
    
      
    }

    delay(2000);
  }
}

void analogWriteDelay(unsigned int pin, 
                      unsigned int value, 
                      unsigned int waitTime) {
    analogWrite(pin,value); // Schreibt einen PWM-Wellen-Wert (value) auf einen Pin (pin)
    delay(waitTime);        // Warten für waitTime (in Millisekunden) 
}
