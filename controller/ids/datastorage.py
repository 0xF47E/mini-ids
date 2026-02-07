import json

# This has to be a database later

def save(outfile, records):
    with open(outfile, "a") as f:
        for record in records:
            f.write(json.dumps(record) + "\n")
        f.close()
