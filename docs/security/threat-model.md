# Syfte
Kartlägga vilka hot som finns mot IoT Fall-Larm-systemet och identifiera vilka delar som behöver skyddas för att minska risk för dataläckage, manipulation och obehörig åtkomst.

# Tillgångar
- WiFi-uppgifter
- API-nycklar
- sensordata
- larmhändelser
- lokalt lagrad data
- konfigurationsfiler
- loggar och systemstatus

# Angripare
- person med fysisk tillgång till edge-enheten
- person med tillgång till lokalt nätverk
- obehörig användare med åtkomst till fog-hubben
- angripare som försöker skicka falska meddelanden till systemet

# Attackytor
- ESP32 / edge
- MQTT-kommunikation
- Raspberry Pi / fog hub
- SQLite-databas
- konfigurationsfiler
- SSH och andra exponerade tjänster på fog-hubben

# Möjliga konsekvenser
- läckta WiFi-uppgifter kan ge åtkomst till lokalt nätverk
- läckta API-nycklar kan användas för obehörig kommunikation
- manipulerade larmhändelser kan skapa falska larm eller dölja riktiga fall
- obehörig åtkomst till fog-hubben kan ge tillgång till databas, loggar och systemfunktioner
- ändrade konfigurationsfiler kan styra om trafik eller försvaga säkerheten

# Prioriterade hot
## Hög risk
- fysisk åtkomst till edge-enheten
- MQTT utan TLS eller autentisering
- okrypterad lagring i SQLite
- hårdkodade hemligheter i kod eller konfigurationsfiler

## Medel risk
- manipulation av konfigurationsfiler
- svag SSH-konfiguration på fog-hubben
- otillräcklig loggning eller övervakning

# Exempel på attackscenarier
1. En angripare ansluter till det lokala nätverket och avlyssnar MQTT-trafik.
2. En angripare får fysisk tillgång till edge-enheten och försöker läsa ut sparade uppgifter.
3. En obehörig användare får åtkomst till Raspberry Pi och läser den lokala SQLite-databasen.
4. En angripare skickar falska MQTT-meddelanden som ser ut som riktiga larm.
5. En angripare ändrar en konfigurationsfil för att omdirigera kommunikation eller försvaga systemet.

# Antaganden
- systemet används i hemmiljö
- edge-enheten kan vara fysiskt åtkomlig
- fog-hubben körs på lokalt nätverk
- internetuppkoppling kan vara instabil eller saknas
- systemet hanterar känslig hälsorelaterad information