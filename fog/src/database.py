import sqlite3
import threading
from pathlib import Path
from typing import Any, Dict, List, Tuple


class AlarmDatabase:
    def __init__(self, db_path: str) -> None:
        self.db_path = db_path
        Path(db_path).parent.mkdir(parents=True, exist_ok=True)
        self.conn = sqlite3.connect(db_path, check_same_thread=False)
        self.lock = threading.Lock()
        self._create_tables()

    def _create_tables(self) -> None:
        with self.lock:
            cursor = self.conn.cursor()
            cursor.execute(
                """
                CREATE TABLE IF NOT EXISTS alarms (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    device_id TEXT NOT NULL,
                    event TEXT NOT NULL,
                    severity TEXT NOT NULL,
                    pulse REAL,
                    spo2 REAL,
                    temperature REAL,
                    ts TEXT NOT NULL,
                    received_at DATETIME DEFAULT CURRENT_TIMESTAMP
                )
                """
            )
            self.conn.commit()

    def insert_alarm(self, payload: Dict[str, Any]) -> None:
        with self.lock:
            cursor = self.conn.cursor()
            cursor.execute(
                """
                INSERT INTO alarms (
                    device_id, event, severity, pulse, spo2, temperature, ts
                ) VALUES (?, ?, ?, ?, ?, ?, ?)
                """,
                (
                    payload.get("device_id"),
                    payload.get("event"),
                    payload.get("severity"),
                    payload.get("pulse"),
                    payload.get("spo2"),
                    payload.get("temperature"),
                    payload.get("ts"),
                ),
            )
            self.conn.commit()

    def get_latest(self, limit: int = 10) -> List[Tuple[Any, ...]]:
        with self.lock:
            cursor = self.conn.cursor()
            cursor.execute(
                """
                SELECT id, device_id, event, severity, pulse, spo2, temperature, ts, received_at
                FROM alarms
                ORDER BY id DESC
                LIMIT ?
                """,
                (limit,),
            )
            return cursor.fetchall()

    def close(self) -> None:
        with self.lock:
            self.conn.close()
