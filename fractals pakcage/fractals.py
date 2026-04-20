import sys
import subprocess
try:
    if sys.platform == 'win32':
        subprocess.run(["fractals.exe"])
except KeyboardInterrupt:
    pass