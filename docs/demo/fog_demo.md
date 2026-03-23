# Fog Demo – Raspberry Pi (Fall-Larm)

## Syfte
Denna demo visar fog-lagret i Fall-Larm-systemet:
- Mottagning av larm via MQTT
- Lokal bearbetning
- Lagring i SQLite
- Robust hantering med signaler

---

## Vad är implementerat

### MQTT (kommunikation)
- Raspberry Pi kör MQTT-klient
- Prenumererar på: `fallarm/events`
- Tar emot JSON payload

### Fog Hub (backend)
- Python-applikation
- Modulär struktur:
  - mqtt_client.py
  - alarm_service.py
  - database.py
  - config.py

### Databas
- SQLite
- Tabell: alarms
- Sparar:
  - device_id
  - event
  - severity
  - sensordata
  - timestamp

### Process & signalhantering
- Kör som Linux-process
- Hanterar:
  - SIGINT
  - SIGTERM
- Clean shutdown:
  - stoppar MQTT
  - stänger DB

### Scripts
- run_demo.sh → startar hubben
- publish_test_alarm.sh → simulerar ESP32
- check_db.sh → visar data

---

## Dataflöde (demo)

Simulerat flöde:
ESP32 (simulerad)
↓
MQTT (mosquitto)
↓
Raspberry Pi (fog hub)
↓
SQLite databas


---

## Vad är simulerat

- ESP32 är ersatt av script
- Sensordata är mockad
- MQTT körs lokalt utan TLS
- Databasen är okrypterad

---

## Vad fungerar

- End-to-end dataflöde
- Realtidsmottagning via MQTT
- Lagring i databas
- Processhantering
- Clean shutdown

---

## Vad saknas (framtida arbete)

- Riktig ESP32-integration
- TLS (säker MQTT)
- Krypterad databas (SQLCipher)
- Autentisering
- Systemd service
- Fler sensorer och signalanalys

---

## Slutsats

Fog-lagret är implementerat och fungerar som en robust lokal hub
för att ta emot och hantera fall-larm i realtid.

Systemet är redo att kopplas till riktig hårdvara.
