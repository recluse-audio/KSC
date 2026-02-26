# KSC
Narrative Mystery Told in Lo-Fi Application and ESP32 with integrated display.


KSC stands for "Kasota Stone Conspiracy", but is a working title.


**Memory model:** when the active scene changes, its JSON is loaded into memory. Images and text files referenced by that JSON are read from SD individually on demand — not pre-loaded. Only one scene JSON is held in memory at a time.


## Scenes

A **Scene** is the fundamental unit of the game. Every scene has something to display (an image, document, or note) and a set of **Zones** — clickable regions that the player can interact with.

The game is built from two major collections of scenes:

- **Locations** — physical places the player can visit and explore
- **Notes** — collected information: bullet points, charts, images, and documents


## Clues

A **Clue** is something discoverable within a location. When the player finds a clue, it is marked as discovered and a corresponding **Note** is unlocked in the player's Notes collection.

Clues live in the JSON hierarchy of the location they belong to, as entries in that location's `clues` array. Each clue has a name, a description of what was found, an `isDiscovered` flag (defaults to `false`), and a reference to the Note it unlocks.

```json
{
  "id": "AVERYS_LOGIN",
  "name": "Avery's Login Credentials",
  "description": "Login history showing repeated failed attempts from an external IP.",
  "isDiscovered": false,
  "unlocksNote": "AVERYS_LOGIN_HISTORY"
}
```

### The Game Loop

1. Player **explores a location** — navigating its zones and sub-locations
2. Player **discovers a clue** — triggering it to be marked discovered
3. A **Note is unlocked** — new information appears in the Notes collection
4. Notes **guide the player** — hinting at new locations to visit or new things to look for
5. New locations may become **accessible** as clues and notes accumulate
6. The cycle repeats, drawing the player closer to solving the mystery


## Game Structure

The **GameRunner** manages the active scene and the current mode of the application.

### Location Mode

The player navigates a set of locations. Left/right input cycles through the top-level locations. Clicking a zone within a location leads to a sub-location, drilling deeper into that place. Left/right cycling is only available at the root level of the location hierarchy.

### Notes Mode

The player navigates their collected notes. Left/right input cycles through top-level notes. Within a note, zones lead to sub-content navigated vertically — bullet points, charts, images, and supporting documents. Left/right cycling is only available at the root level of the notes hierarchy.

### Navigation Summary

| Input | Location Mode | Notes Mode |
|---|---|---|
| Left / Right | Cycle root locations | Cycle root notes |
| Tap a Zone | Enter sub-location | Enter sub-content (vertical) |
| Back | Return to root | Return to root |