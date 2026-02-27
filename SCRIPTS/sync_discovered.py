#!/usr/bin/env python3
"""
Sync the discovered maps in KSC_DATA/GAME_STATE/Game_State.json against all scene JSONs.

Every JSON with an 'isDiscovered' field is a discoverable scene. Scenes are
grouped by subject area, derived from their path:

    /LOCATIONS/DESK/…      -> "desk_locations"
    /LOCATIONS/LIBRARY/…   -> "library_locations"
    /NOTES/<FOLDER>/…      -> "<folder_lowercase>_note"

Within each group, scenes with 'isRoot: true' are listed first.

Usage:
    python SCRIPTS/sync_discovered.py           # add new, drop deleted, keep runtime values
    python SCRIPTS/sync_discovered.py --reset   # rebuild from JSON defaults (new game)
"""

import json
import sys
from pathlib import Path

ROOT            = Path(__file__).parent.parent
DATA_ROOT       = ROOT / "KSC_DATA"
GAME_STATE_PATH = DATA_ROOT / "GAME_STATE" / "Game_State.json"


def data_key(path: Path) -> str:
    """KSC_DATA/LOCATIONS/DESK/MAIN/Desk_Full.json -> /LOCATIONS/DESK/MAIN/Desk_Full.json"""
    return "/" + path.relative_to(DATA_ROOT).as_posix()


def group_for(key: str) -> str:
    """Derive the group name from a data-root-relative path."""
    parts = key.strip("/").split("/")
    if parts[0] == "LOCATIONS" and len(parts) >= 2:
        return parts[1].lower() + "_locations"
    if parts[0] == "NOTES" and len(parts) >= 2:
        return parts[1].lower() + "_note"
    return "misc"


def scan_defaults() -> dict[str, tuple[bool, bool]]:
    """Return {key: (isDiscovered_default, isRoot)} for every discoverable scene JSON."""
    defaults = {}
    for json_file in sorted(DATA_ROOT.rglob("*.json")):
        if json_file == GAME_STATE_PATH:
            continue
        try:
            data = json.loads(json_file.read_text(encoding="utf-8"))
        except (json.JSONDecodeError, OSError):
            continue
        if "isDiscovered" not in data:
            continue
        key = data_key(json_file)
        defaults[key] = (bool(data["isDiscovered"]), bool(data.get("isRoot", False)))
    return defaults


def sort_group(entries: dict[str, bool], defaults: dict[str, tuple[bool, bool]]) -> dict[str, bool]:
    """Return entries sorted with isRoot scenes first, then alphabetically."""
    def sort_key(k):
        is_root = defaults.get(k, (False, False))[1]
        return (0 if is_root else 1, k)
    return {k: entries[k] for k in sorted(entries, key=sort_key)}


def main() -> None:
    reset = "--reset" in sys.argv

    game_state = json.loads(GAME_STATE_PATH.read_text(encoding="utf-8"))
    defaults   = scan_defaults()

    # Flatten all existing groups into one lookup: key -> value
    existing: dict[str, bool] = {}
    for v in game_state.values():
        if isinstance(v, dict):
            existing.update(v)

    added   = []
    removed = []
    merged: dict[str, dict[str, bool]] = {}

    if reset:
        for key, (default, _) in defaults.items():
            group = group_for(key)
            merged.setdefault(group, {})[key] = default
        total = sum(len(v) for v in merged.values())
        print(f"Reset: {total} entries restored to defaults.")
    else:
        for key, (default, _) in defaults.items():
            group = group_for(key)
            merged.setdefault(group, {})[key] = existing[key] if key in existing else default
            if key not in existing:
                added.append((key, group, default))

        for key in existing:
            if key not in defaults:
                removed.append(key)

        if added:
            print(f"Added   ({len(added)}):")
            for key, group, default in added:
                print(f"  [{group}] {key}  [default={default}]")
        if removed:
            print(f"Removed ({len(removed)}):")
            for key in removed:
                print(f"  {key}")
        if not added and not removed:
            total = sum(len(v) for v in merged.values())
            print(f"Up to date — {total} discoverable scenes tracked.")

    # Sort each group: root entries first, then alphabetical
    for group in merged:
        merged[group] = sort_group(merged[group], defaults)

    # Write back: preserve non-dict fields, replace group dicts
    output = {k: v for k, v in game_state.items() if not isinstance(v, dict)}
    output.update(merged)

    GAME_STATE_PATH.write_text(
        json.dumps(output, indent=2) + "\n",
        encoding="utf-8"
    )


if __name__ == "__main__":
    main()
