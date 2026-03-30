#!/bin/bash
set -e

TOPIC="fallarm/events"
TS=$(date -u +"%Y-%m-%dT%H:%M:%SZ")

PAYLOAD=$(cat <<EOF
{"device_id":"esp32-demo","event":"fall_detected","severity":"high","pulse":88,"spo2":97,"temperature":36.8,"ts":"$TS"}
EOF
)

echo "Publicerar testlarm till $TOPIC"
echo "$PAYLOAD"

mosquitto_pub -h 127.0.0.1 -p 1883 -t "$TOPIC" -m "$PAYLOAD"
echo "Klart"
