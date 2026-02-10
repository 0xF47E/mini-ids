from pathlib import Path
import subprocess

PROJECT_PATH = Path(__file__).resolve().parent.parent.parent
SCANNER_PATH = PROJECT_PATH / "scanner" / "scanner"

def scan(target="127.0.0.1", port="22,80,443"):
    proc = subprocess.run([str(SCANNER_PATH), "-t", target, "-p", port],
                      capture_output=True,
                      text=True)
    
    return proc.stdout
