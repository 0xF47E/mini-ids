from pathlib import Path
import subprocess

PROJECT_PATH = Path(__file__).resolve().parent.parent.parent
SCANNER_PATH = PROJECT_PATH / "scanner" / "scanner"

def scan(target, ping):
    proc = subprocess.run([str(SCANNER_PATH), "-t", "127.0.0.1", "-p", "22"],
                      capture_output=True,
                      text=True)
    
    return proc.stdout
