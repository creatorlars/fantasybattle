"""
Impose a PDF as a saddle-stitch booklet for printing on US Letter sheets.

Two source pages are placed side-by-side on each landscape Letter sheet (11" x 8.5"),
producing a digest-sized (5.5" x 8.5") booklet when folded.

Usage:
    python scripts/booklet_impose.py <input.pdf> -o <output.pdf>
"""

from __future__ import annotations

import argparse
from pathlib import Path

from pypdf import PdfReader, PdfWriter, PageObject, Transformation
from pypdf.generic import RectangleObject


LETTER_W = 11.0 * 72.0   # 792 pt (landscape width)
LETTER_H = 8.5 * 72.0    # 612 pt (landscape height)
HALF_W   = LETTER_W / 2  # 396 pt per booklet page slot


def make_blank() -> PageObject:
    p = PageObject.create_blank_page(width=LETTER_W / 2, height=LETTER_H)
    return p


def impose(input_path: Path, output_path: Path) -> None:
    reader = PdfReader(str(input_path))
    src_pages: list[PageObject | None] = list(reader.pages)

    # Pad to a multiple of 4 with blanks.
    while len(src_pages) % 4 != 0:
        src_pages.append(None)

    n = len(src_pages)
    writer = PdfWriter()

    # Saddle-stitch ordering: outer sheet first, working inward.
    # For each sheet (front, then back):
    #   front: [n - 2i, 2i + 1]   (left, right) using 1-based page numbers
    #   back:  [2i + 2, n - 2i - 1]
    for i in range(n // 4):
        front_left  = n - 2 * i           # 1-based
        front_right = 2 * i + 1
        back_left   = 2 * i + 2
        back_right  = n - 2 * i - 1

        for left_idx, right_idx in ((front_left, front_right), (back_left, back_right)):
            sheet = PageObject.create_blank_page(width=LETTER_W, height=LETTER_H)
            for slot, src_idx in enumerate((left_idx, right_idx)):
                src = src_pages[src_idx - 1]  # convert to 0-based
                if src is None:
                    continue
                src_w = float(src.mediabox.width)
                src_h = float(src.mediabox.height)
                # Tight margins: scale each axis independently to fill the half-page slot exactly.
                sx = HALF_W / src_w
                sy = LETTER_H / src_h
                tx = slot * HALF_W
                ty = 0
                ctm = Transformation().scale(sx, sy).translate(tx, ty)
                sheet.merge_transformed_page(src, ctm)
            writer.add_page(sheet)

    with open(output_path, "wb") as fh:
        writer.write(fh)


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("input", type=Path)
    ap.add_argument("-o", "--output", type=Path, required=True)
    args = ap.parse_args()
    impose(args.input, args.output)
    print(f"Wrote {args.output}")


if __name__ == "__main__":
    main()
