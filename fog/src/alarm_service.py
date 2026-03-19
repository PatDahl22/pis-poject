import json
import logging
from typing import Any, Dict

from src.database import AlarmDatabase


class AlarmService:
    REQUIRED_FIELDS = {"device_id", "event", "severity", "ts"}

    def __init__(self, db: AlarmDatabase) -> None:
        self.db = db
        self.logger = logging.getLogger(self.__class__.__name__)

    def handle_payload(self, payload: Dict[str, Any]) -> None:
        missing = self.REQUIRED_FIELDS - payload.keys()
        if missing:
            raise ValueError(f"Payload saknar fält: {sorted(missing)}")

        self.db.insert_alarm(payload)

        pretty = json.dumps(payload, ensure_ascii=False)
        self.logger.info("Larm sparat i databas: %s", pretty)

        if payload.get("event") == "fall_detected":
            self.logger.warning(
                "FALL-LARM från %s | severity=%s | pulse=%s | spo2=%s | temp=%s",
                payload.get("device_id"),
                payload.get("severity"),
                payload.get("pulse"),
                payload.get("spo2"),
                payload.get("temperature"),
            )
