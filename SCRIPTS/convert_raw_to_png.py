#!/usr/bin/env python3
"""
Convert images in a RAW directory to PNG.

Supports JPEG, BMP, GIF, TIFF, WEBP and camera RAW formats
(ARW, CR2, NEF, DNG, etc.) in a single pass.

Usage:
    # 320x240 from default input dir (aspect-preserved, black bars)
    python SCRIPTS/convert_raw_to_png.py

    # Full native resolution from a specific input dir
    python SCRIPTS/convert_raw_to_png.py --input ASSETS/IMAGES/RAW/best

    # Specific size from a specific input dir
    python SCRIPTS/convert_raw_to_png.py --input ASSETS/IMAGES/RAW/best --size 1920x1080
"""

import argparse
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

REPO_ROOT         = Path(__file__).parent.parent
DEFAULT_INPUT_DIR = REPO_ROOT / "ASSETS" / "IMAGES" / "RAW"
PNG_ROOT          = REPO_ROOT / "ASSETS" / "IMAGES" / "PNG"


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
        img.load()
        return img
    except Exception as e:
        print(f"  Failed to open {path.name}: {e}")
        return None


def letterbox(img: Image.Image, size: tuple[int, int]) -> Image.Image:
    """Fit image inside size with black bars, return RGB image at exact size."""
    img = img.convert("RGB")
    img.thumbnail(size, Image.Resampling.LANCZOS)
    canvas = Image.new("RGB", size, (0, 0, 0))
    x = (size[0] - img.width) // 2
    y = (size[1] - img.height) // 2
    canvas.paste(img, (x, y))
    return canvas


def main():
    parser = argparse.ArgumentParser(description="Convert RAW/image files to PNG.")
    parser.add_argument(
        "--input", "-i",
        type=Path,
        default=None,
        help="Input directory (default: ASSETS/IMAGES/RAW)",
    )
    parser.add_argument(
        "--size", "-s",
        type=str,
        default=None,
        help="Output size as WxH (e.g. 320x240). Omit for full native resolution.",
    )
    args = parser.parse_args()

    input_dir = REPO_ROOT / args.input if args.input and not args.input.is_absolute() else (args.input or DEFAULT_INPUT_DIR)

    target_size: tuple[int, int] | None = None
    if args.size:
        try:
            w, h = args.size.lower().split("x")
            target_size = (int(w), int(h))
        except ValueError:
            print(f"Error: invalid --size '{args.size}'. Expected format: WxH (e.g. 320x240)")
            return

    size_label  = f"{target_size[0]}x{target_size[1]}" if target_size else "FULL"
    output_dir  = PNG_ROOT / size_label.upper()

    if not input_dir.exists():
        print(f"Error: input directory not found: {input_dir}")
        return

    output_dir.mkdir(parents=True, exist_ok=True)

    files = [f for f in sorted(input_dir.iterdir()) if f.is_file()]
    print(f"Input:  {input_dir}")
    print(f"Output: {output_dir}")
    print(f"Size:   {size_label}")
    print(f"Found {len(files)} file(s)\n")

    converted = skipped = failed = 0

    for src in files:
        suffix = f"_{size_label}" if target_size else ""
        dest = output_dir / f"{src.stem}{suffix}.png"

        if dest.exists():
            print(f"  Skipping {src.name} (already exists)")
            skipped += 1
            continue

        img = open_image(src)
        if img is None:
            failed += 1
            continue

        result = letterbox(img, target_size) if target_size else img.convert("RGB")
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
