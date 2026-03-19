# IoT-Fall_Larm

Demo-fokus på Fog Hubben (Raspberry Pi).

## Demo-funktioner
- Tar emot MQTT-meddelanden på `fallarm/events`
- Validerar JSON-payload
- Sparar larm i lokal SQLite-databas
- Loggar till terminal
- Hanterar clean shutdown via SIGINT/SIGTERM

## Körning

```bash
cd fog
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
bash scripts/run_demo.sh

# IoT Projekt - Arkitekturisk Dokumentation

## **Index**
* [1. Projektöversikt](#1-projektöversikt)
* [2. Systemarkitektur](#2-systemarkitektur)
    - [2.1 Hårdvarukomponenter](#21-hårdvarukomponenter)
    - [2.2 Mjukvarukomponenter](#22-mjukvarukomponenter)
    - [2.3 Nätverkskonfiguration](#23-nätverkskonfiguration)
* [3. Designprinciper](#3-designprinciper)
* [4. Utvecklingsprocess](#4-utvecklingsprocess)
* [5. Datahantering](#5-datahantering)
    - [5.1 Datainsamling](#51-datainsamling)
    - [5.2 Databearbetning](#52-databearbetning)
    - [5.3 Datalagring](#53-datalagring)
* [6. Utmaningar-och-lösningar](#6-utmaningar-och-lösningar)


## Fall & Hälsolarm

**Projekttitel:** Fall & Hälsolarm
**Gruppmedlemmar:** Rasmus Pantsari, Daniela Lööw, Sacharias Götesson, Pattaravarat Dahl, Ossian Petermann, Mahdi Ahmadi, Enes Caner Geyve, Rasmus Söberg
**Datum:** 2026-01-12

---

## 1. Projektöversikt

### 1.1 Mål
Målet med detta projekt är att designa och visualisera ett IoT-baserat fallvarningssystem för äldre användare. Systemet syftar till att öka säkerheten och responstiden vid fall genom att automatiskt upptäcka incidenter och meddela utsedda kontakter såsom familjemedlemmar eller vårdgivare.
Projektet fokuserar på systemarkitektur, datakommunikation och säkerhet, med särskild tonvikt på hur IoT-enheter, mobilapplikationer, backend-tjänster och externa aviseringstjänster samverkar inom en robust och tillförlitlig lösning.

### 1.2 Omfattning
Systemet ska i realtid avläsa, lagra biometrics samt bedöma allmäntillstånd för bärare av enheten. Syftet är inte diagnostik likställd av läkare utan enbart indikationer på potentiella hälsorisker baserat på förinställda parametrar.

**Ingår:**
- Fallidentifiering
- Hälsodataövervakning
- Avisering till kontaktpersoner

**Ingår ej**
- Medicinsk diagnos
- Aktiv behandling eller vård

### 1.3 Viktiga Leveranser


---

## 2. Systemarkitektur

### 2.1 Hårdvarukomponenter
Följande hårdvarukomponenter ingår i systemet:
- ESP32 H2 - ingår
- Raspberry Pi zero 2 W - ingår
- Temperatursensor: DS18B20 digital sensor - ingår
- Accelerometer: 3-axlig accelerometer MMA8452Q - ingår ej
- Pulsoximeter: MAX30102 - ingår ej
- GPS-modul
- WiFi / Bluetooth-modul - inbyggt i ESP32, Pi zero
- Analog knapp: 2st Tryckknapp momentan - Ingår iaf en.
- RGB LED: 5050 SMD LED - Ingår
- RFID/NFC: RFID-RC522 - Ingår ej (Men vi har en)
- SD-kort
- Induktionsladdare
- Mikrofon
- Högtalare

### 2.2 Mjukvarukomponenter

### 2.3 Nätverkskonfiguration
Systemet använder en IOT-nätverksarkitektur där edge-enhet(armband) samlar in sensordata ocj skickar all data till lokal gateway(hub). Kommunikation sker via MQTT QoS 2 för pålitligt dataöverföring, medan CoAp används för lätta trigger och kontrollmeddelande. 
Edge-enhet kör ett RTOS som hanterar sensormätning, falldetektering och nätverkskommunikation i realtid. RTOS ser till att larm och viktigt data prioriteras och skickas utan fördröjning. 
Gatewayen fungerar som MQTT-broker, hanterar autentisering och skickar vidare data till molnen via MQTT över TLS och HTTP/REST. Nätverket skyddas med brandvägg, kryptering och identitetsbaserad åtkomstkontroll för att säkerställa säker och pålitligt leverans 
av larmdata.

---

Läs mer om systemarkitektur i [Systemarkitektur-dokumentet](docs/architecture.md). 
 

## 3. Designprinciper

### 3.1 Modularitet

### 3.2 Skalbarhet

### 3.3 Säkerhetsåtgärder

### 3.4 Prestandaoptimering

---

## 4. Utvecklingsprocess

### 4.1 Utvecklingsverktyg och teknologier 

### 4.2 Versionshantering

### 4.3 Testning och kvalitetssäkring

--- 

## 5. Datahantering

### 5.1 Datainsamling
Datan samlas in genom armbandet som är utrustad med sensorer för rörelse och hälsodata. Enheten läser av sensorerna och jämföra värdena mot vissa gränsvärden för att kunna upptäcka avvikelser eller fall.
För att spara data används en cirkulär buffer i RAM där det senaste datan lagras tillfälligt. Vissa datapunkter sparas även i krypterat flashminne via ESP-IDF. Alla insamlade data skickas vidare till hubben där den bearbetas och lagras.

### 5.2 Databearbetning
Databehandling sker båda i enhet och hubben.
Edge-enhet analyserar data löpande och gör snabba beslut baserat på gränsvärden, detta minskar fördröjning och gör larmet snabbare. 
Hubben tar emot datan, kontrollerar det och lagrar den i en krypterad SQLite-database. Där kan datan också sorteras och prioriteras om det behövs. För att kunna visa historik i appen sammanställs data i tidsperioder, då kan appen enkelt skapa grafer och överskikter

### 5.3 Datalagring
Datalagring sker på två nivåer: lokalt på edge enheten och centralt på hubben.

#### Lokal lagring på edge
Edge enheten använder en cirkulär buffer i RAM för att tillfälligt lagra sensordata. Det möjliggör:
- Realtidsanalys
- Sabb dataåtkomst vid falldetektering
- Minskad belastning på flashminnet

Endas relevant data skrivs över till flashminnet, datapunkter från förbestämda intervaller samt händelser. 
Detta för att minska slitage på flashminnet på grund av begränsade skrivcykler. 

#### Central lagring i hub (fog)
Hubben lagrar persistent data i en krypterad SQLite databas. Av datanintegritet och sekretes skäl använder vi:
- SQLCipher för kryptering av databasen
- Libsodium för säker hantering av kryptografiska nycklar

Databasen är strukturerad med tabeller för:
- Användardata
- Sensorvärden
- Händelser (Fall och larm)
- Ehets loggar

Diagram för datahantering finns i [Datahanterings-dokumentet](docs/data-handling.md).
---

## 6. Utmaningar och lösningar
Under projektets gång identifierade vi flera tekniska och arkitektoriska utmaningar.

### Realtidskrav och latens
Utmaing i att systemet måste reagera snabbt vid event då fördröjningar kan innebära brister i säkerheten.

Lösningen var att lokal analys på edge nivå med hjälp av RTOS. Genom att köra sensorhantering och fallidentifiering i separata tasks kan systemet prioritera kritiska funktioner för att minska latens.

### Begränsade resurser på edge enheten
Enheten har begränsad RAM, CPU och flashminne. Vi löste detta genom:

- Cirkulär buffer i RAM minnet möjliggör begränsa mängden aktuell data utan att behöva lagra onödiga datapunkter under längre perioder
- Selektivt skriva över enbart relevanta datapunkter till flashminnet

### Datasäkerhet och intigritet
Systemet hanterar känslig data som måste skyddas, detta uppnås genom:

- Krypterad komunikation
- Krypterad SQLite-databas genom SQLCipher
- Säker nyckehantering med Libsodium
- Separation av rådata och användaridentifierande information

### Kalibrering av komponenter
Efter lite testning så visade det sig att vissa sensorer kräver kalibrering för att ge rimliga värden.

Ett exempel på detta är kalibreringen av MAX30102:
* Vi behövde justera LED strömmen för att ADC'n inte skulle bli mättad, men även för att kompensera för signalbrus
    - Ljus från omgivningen
    - Variation i blodflöde
    - Finger tryck
    - Temperaturförändringar
    - Hudfärg
* Det behövdes en AC/DC-separation för att isolera den pulserande komponenten i sensorn från den statiska    vävnadsabsorbationen.
* Valideringen gjordes sedan genom en jämförelse mot ett aktivitetsarmband (Fitbit), som vi använde som ett basvärde när vi försökte öka nogrannheten på sensorn.

### Komunikation mellan enheter
Kommunikationen mellan enheterna har vi löst på förljande vis:

* Den primära dataöverföringen kommer att ske med MQTT över wifi, där fog enheten fungerar MQTT-broker och tar emot data via specifika topics.
* IEEE 802.15.4 kommer att vara på hela tiden för att lyssna på styrkommandon som skickas över CoAP från vår fog enhet med jämna mellanrum (data requests) eller vid behov (checkin). Men Edge enheten ska även kunna skicka CoAP meddelanden tillbaka för att bekräfta att fog enheten är inom räckvidd. Det kommer även att användas för att trigga uppstart av WIFI så vi kan överföra data från edge -> fog på ett säkert sätt (MQTT + TLS).
    - QoS 2 kommer att användas för att säkerställa att datan kommit fram innan den tas bort från Edge för att spara lagringsutrymme.
* TLS-kryptering används vid MQTT-kommunikation för att skydda data mellan edge och fog från avlyssning och manipulation
* BLE kommer att användas för provisioning för följande:
    - SSID
    - Lösenord
    - Device ID
* BLE kommunikationen kommer att ske med Security 1 (X25519 + AES-CTR) för att ha en säker överföring.
* Vid flera användare av fall-larmet på samma fog, så kommer kommunikationen endast att ske mellan edge och fog, ej mellan edge enheterna.
---

## 7. Resultat och diskussion

### 7.1 Projektresultat

### 7.2 Analys av resultat

### 7.3 Lärdomar

---

## 8. Framtida arbete

---

## 9. Referenser

---

## 10. Bilagor

