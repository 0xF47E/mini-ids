import json
from pathlib import Path

# This has to be a database later
PROJECT_PATH = Path(__file__).resolve().parent.parent.parent
DATA_PATH = PROJECT_PATH / "data"

def save(records):
    with open(str(DATA_PATH / "scan.log"), "a") as f:
        for record in records:
            f.write(json.dumps(record) + "\n")
        f.close()
