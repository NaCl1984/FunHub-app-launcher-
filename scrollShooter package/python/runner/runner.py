#!/usr/bin/env python3
"""
Запускает нативный бинарник в зависимости от ОС.
"""
import subprocess
import sys
import os
from pathlib import Path

def get_binary_path() -> Path:
    """Возвращает путь к исполняемому файлу для текущей платформы."""
    platform = sys.platform
    base_dir = Path(__file__).parent / "binaries"
    
    if platform.startswith("win"):
        binary_dir = base_dir / "windows"
        binary_name = "Gnorp.exe"
    elif platform.startswith("linux"):
        binary_dir = base_dir / "linux"
        binary_name = "Gnorp"
    elif platform.startswith("darwin"):
        binary_dir = base_dir / "darwin"
        binary_name = "Gnorp"
    else:
        raise OSError(f"Unsupported platform: {platform}")
    
    binary_path = binary_dir / binary_name
    if not binary_path.exists():
        raise FileNotFoundError(f"Binary not found: {binary_path}")
    
    # На Linux/macOS даём права на выполнение (если ещё нет)
    if not platform.startswith("win"):
        if not os.access(binary_path, os.X_OK):
            binary_path.chmod(binary_path.stat().st_mode | 0o555)
    
    return binary_path

def run_binary():
    """Запускает бинарник в текущей консоли."""
    binary = get_binary_path()
    try:
        result = subprocess.run(
            [str(binary)] + sys.argv[1:],
            stdin=sys.stdin,
            stdout=sys.stdout,
            stderr=sys.stderr,
            check=False
        )
        sys.exit(result.returncode)
    except KeyboardInterrupt:
        sys.exit(130)
    except Exception as e:
        print(f"Ошибка запуска: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    run_binary()