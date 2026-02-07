import json

def parse(ndjson_input):
    records = []

    for line in ndjson_input.split("\n"):
        line = line.strip()
        if not line:
            continue
        record = json.loads(line)
        records.append(record)

    return records
