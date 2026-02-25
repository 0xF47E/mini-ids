import json
from pathlib import Path
import sqlite3

# This has to be a database later
PROJECT_PATH = Path(__file__).resolve().parent.parent
DATA_PATH = PROJECT_PATH / "data"


def get_connection():
    con = sqlite3.connect("data/ids.db")
    con.execute("PRAGMA foreign_keys = ON;")
    return con


def init_db():
    con = get_connection()
    cur = con.cursor()
    cur.execute("""
        CREATE TABLE IF NOT EXISTS scans(
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            started_at TIMESTAMP,
            scope TEXT
        );""")
    con.commit()
    cur.execute("""
        CREATE TABLE IF NOT EXISTS results(
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        scan_id INTEGER,
        host TEXT,
        port INTEGER,
        state TEXT,

        FOREIGN KEY (scan_id) REFERENCES  scans(id)
        );""")
    con.commit()
    con.close()


def create_scan(timestamp, scope):
    con = get_connection()
    cur = con.cursor()
    cur.execute(
        """INSERT INTO scans (started_at, scope) VALUES(?, ?)""", (timestamp, scope)
    )

    scan_id = cur.lastrowid
    con.commit()
    con.close()

    return scan_id


def insert_result(scan_id, host, port, state):
    con = get_connection()
    cur = con.cursor()

    cur.execute(
        """INSERT INTO results (scan_id, host, port, state) VALUES (?,?,?,?)""",
        (scan_id, host, port, state),
    )
    con.commit()
    con.close()


def get_lastest_scan_id():
    con = get_connection()
    cur = con.cursor()

    res = cur.execute("""SELECT MAX(id) FROM scans;""")
    last_scan_id = res.fetchall()
    con.close()

    return last_scan_id


def get_latest_results(last_scan_id):
    con = get_connection()
    cur = con.cursor()

    res = cur.execute("""SELECT * FROM results WHERE scan_id = ?;""", (last_scan_id,))

    scans = res.fetchall()
    con.close()

    return scans


def get_previous_scan():
    pass
