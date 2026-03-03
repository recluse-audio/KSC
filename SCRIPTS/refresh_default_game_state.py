#!/usr/bin/env python3
"""
Rebuild KSC_DATA/GAME_STATE/Game_State.json and NOTES_STATE files to their
default (new-game) values.

  - Location discovery maps: rebuilt from each scene JSON's isDiscovered default.
  - Notes array: rebuilt by scanning KSC_DATA/NOTES/**/*_Base.md.
  - GAME_STATE/NOTES_STATE/<CHAR>/<Name>.md: reset to matching *_Base.md content.

Usage:
    python SCRIPTS/refresh_default_game_state.py
"""

import json
from pathlib import Path

ROOT            = Path(__file__).parent.parent
DATA_ROOT       = ROOT / "KSC_DATA"
GAME_STATE_PATH = DATA_ROOT / "GAME_STATE" / "Game_State.json"
NOTES_STATE_DIR = DATA_ROOT / "GAME_STATE" / "NOTES_STATE"
NOTES_SRC_DIR   = DATA_ROOT / "NOTES"


def data_key(path: Path) -> str:
    """Absolute path → data-root-relative key, e.g. /LOCATIONS/AVERY/ROOT/Avery_Full.json"""
    return "/" + path.relative_to(DATA_ROOT).as_posix()


def group_for(key: str) -> str:
    """
    /LOCATIONS/AVERY/…  -> "avery_locations"
    /LOCATIONS/LIBRARY/… -> "library_locations"
    """
    parts = key.strip("/").split("/")
    if parts[0] == "LOCATIONS" and len(parts) >= 2:
        return parts[1].lower() + "_locations"
    return "misc"


def scan_location_defaults() -> dict[str, tuple[bool, bool]]:
    """
    Scan KSC_DATA/LOCATIONS/ for all JSONs that carry isDiscovered.
    Returns {data_key: (isDiscovered_default, isRoot)}.
    """
    results: dict[str, tuple[bool, bool]] = {}
    for json_file in sorted((DATA_ROOT / "LOCATIONS").rglob("*.json")):
        try:
            data = json.loads(json_file.read_text(encoding="utf-8"))
        except (json.JSONDecodeError, OSError):
            continue
        if "isDiscovered" not in data:
            continue
        key = data_key(json_file)
        results[key] = (bool(data["isDiscovered"]), bool(data.get("isRoot", False)))
    return results


def sort_group(
    entries: dict[str, bool],
    defaults: dict[str, tuple[bool, bool]],
) -> dict[str, bool]:
    """Sort a location group: isRoot entries first, then alphabetical."""
    def sort_key(k: str) -> tuple[int, str]:
        is_root = defaults.get(k, (False, False))[1]
        return (0 if is_root else 1, k)
    return {k: entries[k] for k in sorted(entries, key=sort_key)}


def reset_notes() -> list[str]:
    """
    For every *_Base.md found under KSC_DATA/NOTES/:
      - Copy its content to GAME_STATE/NOTES_STATE/<CHAR>/<Name>.md
        (filename = base filename with '_Base' removed)
    Returns the sorted list of data-root-relative note-state paths.
    """
    note_paths: list[str] = []
    for base_file in sorted(NOTES_SRC_DIR.rglob("*_Base.md")):
        rel_parts = base_file.relative_to(NOTES_SRC_DIR).parts
        if len(rel_parts) < 2:
            # Must be inside a character sub-directory
            continue
        char_dir = rel_parts[0]
        out_name = base_file.stem.replace("_Base", "") + ".md"
        out_dir  = NOTES_STATE_DIR / char_dir
        out_dir.mkdir(parents=True, exist_ok=True)
        out_file = out_dir / out_name
        out_file.write_text(base_file.read_text(encoding="utf-8"), encoding="utf-8")
        key = data_key(out_file)
        note_paths.append(key)
        print(f"  reset  {key}")
    return sorted(note_paths)


def main() -> None:
    # --- Location discovery maps ---
    defaults = scan_location_defaults()
    groups: dict[str, dict[str, bool]] = {}
    for key, (default, _) in defaults.items():
        groups.setdefault(group_for(key), {})[key] = default
    for group in groups:
        groups[group] = sort_group(groups[group], defaults)
    loc_total = sum(len(v) for v in groups.values())
    print(f"Locations: {loc_total} scenes reset to defaults.")

    # --- Note state files ---
    print("Notes:")
    notes_list = reset_notes()
    if not notes_list:
        print("  (no *_Base.md files found)")

    # --- Write Game_State.json ---
    game_state: dict = {
        "currentMode":     "locations",
        "currentLocation": "",
        "currentNote":     "",
        "notes":           notes_list,
    }
    game_state.update(groups)

    GAME_STATE_PATH.write_text(
        json.dumps(game_state, indent=2) + "\n",
        encoding="utf-8",
    )
    print(f"\nWrote {GAME_STATE_PATH.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
