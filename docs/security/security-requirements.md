# Syfte
Detta dokument beskriver säkerhetskrav för IoT Fall-Larm-systemet utifrån identifierade hot i threat model. Kraven ska användas som grund för designval, implementation och framtida testning av systemets säkerhet.

# Övergripande mål
Systemet ska skydda känsliga uppgifter, minska risken för manipulation av data och larm, samt begränsa obehörig åtkomst till edge-enhet, kommunikation och fog-hubb. Säkerheten ska utformas så att systemet fortfarande är användbart i en hemmiljö och fungerar även vid tillfälliga nätverksproblem.

# Konfidentialitet
Systemet ska skydda känslig information mot obehörig åtkomst.

## Krav
- WiFi-uppgifter får inte lagras i klartext i publik kod eller öppna konfigurationsfiler.
- API-nycklar och andra hemligheter får inte hårdkodas direkt i källkod.
- Sensordata och larmhändelser ska skyddas mot obehörig läsning.
- Lokalt lagrad data i fog-hubben ska skyddas så att obehöriga inte enkelt kan läsa den.
- Loggar får inte innehålla känsliga hemligheter som lösenord, tokens eller fullständiga nycklar.

# Integritet
Systemet ska skydda data och funktioner från manipulation.

## Krav
- MQTT-meddelanden ska valideras innan de accepteras av fog-hubben.
- Endast godkända enheter ska kunna skicka data som behandlas som giltiga larm eller sensorvärden.
- Payload från edge-enheter ska kontrolleras för korrekt struktur och rimligt innehåll.
- Konfigurationsfiler ska skyddas mot obehörig ändring.
- Systemet ska minska risken för falska larm som skickas från obehöriga enheter eller scripts.

# Tillgänglighet
Systemet ska fortsätta fungera så långt som möjligt även vid fel eller störningar.

## Krav
- Fall-larmssystemet ska vara robust mot tillfälliga nätverksavbrott.
- Fog-hubben ska kunna fortsätta hantera lokal lagring även om extern anslutning saknas.
- Systemet ska prioritera lokal funktion och larmhantering i hemmiljö.
- Fel i en enskild komponent ska inte i onödan slå ut hela systemet.
- Loggning och felhantering ska göra det möjligt att förstå driftproblem utan att exponera känslig data.

# Autentisering och åtkomstkontroll
Systemet ska kunna skilja mellan godkända och obehöriga enheter och användare.

## Krav
- Kommunikation mellan edge och fog ska skyddas med autentisering.
- Varje edge-enhet ska ha ett unikt device-id eller motsvarande identifiering.
- Fog-hubben ska endast acceptera meddelanden från förväntade enheter eller godkända clients.
- Åtkomst till fog-hubben via SSH eller andra administrativa gränssnitt ska begränsas till behöriga användare.
- Tjänster på fog-hubben ska köras med minsta möjliga privilegier.

# Säker lagring
Systemet ska lagra så lite känslig data som möjligt och skydda det som måste sparas.

## Krav
- SQLite-databasen ska skyddas mot obehörig åtkomst.
- Endast nödvändig data ska lagras lokalt.
- Känsliga fält ska krypteras eller hanteras på ett sätt som minskar risken för läckage.
- Backups och exporterad data ska omfattas av samma skydd som den aktiva databasen.
- Lagrad data ska kunna motiveras utifrån systemets funktion.

# Secrets Management
Hemligheter ska hanteras separat från vanlig kod och dokumentation.

## Krav
- WiFi-lösenord, API-nycklar och liknande hemligheter ska lagras utanför publik källkod.
- Repo ska inte innehålla riktiga credentials.
- En tydlig strategi ska finnas för hur hemligheter laddas in i systemet.
- Det ska vara möjligt att byta ut hemligheter utan att skriva om hela applikationen.
- Delning av hemligheter mellan teammedlemmar ska minimeras.

# Hardening av fog-hubb
Raspberry Pi Zero 2 W ska konfigureras så att attackytan minskar.

## Krav
- Onödiga tjänster och portar ska stängas av eller begränsas.
- SSH ska använda säkrare konfiguration, helst nyckelbaserad inloggning.
- Administrativa konton och tjänstekonton ska separeras där det är möjligt.
- Fog-tjänster ska inte köras med högre privilegier än nödvändigt.
- Systemet ska ha grundläggande skydd mot enkel obehörig åtkomst från lokalt nätverk.

# Edge-enhet
Edge-enheten ska utformas för att minska risk vid fysisk åtkomst.

## Krav
- Konfiguration och credentials på edge-enheten ska skyddas så långt som möjligt.
- Systemet ska inte exponera mer information än nödvändigt via debug, serial eller liknande gränssnitt.
- Device identity ska användas för att skilja legitima edge-enheter från obehöriga avsändare.
- Framtida säkerhetsförbättringar som secure boot och signerad firmware ska kunna motiveras i arkitekturen.

# Testbarhet
Säkerhetskrav ska kunna kopplas till praktiska kontroller eller enklare demo-scenarier.

## Krav
- Det ska gå att testa att ogiltiga MQTT-meddelanden nekas eller ignoreras.
- Det ska gå att verifiera att repo inte innehåller riktiga hemligheter.
- Det ska gå att visa att känslig data inte lagras öppet utan skydd.
- Det ska gå att dokumentera vilka krav som är designmål och vilka som faktiskt är implementerade.

# Sammanfattning
Säkerhetskraven för IoT Fall-Larm fokuserar på att skydda hemligheter, säkra kommunikationen mellan edge och fog, skydda lokal lagring och minska attackytan på fog-hubben. Målet är inte att eliminera all risk, utan att bygga ett rimligt och tydligt säkerhetslager för ett IoT-system i hemmiljö.