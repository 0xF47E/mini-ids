from ids.runner import scan
from ids.parser import parse
from ids.datastorage import save

def main():
    result = scan("127.0.0.1", "22")
    records = parse(result)
    save("test.txt", records)

if __name__ == "__main__":
    main()
