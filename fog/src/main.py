import logging
import os
import signal
import sys
import time
from threading import Event

from src.alarm_service import AlarmService
from src.config import get_settings
from src.database import AlarmDatabase
from src.mqtt_client import MqttAlarmClient


stop_event = Event()


def configure_logging() -> None:
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)s | %(levelname)s | %(name)s | %(message)s",
        stream=sys.stdout,
    )


def handle_shutdown(signum, frame) -> None:
    logging.getLogger("main").info("Signal mottagen: %s. Startar clean shutdown...", signum)
    stop_event.set()


def main() -> int:
    configure_logging()
    logger = logging.getLogger("main")
    settings = get_settings()

    logger.info("Startar %s", settings.app_name)
    logger.info("PID: %s", os.getpid())

    db = AlarmDatabase(settings.db_path)
    alarm_service = AlarmService(db)

    client = MqttAlarmClient(
        host=settings.mqtt_host,
        port=settings.mqtt_port,
        topic=settings.mqtt_topic,
        on_payload=alarm_service.handle_payload,
    )

    signal.signal(signal.SIGINT, handle_shutdown)
    signal.signal(signal.SIGTERM, handle_shutdown)

    try:
        client.start()
        logger.info("Hubben kör. Väntar på MQTT-larm...")
        while not stop_event.is_set():
            time.sleep(1)
    except Exception as exc:
        logger.exception("Kritiskt fel i hubben: %s", exc)
        return 1
    finally:
        try:
            client.stop()
        except Exception:
            logger.exception("Kunde inte stoppa MQTT-klienten korrekt")
        db.close()
        logger.info("Hubben är stoppad")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
