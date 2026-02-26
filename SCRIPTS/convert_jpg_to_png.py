#!/usr/bin/env python3
"""
Convert JPG images in DESK/ and LIBRARY/ to PNG.
Optionally resize using functions from resize_png.py.

Without --resize:
  Input:  JPG/DESK/*.jpg, JPG/LIBRARY/*.jpg
  Output: PNG/JPG/DESK/*.png, PNG/JPG/LIBRARY/*.png (preserves structure)

With --resize:
  Input:  JPG/DESK/*.jpg, JPG/LIBRARY/*.jpg
  Output: PNG/{width}x{height}/DESK/*.png, PNG/{width}x{height}/LIBRARY/*.png

Usage:
    python SCRIPTS/convert_jpg_to_png.py                      # convert only
    python SCRIPTS/convert_jpg_to_png.py --resize             # convert + resize to 320x240
    python SCRIPTS/convert_jpg_to_png.py --resize --width 480 --height 320
"""

import argparse
from pathlib import Path
from PIL import Image

REPO_ROOT   = Path(__file__).parent.parent
JPG_ROOT    = REPO_ROOT / "ASSETS" / "IMAGES" / "JPG"
PNG_ROOT    = REPO_ROOT / "ASSETS" / "IMAGES" / "PNG"


def resize_with_aspect_ratio(img, target_size=(320, 240), bg_color=(0, 0, 0)):
    """From resize_png.py — letterbox with aspect ratio preserved."""
    img_ratio = img.width / img.height
    target_ratio = target_size[0] / target_size[1]

    if img_ratio > target_ratio:
        new_width = target_size[0]
        new_height = int(new_width / img_ratio)
    else:
        new_height = target_size[1]
        new_width = int(new_height * img_ratio)

    resized = img.resize((new_width, new_height), Image.Resampling.LANCZOS)
    result = Image.new('RGB', target_size, bg_color)
    x = (target_size[0] - new_width) // 2
    y = (target_size[1] - new_height) // 2
    result.paste(resized, (x, y))
    return result


def convert_jpg(jpg_path: Path, output_path: Path, resize: bool, target_size: tuple):
    """Convert a single JPG to PNG, optionally resizing."""
    try:
        with Image.open(jpg_path) as img:
            if img.mode != 'RGB':
                img = img.convert('RGB')

            if resize:
                img = resize_with_aspect_ratio(img, target_size)

            img.save(output_path, 'PNG')
            size_str = f" → {target_size[0]}x{target_size[1]}" if resize else ""
            print(f"  {jpg_path.name}{size_str} → {output_path.name}")
            return True

    except Exception as e:
        print(f"  Failed: {jpg_path.name} — {e}")
        return False


def main():
    parser = argparse.ArgumentParser(
        description="Convert JPG images from DESK/ and LIBRARY/ to PNG.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__
    )
    parser.add_argument('--resize', action='store_true',
                        help='Resize images (default: keep original size)')
    parser.add_argument('--width', type=int, default=320,
                        help='Target width if --resize (default: 320)')
    parser.add_argument('--height', type=int, default=240,
                        help='Target height if --resize (default: 240)')

    args = parser.parse_args()
    target_size = (args.width, args.height)

    if not JPG_ROOT.exists():
        print(f"Error: {JPG_ROOT} not found")
        return

    # Discover all subdirectories in JPG/
    subdirs = [d for d in JPG_ROOT.iterdir() if d.is_dir()]
    if not subdirs:
        print(f"No subdirectories found in {JPG_ROOT}")
        return

    converted = skipped = failed = 0

    for input_folder in sorted(subdirs):
        folder_name = input_folder.name
        jpg_files = list(input_folder.rglob("*.jpg")) + list(input_folder.rglob("*.jpeg"))
        if not jpg_files:
            print(f"No JPG files in {folder_name}/")
            continue

        print(f"\n{folder_name}/ — {len(jpg_files)} file(s)")

        for jpg_file in sorted(jpg_files):
            # Always preserve folder structure (DESK/LIBRARY)
            rel_path = jpg_file.relative_to(JPG_ROOT)

            if args.resize:
                # Preserve structure under PNG/{width}x{height}/DESK or LIBRARY
                output_dir = PNG_ROOT / f"{args.width}X{args.height}" / rel_path.parent
                output_dir.mkdir(parents=True, exist_ok=True)
                output_name = f"{jpg_file.stem}_{args.width}x{args.height}.png"
                output_path = output_dir / output_name
            else:
                # Preserve structure under PNG/JPG/DESK or LIBRARY
                output_dir = PNG_ROOT / "JPG" / rel_path.parent
                output_dir.mkdir(parents=True, exist_ok=True)
                output_path = output_dir / jpg_file.with_suffix('.png').name

            if output_path.exists():
                print(f"  Skipping {jpg_file.name} (already exists)")
                skipped += 1
                continue

            if convert_jpg(jpg_file, output_path, args.resize, target_size):
                converted += 1
            else:
                failed += 1

    print(f"\n{'='*50}")
    print(f"  Converted : {converted}")
    print(f"  Skipped   : {skipped}")
    print(f"  Failed    : {failed}")
    print(f"{'='*50}")


if __name__ == "__main__":
    main()
