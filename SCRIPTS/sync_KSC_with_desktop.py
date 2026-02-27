#!/usr/bin/env python3
"""
Sync the latest KSC build and data to the desktop install folder.

Copies KSC_Raylib.exe from BUILD/Debug/ and syncs KSC_DATA/ to INSTALL_DIR.
The install folder is treated as a self-contained game directory — run the
exe from there and it will find KSC_DATA/ as a sibling.

Usage:
    python SCRIPTS/sync_KSC_with_desktop.py
    python SCRIPTS/sync_KSC_with_desktop.py --dry-run  # preview without copying
"""

import argparse
import shutil
import sys
from pathlib import Path

SCRIPTS_DIR = Path(__file__).parent
sys.path.insert(0, str(SCRIPTS_DIR))
import sync_discovered

REPO_ROOT   = Path(__file__).parent.parent
BUILD_DIR   = REPO_ROOT / "BUILD" / "Debug"
DATA_SRC    = REPO_ROOT / "KSC_DATA"
INSTALL_DIR = Path(r"C:\KSC_GAME")

EXE_NAME    = "KSC_Raylib.exe"

# Extensions to exclude from KSC_DATA sync
EXCLUDE_EXTENSIONS = {".ino", ".cpp", ".h"}
EXCLUDE_NAMES      = {".git", ".gitignore", "__pycache__"}


def should_copy(src: Path, dest: Path) -> bool:
    """Return True if src is absent at dest, newer, or a different size."""
    if not dest.exists():
        return True
    src_stat  = src.stat()
    dest_stat = dest.stat()
    return src_stat.st_size != dest_stat.st_size or src_stat.st_mtime > dest_stat.st_mtime


def is_excluded(path: Path) -> bool:
    if path.suffix in EXCLUDE_EXTENSIONS:
        return True
    if path.name in EXCLUDE_NAMES:
        return True
    return False


def sync_exe(dest_dir: Path, dry_run: bool) -> None:
    """Copy KSC_Raylib.exe to the install directory if it is newer."""
    src  = BUILD_DIR / EXE_NAME
    dest = dest_dir  / EXE_NAME

    if not src.exists():
        print(f"  [SKIP] {EXE_NAME} not found at {src}")
        return

    if should_copy(src, dest):
        if dry_run:
            print(f"  [DRY] {EXE_NAME}")
        else:
            dest_dir.mkdir(parents=True, exist_ok=True)
            shutil.copy2(src, dest)
            print(f"  {EXE_NAME}")
    else:
        print(f"  (up-to-date) {EXE_NAME}")


def sync_data(dest_data: Path, dry_run: bool) -> tuple[int, int]:
    """Recursively sync KSC_DATA/ to dest_data/."""
    copied = skipped = 0

    for src_file in sorted(DATA_SRC.rglob("*")):
        if src_file.is_dir():
            continue
        if is_excluded(src_file):
            continue

        rel_path  = src_file.relative_to(DATA_SRC)
        dest_file = dest_data / rel_path

        if should_copy(src_file, dest_file):
            if dry_run:
                print(f"  [DRY] {rel_path}")
            else:
                dest_file.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(src_file, dest_file)
                print(f"  {rel_path}")
            copied += 1
        else:
            skipped += 1

    return copied, skipped


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Sync KSC build and data to the desktop install folder.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument("--dry-run", action="store_true",
                        help="Preview changes without copying")
    args = parser.parse_args()

    print(f"Install dir : {INSTALL_DIR}")
    print(f"Build dir   : {BUILD_DIR}")
    print(f"Data source : {DATA_SRC}")
    if args.dry_run:
        print("[DRY RUN MODE - no files will be copied]\n")
    else:
        print()

    # --- sync_discovered ---
    print("Discovered state:")
    if args.dry_run:
        print("  [DRY] would run sync_discovered.py")
    else:
        sync_discovered.main()

    # --- Executable ---
    print("Executable:")
    sync_exe(INSTALL_DIR, args.dry_run)

    # --- KSC_DATA ---
    print("\nKSC_DATA:")
    copied, skipped = sync_data(INSTALL_DIR / "KSC_DATA", args.dry_run)

    print(f"\n{'='*50}")
    print(f"  Exe      : synced (see above)")
    print(f"  Data copied  : {copied}")
    print(f"  Data skipped : {skipped} (up-to-date)")
    print(f"{'='*50}")

    if args.dry_run:
        print("\nRun without --dry-run to actually copy files.")


if __name__ == "__main__":
    main()
