# Projektstruktur och Implementationsplan

Denna plan beskriver den övergripande katalogstrukturen för "Fall &
Hälsolarm"-projektet och går igenom de specifika behoven för både Edge-enheten
(ESP32 H2) och Fog Hubben (Raspberry Pi Zero 2 W).

## Övergripande Katalogstruktur

I projektets rotmapp föreslår vi följande struktur för att tydligt separera
koden för de olika systemdelarna:

```text
IoT-Fall_Larm-main/
│
├── edge/                  # Kod och konfiguration för Edge-enheten (ESP32 H2)
│   ├── src/               # Källkod (main.c, RTOS-tasks, sensorkod)
│   ├── include/           # Header-filer (.h)
│   ├── components/        # ESP-IDF-komponenter för externa sensorer (MAX30102, MMA8452Q etc.)
│   ├── test/              # Lokala enhetstester (Unity för ESP-IDF)
│   ├── CMakeLists.txt     # Bygginstruktioner för ESP-IDF
│   └── sdkconfig.defaults # Standardkonfiguration för ESP32 (RTOS, WiFi, BLE)
│
├── fog/                   # Kod och konfiguration för Fog Hubben (Raspberry Pi)
│   ├── src/               # Källkod för hantering av MQTT, databas och CoAP (t.ex. Python/Node.js)
│   ├── config/            # Konfigurationsfiler (.env, TLS-certifikat)
│   ├── database/          # databasscheman för SQLite/SQLCipher
│   ├── scripts/           # Skript för systemd, uppstart och nätverkskonfiguration
│   └── test/              # Tester för hubbens backend och databasanrop
│
├── shared/                # Gemensamma resurser
│   └── protocols/         # Specifikationer/schemas för MQTT payload och CoAP (t.ex. JSON-schema eller Protobuf)
│
├── tests/                 # End-to-end (E2E) integrationstester edge <-> fog
│
├── docs/                  # Dokumentation (Arkitektur, API, Datahantering)
├── README.md              # Övergripande projektbeskrivning
└── .gitignore             # Git ignoreringar
```

---

## 1. Edge-enhet (Armband - ESP32 H2)

Edge-enheten är ansvarig för realtidsinsamling, falldetektering och säker
kommunikation. Den kommer troligtvis att byggas med **ESP-IDF** (Espressif IoT
Development Framework) i C/C++.

### Modulära mjukvarubehov

- **Sensordrivrutiner (`edge/components/`):**
  - I2C/SPI-drivrutiner för MAX30102 (Pulsoximeter) och MMA8452Q
    (Accelerometer).
  - 1-Wire för DS18B20 (Temperatur).
  - GPIO för momentana knappar och RGB LED.
- **RTOS Tasks (`edge/src/`):**
  - **Sensor Task:** Läser av data kontinuerligt med exakt timing och sparar i
    RAM (cirkulär buffer).
  - **Algorithm Task:** Kör falldetekteringsalgoritmen lokalt.
  - **Network Task:** Hanterar WiFi, MQTT (QoS 2 + TLS) och skickar larm.
  - **CoAP Task:** Lystnar på 802.15.4 för inkommande `data requests` eller
    "check-ins" från Fog Hub.
  - **BLE Provisioning Task:** Säker BLE (Security 1, X25519) för onboarding
    (SSID, lösenord, ID).
- **Lokal Lagring:** Hantering av flashminne (NVS / SPIFFS) för att endast spara
  nödvändigt datapunkter för att undvika slitningar.

---

## 2. Fog Hub (Gateway - Raspberry Pi Zero 2 W)

Fog Hubben fungerar som det lokala nätverkets hjärna. Den tar emot, säkrar,
lagrar och vidarebefordrar data. Mjukvaran här kan skrivas i t.ex. **Python**,
**Node.js** eller **Go**.

### Modulära mjukvarubehov

- **MQTT Broker (t.ex. Mosquitto):**
  - Måste konfigureras snabbt via systemd med TLS påslaget för säker
    dataöverföring.
- **Backend Applikation (`fog/src/`):**
  - **MQTT Client/Subscriber:** Konsumerar larm och sensordata från
    Edge-enheten.
  - **CoAP Server:** Skickar regelbundna "check-in"-signaler via IEEE 802.15.4
    och triggar Edge att starta WiFi vid behov.
  - **Cloud Forwarding:** Vidarebefordrar larm via molnet (MQTT TLS eller
    HTTP/REST).
- **Lagring ochSäkerhet (`fog/database/`):**
  - Integration av **SQLite** krypterad med **SQLCipher**.
  - Kryptografiska nycklar hanteras med **Libsodium**.
  - Databasstrukturer för Användardata, Sensordata (tidsserier), Händelser och
    Loggar.
- **Enhetshantering:**
  - Systemd-tjänster för att se till att hubben alltid körs igång vid
    strömbortfall eller omstart (`fog/scripts/`).
