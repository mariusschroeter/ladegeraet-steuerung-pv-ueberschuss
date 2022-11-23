#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// Bibliotheken um sich zum Wifi zu verbinden +
// einen get Request ausführen zu können
HTTPClient sender;
WiFiClient wifiClient;

// WLAN-Daten
const char *ssid = "";               // Wifi-Name
const char *password = "";           // Wifi-Passwort
const unsigned int LED_PIN = 5;      // GPIO Pin-Nummer
const unsigned int DELAY_TIME = 5;   // Millisekunden
const unsigned int MIN_LEVEL = 0;    // Minimum-Wert
const unsigned int MAX_LEVEL = 1023; // Maximum-Wert
unsigned int CURRENT_VALUE = 0;      // Aktueller Wert

void setup()
{
  // Legt die Datenrate in Bit pro Sekunde (Baud) für die serielle Datenübertragung fest.
  Serial.begin(115200);

  // Zum Wifi verbinden
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print(".");
  }

  Serial.println("Verbunden!");

  // Konfiguriert den spezifizierten Pin als Input oder Output.
  pinMode(LED_PIN, OUTPUT);
}

void loop()
{
  if (sender.begin(wifiClient, "http://192.168.0.55/data"))
  {

    // HTTP-Code der Response speichern
    int httpCode = sender.GET();

    if (httpCode > 0)
    {

      // Anfrage wurde gesendet und Server hat geantwortet
      // Info: Der HTTP-Code für 'OK' ist 200
      if (httpCode == HTTP_CODE_OK)
      {

        // Hier wurden die Daten vom Server empfangen

        // String vom Webseiteninhalt speichern
        String payload = sender.getString();

        // Parse response
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, payload);

        // tP = totalPower aus dem Json Objekt (doc)
        float tP = doc["L1L2L3"];

        unsigned int i = CURRENT_VALUE;

        // Fall1: totalPower kleiner gleich -50
        if (tP <= -50)
        {
          if (i < 1013)
          {
            unsigned int j = i + 100;
            for (i; i < j; i++)
            {
              Serial.print(i);
              analogWriteDelay(LED_PIN, i, DELAY_TIME);
              CURRENT_VALUE = j;
            }
          }
        }
        // Fall2: totalPower größer gleich -45
        if (tP >= -45)
        {
          for (i; i > MIN_LEVEL; i--)
          {
            analogWriteDelay(LED_PIN, i, DELAY_TIME);
          }
        }
      }
      else
      {
        // Falls HTTP-Error
        Serial.printf("HTTP-Error: ", sender.errorToString(httpCode).c_str());
      }
    }
    else
    {
      Serial.printf("HTTP-Verbindung konnte nicht hergestellt werden!");
    }

    delay(2000);
  }
}

void analogWriteDelay(unsigned int pin,
                      unsigned int value,
                      unsigned int waitTime)
{
  analogWrite(pin, value); // Schreibt einen PWM-Wellen-Wert (value) auf einen Pin (pin)
  delay(waitTime);         // Warten für waitTime (Millisekunden)
}
