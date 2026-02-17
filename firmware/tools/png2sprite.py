#!/usr/bin/env python3
"""Convert a PNG image to a C++ header with sprite data for the ILI9488 LCD driver."""

import argparse
import os
import sys
from pathlib import Path

from PIL import Image


def png_to_sprite(png_path: str, output_path: str) -> None:
    img = Image.open(png_path)
    w, h = img.size

    # Derive sprite name from filename (e.g. "button1.png" -> "button1")
    name = Path(png_path).stem

    # Extract RGB data (always convert to RGBA first to handle all formats)
    img_rgba = img.convert("RGBA")
    pixels = list(img_rgba.getdata())

    # Extract RGB888 pixel bytes
    rgb_bytes = []
    for r, g, b, _a in pixels:
        rgb_bytes.extend([r, g, b])

    # Check alpha channel
    has_alpha = img.mode in ("RGBA", "LA", "PA") or "transparency" in img.info
    alpha_bytes = []
    fully_opaque = True

    if has_alpha:
        # Build 1-bit alpha mask: alpha >= 128 -> opaque (1)
        alpha_bits = []
        for _r, _g, _b, a in pixels:
            bit = 1 if a >= 128 else 0
            alpha_bits.append(bit)
            if bit == 0:
                fully_opaque = False

        # Pack into bytes, MSB = leftmost pixel
        for i in range(0, len(alpha_bits), 8):
            byte = 0
            for j in range(8):
                if i + j < len(alpha_bits):
                    byte |= alpha_bits[i + j] << (7 - j)
            alpha_bytes.append(byte)

    # Generate C++ header
    lines = []
    lines.append("#pragma once")
    lines.append('#include "lcd/sprite.hpp"')
    lines.append("")
    lines.append("namespace lcd::sprites {")
    lines.append("")

    # Pixel data
    lines.append(f"inline constexpr uint8_t {name}_pixels[] = {{")
    for i in range(0, len(rgb_bytes), 24):  # 8 pixels per line (24 bytes)
        chunk = rgb_bytes[i : i + 24]
        line = "    " + ", ".join(f"0x{b:02X}" for b in chunk) + ","
        lines.append(line)
    lines.append("};")
    lines.append("")

    # Alpha mask (only if not fully opaque)
    if has_alpha and not fully_opaque:
        lines.append(f"inline constexpr uint8_t {name}_alpha[] = {{")
        for i in range(0, len(alpha_bytes), 16):
            chunk = alpha_bytes[i : i + 16]
            line = "    " + ", ".join(f"0x{b:02X}" for b in chunk) + ","
            lines.append(line)
        lines.append("};")
        lines.append("")
        alpha_ptr = f"{name}_alpha"
    else:
        alpha_ptr = "nullptr"

    lines.append(
        f"inline constexpr Sprite {name} = {{ {w}, {h}, {name}_pixels, {alpha_ptr} }};"
    )
    lines.append("")
    lines.append("} // namespace lcd::sprites")
    lines.append("")

    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    with open(output_path, "w") as f:
        f.write("\n".join(lines))


def main() -> None:
    parser = argparse.ArgumentParser(description="Convert PNG to C++ sprite header")
    parser.add_argument("input", help="Input PNG file path")
    parser.add_argument("output", help="Output C++ header file path")
    args = parser.parse_args()

    if not os.path.isfile(args.input):
        print(f"Error: input file not found: {args.input}", file=sys.stderr)
        sys.exit(1)

    png_to_sprite(args.input, args.output)
    print(f"Generated: {args.output}")


if __name__ == "__main__":
    main()
