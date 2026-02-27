#!/usr/bin/env python3
"""
Convert all non-PNG images under ASSETS/IMAGES/RAW/ to PNG format,
mirroring the subdirectory structure into ASSETS/IMAGES/PNG/ORIGINAL_SIZE/.

Example:
  RAW/AVERY/DESK/BOOKS/file.arw -> PNG/ORIGINAL_SIZE/AVERY/DESK/BOOKS/file.png
"""

import os
import shutil
from pathlib import Path
from PIL import Image

# Try to import rawpy for RAW format support
try:
    import rawpy
    HAS_RAWPY = True
except ImportError:
    HAS_RAWPY = False
    print("Warning: rawpy not installed. RAW formats (.ARW, .CR2, .NEF, etc.) won't be converted.")
    print("Install with: pip install rawpy")

# Common image extensions
COMMON_FORMATS = {'.jpg', '.jpeg', '.bmp', '.gif', '.tiff', '.tif', '.webp'}
RAW_FORMATS = {'.arw', '.cr2', '.nef', '.dng', '.orf', '.rw2', '.pef', '.srw'}

def convert_raw_to_png(input_path, output_path):
    """Convert RAW image format to PNG using rawpy."""
    if not HAS_RAWPY:
        print(f"Skipping {input_path.name}: rawpy not installed")
        return False

    try:
        with rawpy.imread(str(input_path)) as raw:
            rgb = raw.postprocess()
            img = Image.fromarray(rgb)
            img.save(output_path, 'PNG')
            print(f"Converted {input_path.name} -> {output_path}")
            return True
    except Exception as e:
        print(f"Failed to convert {input_path.name}: {e}")
        return False

def convert_common_to_png(input_path, output_path):
    """Convert common image formats to PNG using PIL."""
    try:
        with Image.open(input_path) as img:
            if img.mode in ('RGBA', 'LA'):
                img.save(output_path, 'PNG')
            elif img.mode != 'RGB':
                img = img.convert('RGB')
                img.save(output_path, 'PNG')
            else:
                img.save(output_path, 'PNG')
            print(f"Converted {input_path.name} -> {output_path}")
            return True
    except Exception as e:
        print(f"Failed to convert {input_path.name}: {e}")
        return False

def scan_and_convert(raw_root):
    """
    Recursively scan raw_root for non-PNG images and convert them to PNG,
    mirroring the directory structure under PNG/ORIGINAL_SIZE/.
    """
    raw_root = Path(raw_root)
    png_root = raw_root.parent / "PNG" / "ORIGINAL_SIZE"

    if not raw_root.exists():
        print(f"Error: Directory {raw_root} does not exist")
        return

    print(f"Input root:  {raw_root}")
    print(f"Output root: {png_root}\n")

    all_files = [f for f in raw_root.rglob('*') if f.is_file()]
    converted = skipped = failed = 0

    print(f"Found {len(all_files)} files\n")

    for file_path in all_files:
        ext = file_path.suffix.lower()
        rel = file_path.relative_to(raw_root)

        output_path = png_root / rel.with_suffix('.png')
        output_path.parent.mkdir(parents=True, exist_ok=True)

        if output_path.exists():
            print(f"Skipping {rel} (PNG already exists)")
            skipped += 1
            continue

        if ext == '.png':
            # Move existing PNGs into the mirrored structure
            try:
                shutil.copy2(str(file_path), str(output_path))
                print(f"Copied existing PNG: {rel}")
                converted += 1
            except Exception as e:
                print(f"Failed to copy {rel}: {e}")
                failed += 1
        elif ext in RAW_FORMATS:
            success = convert_raw_to_png(file_path, output_path)
            converted += success
            failed += not success
        elif ext in COMMON_FORMATS:
            success = convert_common_to_png(file_path, output_path)
            converted += success
            failed += not success
        else:
            print(f"Skipping unknown format: {rel}")
            skipped += 1

    print(f"\n{'='*50}")
    print(f"Summary:")
    print(f"  Converted/copied: {converted}")
    print(f"  Skipped:          {skipped}")
    print(f"  Failed:           {failed}")
    print(f"{'='*50}\n")

if __name__ == "__main__":
    raw_root = Path(__file__).parent.parent / "ASSETS" / "IMAGES" / "RAW"
    scan_and_convert(raw_root)
