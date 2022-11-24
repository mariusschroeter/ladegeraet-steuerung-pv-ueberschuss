#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

HTTPClient sender;
WiFiClient wifiClient;

// WLAN-Daten
const char* ssid = "";
const char* password = "";
const unsigned int LED_PIN    = 5; // GPIO pin number
const unsigned int DELAY_TIME = 5; // Milliseconds
const unsigned int MIN_LEVEL  = 0;
const unsigned int MAX_LEVEL  = 255; //1023
unsigned int CURRENT_VALUE = 0;

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("Verbunden!");
 pinMode(LED_PIN,OUTPUT); // Set LED pin as output
}

void loop() {
  //läuft nach setup
  if (sender.begin(wifiClient, "http://192.168.0.55/data")) {

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

          //get totalPower=tP from json obj doc
          float tP = doc["L1L2L3"];
          
          Serial.println("Total Power");
          Serial.println(tP);

          Serial.println("Aktueller Wert");
          Serial.println(CURRENT_VALUE);

          unsigned int i = CURRENT_VALUE;

          unsigned int j;

          if(tP <= -50){ // 1 --- (+10)
            if(i <= 255){
              j = i + 10;
              for(i; i < j; i++) // [0,255]
                analogWriteDelay(LED_PIN,i,DELAY_TIME);
                CURRENT_VALUE = j;
            }
          } else if(tP < 0 && tP > -50){ // 2 --- (-10) 
            j = i - 10;
            if(i < 10){
              j = 0; 
            }
            if(i != 0){
              for(i; i > j; i--){
                  analogWriteDelay(LED_PIN,i,DELAY_TIME);
                  CURRENT_VALUE = j;
              }
            }
          } else { // 3 --- (auf 0)
            j = 0;
            if(i != 0){
              for(i; i > j; i--){
                  analogWriteDelay(LED_PIN,i,DELAY_TIME);
                  CURRENT_VALUE = j;
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
    analogWrite(pin,value); // Analog write in LED pin
    delay(waitTime);        // Delay in milliseconds  
}
