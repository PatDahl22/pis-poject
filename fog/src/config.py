import os
from dataclasses import dataclass


@dataclass
class Settings:
    app_name: str
    mqtt_host: str
    mqtt_port: int
    mqtt_topic: str
    db_path: str


def get_settings() -> Settings:
    return Settings(
        app_name=os.getenv("APP_NAME", "fallarm-fog-hub"),
        mqtt_host=os.getenv("MQTT_HOST", "127.0.0.1"),
        mqtt_port=int(os.getenv("MQTT_PORT", "1883")),
        mqtt_topic=os.getenv("MQTT_TOPIC", "fallarm/events"),
        db_path=os.getenv("DB_PATH", "database/fall_larm_demo.db"),
    )
