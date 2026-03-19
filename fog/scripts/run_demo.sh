#!/bin/bash
set -e

cd "$(dirname "$0")/.."

export MQTT_HOST=127.0.0.1
export MQTT_PORT=1883
export MQTT_TOPIC=fallarm/events
export DB_PATH=database/fall_larm_demo.db
export APP_NAME=fallarm-fog-hub

echo "Startar demo-hubben..."
echo "MQTT_HOST=$MQTT_HOST"
echo "MQTT_PORT=$MQTT_PORT"
echo "MQTT_TOPIC=$MQTT_TOPIC"
echo "DB_PATH=$DB_PATH"

source .venv/bin/activate
python3 -m src.main
