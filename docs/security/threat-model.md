# Threat Model

## Syfte
Kartlägga vilka hot som finns mot IoT Fall-Larm-systemet.

## Tillgångar
- WiFi-uppgifter
- API-nycklar
- sensordata
- larmhändelser
- lokalt lagrad data

## Angripare
- person med fysisk tillgång till edge-enheten
- person med tillgång till lokalt nätverk
- obehörig användare med åtkomst till fog-hubben

## Attackytor
- ESP32 / edge
- MQTT-kommunikation
- Raspberry Pi / fog hub
- SQLite-databas
- konfigurationsfiler