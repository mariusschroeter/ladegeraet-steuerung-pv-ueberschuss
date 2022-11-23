# Batterie laden mit Photovoltaik-Überschuss (Wemos D1 Mini Flash)

## Hilfreiche Videos zu diesem Thema
1. https://www.youtube.com/watch?v=WK9PQ1_TpU8&ab_channel=DerKanal
2. https://www.youtube.com/watch?v=TAW5yowh12U&ab_channel=BavarianSuperGuy

## Benutzte Hardware
1. MW Netzteil (LED dimmbares Ladegerät)
2. Wemos D1 Mini (Sendet PWM Signal an Optokoppler)
3. Optokoppler (Wandelt das Signal des D1 Mini von 3,3V zu 10V um

![ladegeraet-steuerung](https://user-images.githubusercontent.com/43613156/203628502-100ae913-ec1d-4175-af68-53630858ecf7.png)

## Code benutzen und Netzgerät steuern
1. Wifi-Namen und Wifi-Password eingeben, damit der Wemos eine Verbindung zum Wifi aufbauen kann
2. GPIO Pin-Nummer eintragen
```
const char *ssid = "";               // Wifi-Name
const char *password = "";           // Wifi-Passwort
const unsigned int LED_PIN = 5;      // GPIO Pin-Nummer
```
3. dataUrl anpassen (von wo kann die aktuelle total-power abgegriffen werden?) für ein response objekt
```
const char dataUrl = "http://192.168.0.55/data"
```
4. total-power wert aus der Reponse ziehen (in meinem Fall doc["L1L2L3"])

<img width="704" alt="Bildschirmfoto 2022-11-23 um 19 46 36" src="https://user-images.githubusercontent.com/43613156/203624655-fd8c8bcc-9c7f-4774-a21e-e2723ddbe0f5.png">

```
float tP = doc["L1L2L3"];
```

5. Wert festlegen ab welcher totalPower (tp) Strom in der Batterie gespeichert werden soll (in meinem Fall ab tp <= -50)

```
if (tP <= -50)
```

6. Wert festlegen ab welcher totalPower (tp) Strom NICHT in der Batterie gespeichert werden soll (in meinem Fall ab tp >= -45)

```
if (tP >= -45)
```
