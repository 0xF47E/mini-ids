from .ids.runner import scan
from .ids.parser import parse
from storage import datastorage
import time

# This is the entrypoint of the controller


def main():
    while True:
        result = scan()
        records = parse(result)
        datastorage.init_db()
        if records == []:
            time.sleep(10)
            continue
        scan_id = datastorage.create_scan(records[0]["ts"], records[0]["target"])

        for result in records:
            datastorage.insert_result(
                scan_id, result["target"], result["port"], result["state"]
            )

        time.sleep(10)


if __name__ == "__main__":
    main()
