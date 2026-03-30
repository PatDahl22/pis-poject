import json
import logging
from typing import Callable

import paho.mqtt.client as mqtt


class MqttAlarmClient:
    def __init__(
        self,
        host: str,
        port: int,
        topic: str,
        on_payload: Callable[[dict], None],
    ) -> None:
        self.host = host
        self.port = port
        self.topic = topic
        self.on_payload = on_payload
        self.logger = logging.getLogger(self.__class__.__name__)

        self.client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
        self.client.on_connect = self._on_connect
        self.client.on_message = self._on_message
        self.client.on_disconnect = self._on_disconnect

    def start(self) -> None:
        self.logger.info(
            "Ansluter till MQTT-broker %s:%s och topic '%s'",
            self.host,
            self.port,
            self.topic,
        )
        self.client.connect(self.host, self.port, keepalive=60)
        self.client.loop_start()

    def stop(self) -> None:
        self.logger.info("Stoppar MQTT-klient")
        self.client.loop_stop()
        self.client.disconnect()

    def _on_connect(self, client, userdata, flags, reason_code, properties) -> None:
        self.logger.info("MQTT ansluten, reason_code=%s", reason_code)
        client.subscribe(self.topic)
        self.logger.info("Prenumererar på topic '%s'", self.topic)

    def _on_disconnect(self, client, userdata, disconnect_flags, reason_code, properties) -> None:
        self.logger.info("MQTT frånkopplad, reason_code=%s", reason_code)

    def _on_message(self, client, userdata, msg) -> None:
        raw = msg.payload.decode("utf-8")
        self.logger.info("Meddelande mottaget på %s: %s", msg.topic, raw)

        payload = json.loads(raw)
        self.on_payload(payload)
