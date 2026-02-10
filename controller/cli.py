from ids.runner import scan
from ids.parser import parse
from ids.datastorage import save

# This is the entrypoint of the controller

def main():
    result = scan()
    records = parse(result)
    save(records)

if __name__ == "__main__":
    main()
