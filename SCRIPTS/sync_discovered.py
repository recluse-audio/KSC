#!/usr/bin/env python3
"""
Sync the 'discovered' map in KSC_DATA/Game_State.json against all scene JSONs in KSC_DATA/.

Every JSON in KSC_DATA/ that contains an 'isDiscovered' field is treated as a
discoverable scene.  The field's value in the JSON is the factory default
(some locations start as true, notes typically start as false).

Usage:
    python SCRIPTS/sync_discovered.py           # add new, drop deleted, keep runtime values
    python SCRIPTS/sync_discovered.py --reset   # rebuild from JSON defaults (new game)
"""

import json
import sys
from pathlib import Path

ROOT            = Path(__file__).parent.parent
SD_ROOT         = ROOT / "KSC_DATA"
GAME_STATE_PATH = SD_ROOT / "Game_State.json"


def sd_key(path: Path) -> str:
    """KSC_DATA/LOCATIONS/DESK/MAIN/Desk_Full.json -> /LOCATIONS/DESK/MAIN/Desk_Full.json"""
    return "/" + path.relative_to(SD_ROOT).as_posix()


def scan_defaults() -> dict[str, bool]:
    """Return {sd_key: default_bool} for every JSON in KSC_DATA/ that has 'isDiscovered'."""
    defaults = {}
    for json_file in sorted(SD_ROOT.rglob("*.json")):
        if json_file == GAME_STATE_PATH:
            continue
        try:
            data = json.loads(json_file.read_text(encoding="utf-8"))
        except (json.JSONDecodeError, OSError):
            continue
        if "isDiscovered" in data:
            defaults[sd_key(json_file)] = bool(data["isDiscovered"])
    return defaults


def main() -> None:
    reset = "--reset" in sys.argv

    game_state = json.loads(GAME_STATE_PATH.read_text(encoding="utf-8"))
    existing   = game_state.get("discovered", {})
    defaults   = scan_defaults()

    if reset:
        merged = dict(defaults)
        print(f"Reset: {len(merged)} entries restored to defaults.")
    else:
        merged  = {}
        added   = []
        removed = []

        for path, default in defaults.items():
            if path in existing:
                merged[path] = existing[path]
            else:
                merged[path] = default
                added.append(path)

        for path in existing:
            if path not in defaults:
                removed.append(path)

        if added:
            print(f"Added   ({len(added)}):")
            for p in added:
                print(f"  {p}  [default={defaults[p]}]")
        if removed:
            print(f"Removed ({len(removed)}):")
            for p in removed:
                print(f"  {p}")
        if not added and not removed:
            print(f"Up to date — {len(merged)} discoverable scenes tracked.")

    game_state["discovered"] = merged
    GAME_STATE_PATH.write_text(
        json.dumps(game_state, indent=2) + "\n",
        encoding="utf-8"
    )


if __name__ == "__main__":
    main()
