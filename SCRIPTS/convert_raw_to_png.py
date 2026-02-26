#!/usr/bin/env python3
"""
Convert all images in ASSETS/IMAGES/RAW/ to 320x240 PNG.

Supports JPEG, BMP, GIF, TIFF, WEBP and camera RAW formats
(ARW, CR2, NEF, DNG, etc.) in a single pass.

Aspect ratio is preserved; black bars fill any remaining space.
Output goes to ASSETS/IMAGES/PNG/320X240/

Usage:
    python SCRIPTS/convert_raw_to_png.py
"""

from pathlib import Path
from PIL import Image

try:
    import rawpy
    HAS_RAWPY = True
except ImportError:
    HAS_RAWPY = False
    print("Warning: rawpy not installed — camera RAW files will be skipped.")
    print("         Install with: pip install rawpy\n")

RAW_FORMATS = {'.arw', '.cr2', '.nef', '.dng', '.orf', '.rw2', '.pef', '.srw'}
TARGET_SIZE = (320, 240)

REPO_ROOT  = Path(__file__).parent.parent
INPUT_DIR  = REPO_ROOT / "ASSETS" / "IMAGES" / "RAW"
OUTPUT_DIR = REPO_ROOT / "ASSETS" / "IMAGES" / "PNG" / "320X240"


def open_image(path: Path) -> Image.Image | None:
    """Open any supported image format and return an RGB PIL Image."""
    ext = path.suffix.lower()

    if ext in RAW_FORMATS:
        if not HAS_RAWPY:
            print(f"  Skipping {path.name} (rawpy not installed)")
            return None
        try:
            with rawpy.imread(str(path)) as raw:
                rgb = raw.postprocess()
            return Image.fromarray(rgb)
        except Exception as e:
            print(f"  Failed to open {path.name}: {e}")
            return None

    try:
        img = Image.open(path)
        img.load()  # Force read before file closes
        return img
    except Exception as e:
        print(f"  Failed to open {path.name}: {e}")
        return None


def letterbox(img: Image.Image, size: tuple[int, int] = TARGET_SIZE) -> Image.Image:
    """Fit image inside size with black bars, return RGB image at exact size."""
    img = img.convert("RGB")
    img.thumbnail(size, Image.Resampling.LANCZOS)
    canvas = Image.new("RGB", size, (0, 0, 0))
    x = (size[0] - img.width) // 2
    y = (size[1] - img.height) // 2
    canvas.paste(img, (x, y))
    return canvas


def main():
    if not INPUT_DIR.exists():
        print(f"Error: input directory not found: {INPUT_DIR}")
        return

    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

    files = [f for f in sorted(INPUT_DIR.iterdir()) if f.is_file()]
    print(f"Input:  {INPUT_DIR}")
    print(f"Output: {OUTPUT_DIR}")
    print(f"Found {len(files)} file(s)\n")

    converted = skipped = failed = 0

    for src in files:
        suffix = f"_{TARGET_SIZE[0]}x{TARGET_SIZE[1]}"
        dest = OUTPUT_DIR / f"{src.stem}{suffix}.png"

        if dest.exists():
            print(f"  Skipping {src.name} (already exists)")
            skipped += 1
            continue

        img = open_image(src)
        if img is None:
            failed += 1
            continue

        result = letterbox(img)
        result.save(dest, "PNG")
        print(f"  {src.name}  ->  {dest.name}")
        converted += 1

    print(f"\n{'='*45}")
    print(f"  Converted : {converted}")
    print(f"  Skipped   : {skipped}")
    print(f"  Failed    : {failed}")
    print(f"{'='*45}")


if __name__ == "__main__":
    main()
