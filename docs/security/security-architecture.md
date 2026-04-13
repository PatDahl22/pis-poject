# Syfte
Detta dokument beskriver säkerhetsarkitekturen för IoT Fall-Larm-systemet. Arkitekturen utgår från systemets edge–fog-modell och visar hur säkerhet byggs in i varje del av lösningen: edge-enhet, kommunikationslager, fog-hubb och datalagring.

# Bakgrund
IoT Fall-Larm är ett system där en edge-enhet samlar in sensordata och identifierar möjliga fallhändelser. Händelser skickas vidare till en fog-hubb som tar emot, validerar, bearbetar och lagrar data lokalt. Eftersom systemet hanterar känslig information och kan vara fysiskt åtkomligt i hemmiljö måste säkerheten utformas med flera lager av skydd.

# Säkerhetsprinciper
Arkitekturen bygger på följande principer:

- defense in depth
- least privilege
- secure by design
- minimera attackyta
- dataminimering
- lokal robusthet även utan internet
- tydlig separation mellan komponenter och ansvar

# Systemöversikt
Säkerhetsarkitekturen omfattar fyra huvuddelar:

1. Edge-enhet (ESP32)
2. MQTT-kommunikation mellan edge och fog
3. Fog-hubb (Raspberry Pi Zero 2 W)
4. Lokal datalagring i SQLite

Varje del har egna risker och egna skyddsåtgärder.

# 1. Edge-enhet (ESP32)

## Roll
Edge-enheten samlar in sensordata och genererar larmhändelser som skickas till fog-hubben.

## Risker
- fysisk åtkomst till enheten
- läsning av lagrade credentials
- manipulation av konfiguration
- falsk identitet eller obehörig enhet som skickar data
- exponering via debug, serial eller andra gränssnitt

## Säkerhetsmål
- skydda credentials och konfiguration så långt som möjligt
- minska mängden känslig data som lagras lokalt
- använda en tydlig device identity
- begränsa exponering av interna gränssnitt
- kunna motivera framtida skydd som secure boot och signerad firmware

## Designval
- Edge-enheten ska inte innehålla hårdkodade hemligheter i källkod som delas i repo.
- Varje edge-enhet ska ha ett unikt device-id som används vid kommunikation med fog-hubben.
- Payload som skickas från edge ska vara enkel, tydlig och validerbar.
- Om konfiguration behöver lagras lokalt ska den skyddas bättre än vanlig hårdkodad text.
- Framtida förbättringar kan inkludera säkrare provisioning, secure boot och starkare skydd av firmware.

# 2. MQTT-kommunikation

## Roll
MQTT används som kommunikationslager mellan edge och fog.

## Risker
- avlyssning av trafik
- spoofing av enheter
- falska larmmeddelanden
- replay eller återuppspelning av gamla meddelanden
- ogiltiga payloads som ändå behandlas som riktiga

## Säkerhetsmål
- säkerställa att endast godkända enheter kan skicka data
- validera innehåll innan det accepteras
- minska risken för falska larm och manipulerade meddelanden
- på sikt skydda trafiken bättre mot avlyssning

## Designval
- Fog-hubben ska validera topic, device-id och payload innan ett meddelande accepteras.
- MQTT-topics ska vara tydligt definierade och inte öppna för godtycklig användning.
- Trafiken bör på sikt skyddas med TLS eller motsvarande säkrare transport.
- Endast förväntade clients ska kunna publicera på kritiska topics.
- Meddelanden som saknar korrekt struktur eller kommer från okänd avsändare ska ignoreras eller loggas som avvikelse.

# 3. Fog-hubb (Raspberry Pi Zero 2 W)

## Roll
Fog-hubben tar emot data från edge, validerar meddelanden, lagrar information lokalt och fungerar som lokal besluts- och logiknod.

## Risker
- obehörig åtkomst via SSH eller andra tjänster
- lokala användare med för höga privilegier
- läsning eller manipulation av databas och loggar
- onödiga tjänster som ökar attackytan
- osäker driftkonfiguration

## Säkerhetsmål
- minska attackytan på operativsystemet
- separera administrativa funktioner från applikationstjänster
- begränsa privilegier
- skydda loggar, databas och konfiguration
- göra systemet robust utan att öppna för enkel åtkomst

## Designval
- Fog-hubben ska hardenas genom att onödiga tjänster stängs av eller begränsas.
- SSH ska konfigureras säkrare, helst med nyckelbaserad inloggning.
- Applikationstjänster ska köras med minsta möjliga privilegier.
- Administrativ åtkomst ska begränsas till behöriga användare.
- Loggning ska stödja felsökning utan att läcka hemligheter eller känslig data.

# 4. Lokal datalagring (SQLite)

## Roll
SQLite används för att lagra larmhändelser och relaterad lokal data i fog-hubben.

## Risker
- databasen läses av obehörig användare
- känslig data lagras i klartext
- backup eller kopior hanteras osäkert
- mer data än nödvändigt sparas

## Säkerhetsmål
- skydda känslig data mot obehörig läsning
- minska mängden lagrad information
- tydligt motivera varför viss data behöver sparas
- skapa bättre förutsättningar för framtida kryptering eller säkrare lagringsmodell

## Designval
- Endast nödvändig data ska lagras lokalt.
- Känsliga fält ska identifieras och skyddas bättre än vanlig metadata.
- Databasen ska inte användas som plats för att lagra hemligheter i klartext.
- Om kryptering inte är fullt implementerad ännu ska det dokumenteras som ett säkerhetsgap och framtida förbättring.
- Exporter, backup och kopior ska ses som lika känsliga som originaldata.

# 5. Secrets Management

## Roll
Systemet använder hemligheter som WiFi-uppgifter, API-nycklar eller andra credentials för kommunikation och drift.

## Risker
- credentials ligger i repo
- credentials lagras i klartext
- credentials återanvänds eller delas osäkert
- svårt att rotera hemligheter när något läcker

## Säkerhetsmål
- separera secrets från vanlig kod
- undvika riktiga credentials i repo
- förenkla säker konfiguration
- göra det möjligt att byta secrets utan stora kodändringar

## Designval
- Hemligheter ska hanteras separat från vanlig källkod.
- Repo ska endast innehålla exempelkonfiguration eller placeholders, inte riktiga värden.
- Fog-hubben ska använda en tydlig config-strategi, exempelvis separat lokal konfiguration.
- Secrets på edge ska hanteras med större försiktighet än övriga parametrar.
- Hanteringen ska dokumenteras så att teamet använder samma arbetssätt.

# 6. Loggning och övervakning

## Roll
Loggning behövs för felsökning, testning och upptäckt av avvikelser.

## Risker
- loggar innehåller känsliga uppgifter
- loggar används inte för att upptäcka avvikande beteende
- incidenter blir svåra att förstå i efterhand

## Säkerhetsmål
- logga relevanta säkerhetshändelser
- undvika att logga hemligheter
- kunna se när ogiltiga meddelanden eller obehöriga försök sker

## Designval
- Fog-hubben ska logga ogiltiga MQTT-meddelanden och andra misstänkta händelser.
- Loggar ska inte innehålla fullständiga hemligheter eller onödigt känslig information.
- Loggning ska användas som stöd för både drift och enklare säkerhetsanalys.

# Arkitekturell sammanfattning
Säkerhetsarkitekturen för IoT Fall-Larm bygger på att varje lager skyddas utifrån sin roll i systemet. Edge-enheten ska begränsa exponering och identifiera sig tydligt. MQTT-kommunikationen ska valideras och på sikt skyddas bättre med starkare transportskydd. Fog-hubben ska hardenas och köras med minsta möjliga privilegier. Lokal lagring ska skyddas och känsliga uppgifter ska minimeras. Secrets ska hanteras separat från vanlig kod.

# Nuvarande läge och nästa steg
I nuvarande projekt finns redan en tydlig edge–fog-arkitektur, MQTT-kommunikation och lokal lagring i fog-hubben. Säkerhetsarbetet i denna dokumentation beskriver hur den befintliga lösningen kan stärkas.

Prioriterade nästa steg är:
- validering av MQTT topic, device-id och payload
- förbättrad hantering av secrets
- bättre skydd av lokal datalagring
- hardening av fog-hubben
- dokumentation av vad som är implementerat nu och vad som är designmål för senare iterationer