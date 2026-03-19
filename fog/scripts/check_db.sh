#!/bin/bash
set -e

cd "$(dirname "$0")/.."

DB="database/fall_larm_demo.db"

if [ ! -f "$DB" ]; then
  echo "Databasen finns inte ännu: $DB"
  exit 1
fi

echo "Visar senaste larm i $DB"
sqlite3 "$DB" <<EOF
.headers on
.mode column
SELECT id, device_id, event, severity, pulse, spo2, temperature, ts, received_at
FROM alarms
ORDER BY id DESC
LIMIT 10;
EOF
