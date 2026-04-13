# Syfte
Detta dokument beskriver hur kommunikationen mellan edge-enheter (ESP32) och fog-hubben (Raspberry Pi) säkras. Fokus ligger på att skydda mot obehörig åtkomst, falska meddelanden och manipulation av data.

# Nuläge
I nuvarande implementation används MQTT som kommunikationslager mellan edge och fog.

- MQTT broker körs lokalt på fog-hubben
- Kommunikation sker utan TLS
- Meddelanden skickas via topics (t.ex. fallarm/events)
- Payload innehåller sensordata och larmhändelser

# Risker
Följande säkerhetsrisker har identifierats:

- Trafik kan avlyssnas (sniffing) eftersom den inte är krypterad
- Obehöriga enheter kan ansluta till MQTT-brokern
- Falska larm kan skickas av angripare
- Replay attacks där gamla meddelanden skickas igen
- Payload kan manipuleras utan att upptäckas
- Okända clients kan publicera på samma topics

# Säkerhetsmål
Målet är att säkerställa att:

- Endast godkända enheter kan skicka data
- Meddelanden är korrekta och inte manipulerade
- Fog-hubben inte accepterar falska larm
- Kommunikation kan skyddas mot avlyssning (på sikt)
- Systemet är robust även utan full TLS-implementation

# Säkerhetsåtgärder (Design)

## 1. Validering av device-id
- Varje edge-enhet ska ha ett unikt device-id
- Fog-hubben kontrollerar att device-id är känt innan meddelandet accepteras
- Okända enheter ignoreras eller loggas

## 2. Validering av MQTT topics
- Endast definierade topics får användas (t.ex. `fallarm/events`)
- Fog-hubben ska inte acceptera godtyckliga topics
- Topics ska struktureras tydligt per enhet vid behov

## 3. Payload-validering
- Payload måste följa en definierad struktur (t.ex. JSON)
- Obligatoriska fält ska kontrolleras (device_id, timestamp, typ av event)
- Ogiltiga eller ofullständiga meddelanden ignoreras

## 4. Skydd mot falska meddelanden
- Meddelanden från okända device-id ska ignoreras
- Felaktig struktur eller värden ska inte behandlas som riktiga larm
- Fog-hubben ska logga misstänkta försök

## 5. Begränsning av access till broker
- MQTT-brokern ska endast vara tillgänglig inom lokalt nätverk
- Onödiga portar och extern access ska undvikas
- Autentisering (username/password) kan införas som första steg

## 6. TLS (framtida förbättring)
- MQTT över TLS bör användas för att skydda mot sniffing
- Certifikatbaserad autentisering kan införas vid högre säkerhetskrav
- I detta projekt behandlas TLS som en designförbättring, inte full implementation

# Enkel test / Proof of Concept

Följande tester kan användas för att verifiera säkerheten:

## Test 1 – giltigt meddelande
- Skicka ett korrekt MQTT-meddelande från edge
- Förväntat resultat: meddelandet accepteras och lagras

## Test 2 – okänd device
- Skicka ett meddelande med ett okänt device-id
- Förväntat resultat: meddelandet ignoreras eller loggas

## Test 3 – ogiltig payload
- Skicka ett meddelande med saknade eller felaktiga fält
- Förväntat resultat: meddelandet ignoreras

## Test 4 – falskt larm
- Skicka ett meddelande som inte följer formatet för riktiga larm
- Förväntat resultat: systemet ska inte behandla det som ett giltigt fall

# Sammanfattning
MQTT-kommunikationen är en central del av IoT Fall-Larm-systemet och utgör en viktig attackyta. Genom att validera device-id, topics och payload samt begränsa åtkomst till brokern kan systemet skyddas mot många vanliga attacker, även utan fullständig kryptering. TLS och starkare autentisering kan införas som framtida förbättringar.