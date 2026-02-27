#!/usr/bin/env python3
"""
Launch KSC from the desktop install folder (C:\KSC_GAME).

Usage:
    python SCRIPTS/run_KSC.py
"""

import subprocess
import sys
from pathlib import Path

INSTALL_DIR = Path(r"C:\KSC_GAME")
EXE         = INSTALL_DIR / "KSC_Raylib.exe"


def main() -> None:
    if not EXE.exists():
        print(f"Error: {EXE} not found.")
        print("Run sync_KSC_with_desktop.py first to populate the install folder.")
        sys.exit(1)

    print(f"Launching {EXE} ...")
    subprocess.run([str(EXE)], cwd=str(INSTALL_DIR))


if __name__ == "__main__":
    main()
