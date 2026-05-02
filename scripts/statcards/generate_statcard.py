"""
Stat card image generator.

Reads a JSON file describing a unit stat card and generates a PNG image
that mimics the layout shown in the reference screenshot:

  +-----------------------------------------------+
  |              N UNIT NAME (title)              |
  +---+---+---+---+---+---+---+---+---+---+---+---+
  | M |WS |BS | S | T | W | I | A |Ld |Int|Cl |WP |  (static stat headers)
  +---+---+---+---+---+---+---+---+---+---+---+---+
  | 8 | 3 | 0 | 4 | 4 | 2 | 1 | 2 | 2 | 4 | 9 | 9 |  (dynamic stat values)
  +---+---+---+---+---+---+---+---+---+---+---+---+
  | WEAPONS:    Claws (-2 to enemy saves)         |  (optional)
  +-----------------------------------------------+
  | ARMOR:      None                              |  (optional)
  +-----------------------------------------------+
  | PSYCHOLOGY: Fear, stupidity                   |  (optional)
  +-----------------------------------------------+
  | SPECIAL:    Slow charge, stinky               |  (optional)
  +-----------------------------------------------+

Usage:
    python generate_statcard.py <input.json> [-o output.png] [--schema path]
"""

from __future__ import annotations

import argparse
import json
import os
import sys
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

try:
    from jsonschema import validate as jsonschema_validate
except ImportError:
    jsonschema_validate = None


# ---------------------------------------------------------------------------
# Layout constants (in pixels). The card is rendered at a fixed nominal width.
# ---------------------------------------------------------------------------

STAT_KEYS = ["M", "WS", "BS", "S", "T", "W", "I", "A", "Ld", "Int", "Cl", "WP"]

CARD_WIDTH       = 720
LINE_WIDTH       = 3        # outer border / heavy separators
INNER_LINE_WIDTH = 2        # internal grid lines

TITLE_HEIGHT     = 56
HEADER_HEIGHT    = 38
VALUE_HEIGHT     = 38
DETAIL_HEIGHT    = 44
PADDING_X        = 14

BG_COLOR    = (255, 255, 255)
FG_COLOR    = (0, 0, 0)
TITLE_BG    = (0, 0, 0)
TITLE_FG    = (255, 255, 255)

DETAIL_FIELDS = [
    ("weapons",         "WEAPONS:"),
    ("armor",           "ARMOR:"),
    ("psychology",      "PSYCHOLOGY:"),
    ("special",         "SPECIAL:"),
    ("musician",        "MUSICIAN:"),
    ("standard_bearer", "STANDARD BEARER:"),
]


# ---------------------------------------------------------------------------
# Font loading. We try a few common bold sans-serif fonts and gracefully fall
# back to PIL's default bitmap font if none are available.
# ---------------------------------------------------------------------------

def _load_font(size: int, bold: bool = False) -> ImageFont.ImageFont:
    candidates_bold = [
        "arialbd.ttf", "Arial Bold.ttf",
        "DejaVuSans-Bold.ttf",
        "LiberationSans-Bold.ttf",
        "Helvetica-Bold.ttf",
    ]
    candidates_regular = [
        "arial.ttf", "Arial.ttf",
        "DejaVuSans.ttf",
        "LiberationSans-Regular.ttf",
        "Helvetica.ttf",
    ]
    for name in (candidates_bold if bold else candidates_regular):
        try:
            return ImageFont.truetype(name, size)
        except (OSError, IOError):
            continue
    # Last resort: PIL default (size argument ignored)
    return ImageFont.load_default()


# ---------------------------------------------------------------------------
# Drawing helpers
# ---------------------------------------------------------------------------

def _text_size(draw: ImageDraw.ImageDraw, text: str, font) -> tuple[int, int]:
    bbox = draw.textbbox((0, 0), text, font=font)
    return bbox[2] - bbox[0], bbox[3] - bbox[1]


def _draw_centered_text(draw, text, box, font, fill):
    x0, y0, x1, y1 = box
    tw, th = _text_size(draw, text, font)
    # textbbox includes glyph offset; nudge by bbox origin
    bbox = draw.textbbox((0, 0), text, font=font)
    cx = x0 + (x1 - x0 - tw) / 2 - bbox[0]
    cy = y0 + (y1 - y0 - th) / 2 - bbox[1]
    draw.text((cx, cy), text, font=font, fill=fill)


def _draw_left_text(draw, text, box, font, fill, pad_x=PADDING_X):
    x0, y0, x1, y1 = box
    bbox = draw.textbbox((0, 0), text, font=font)
    th = bbox[3] - bbox[1]
    cy = y0 + (y1 - y0 - th) / 2 - bbox[1]
    draw.text((x0 + pad_x - bbox[0], cy), text, font=font, fill=fill)


def _draw_left_text_top(draw, text, x, y, font, fill):
    bbox = draw.textbbox((0, 0), text, font=font)
    draw.text((x - bbox[0], y - bbox[1]), text, font=font, fill=fill)


def _wrap_text(draw, text: str, font, max_width: int) -> list[str]:
    """Greedy word-wrap for a single value string."""
    words = text.split()
    if not words:
        return [""]
    lines: list[str] = []
    cur = words[0]
    for w in words[1:]:
        trial = cur + " " + w
        tw = draw.textbbox((0, 0), trial, font=font)[2]
        if tw <= max_width:
            cur = trial
        else:
            lines.append(cur)
            cur = w
    lines.append(cur)
    return lines


def _draw_rect(draw, box, outline=FG_COLOR, width=LINE_WIDTH, fill=None):
    draw.rectangle(box, outline=outline, width=width, fill=fill)


def _fmt_stat(v) -> str:
    if isinstance(v, bool):
        return str(v)
    if isinstance(v, float):
        return str(int(v)) if v.is_integer() else str(v)
    return str(v)


# ---------------------------------------------------------------------------
# Validation
# ---------------------------------------------------------------------------

def load_and_validate(json_path: Path, schema_path: Path | None) -> dict:
    with open(json_path, "r", encoding="utf-8") as fh:
        data = json.load(fh)

    if schema_path and schema_path.exists():
        with open(schema_path, "r", encoding="utf-8") as fh:
            schema = json.load(fh)
        if jsonschema_validate is not None:
            jsonschema_validate(instance=data, schema=schema)
        else:
            _basic_validate(data, schema)
    else:
        _basic_validate(data, None)
    return data


def _basic_validate(data: dict, schema: dict | None) -> None:
    """Minimal validation used when jsonschema is not installed."""
    if not isinstance(data, dict):
        raise ValueError("Top-level JSON must be an object")
    for key in ("name", "stats"):
        if key not in data:
            raise ValueError(f"Missing required field: {key!r}")
    if "quantity" in data:
        q = data["quantity"]
        if not isinstance(q, (int, str)) or (isinstance(q, int) and q < 0):
            raise ValueError("'quantity' must be a non-negative integer or a string")
    if not isinstance(data["name"], str) or not data["name"]:
        raise ValueError("'name' must be a non-empty string")
    stats = data["stats"]
    if not isinstance(stats, dict):
        raise ValueError("'stats' must be an object")
    for key in STAT_KEYS:
        if key not in stats:
            raise ValueError(f"Missing stat: {key!r}")
        if not isinstance(stats[key], (int, float, str)):
            raise ValueError(f"Stat {key!r} must be a number or string")
    for opt in ("weapons", "armor", "psychology", "special"):
        if opt in data and not isinstance(data[opt], str):
            raise ValueError(f"Optional field {opt!r} must be a string")
    for opt in ("musician", "standard_bearer"):
        if opt in data:
            if not isinstance(data[opt], str) or data[opt] not in ("Yes", "No"):
                raise ValueError(f"Optional field {opt!r} must be 'Yes' or 'No'")


# ---------------------------------------------------------------------------
# Main rendering
# ---------------------------------------------------------------------------

def render_card(data: dict) -> Image.Image:
    n_cols = len(STAT_KEYS)

    # Detail rows that are present in this card
    details = [(label, data[key]) for key, label in DETAIL_FIELDS if key in data and data[key]]

    # Pre-compute label column width and per-row heights based on wrapped text.
    # We need a temporary draw context for measurement.
    _tmp = Image.new("RGB", (10, 10))
    _tmpd = ImageDraw.Draw(_tmp)
    label_font_pre  = _load_font(18, bold=True)
    body_font_pre   = _load_font(18, bold=False)
    label_col_w = max(_text_size(_tmpd, label, label_font_pre)[0]
                      for _, label in DETAIL_FIELDS) + PADDING_X * 2
    value_max_w = CARD_WIDTH - label_col_w - PADDING_X
    line_h = _text_size(_tmpd, "Hg", body_font_pre)[1] + 6

    detail_rows = []  # list of (label, [lines], height)
    for label, value in details:
        lines = _wrap_text(_tmpd, str(value), body_font_pre, value_max_w)
        h = max(DETAIL_HEIGHT, line_h * len(lines) + 16)
        detail_rows.append((label, lines, h))

    total_height = (
        TITLE_HEIGHT
        + HEADER_HEIGHT
        + VALUE_HEIGHT
        + sum(h for _, _, h in detail_rows)
    )

    img = Image.new("RGB", (CARD_WIDTH, total_height), BG_COLOR)
    draw = ImageDraw.Draw(img)

    # Fonts
    title_font  = _load_font(28, bold=True)
    header_font = _load_font(20, bold=True)
    value_font  = _load_font(22, bold=True)
    label_font  = _load_font(18, bold=True)
    body_font   = _load_font(18, bold=False)

    y = 0

    # ---- Title row (black bar, white centered bold text) -------------------
    title_box = (0, y, CARD_WIDTH, y + TITLE_HEIGHT)
    draw.rectangle(title_box, fill=TITLE_BG)
    if "quantity" in data and data["quantity"] not in (None, ""):
        title_text = f"{data['quantity']} {data['name']}"
    else:
        title_text = str(data["name"])
    _draw_centered_text(draw, title_text, title_box, title_font, TITLE_FG)
    y += TITLE_HEIGHT

    # ---- Stat header & value rows (12-column grid) -------------------------
    grid_top    = y
    grid_bottom = y + HEADER_HEIGHT + VALUE_HEIGHT
    col_edges   = [round(i * CARD_WIDTH / n_cols) for i in range(n_cols + 1)]

    # Outer rectangle for the grid area
    _draw_rect(draw, (0, grid_top, CARD_WIDTH - 1, grid_bottom), width=LINE_WIDTH)

    # Horizontal line between header and values
    mid_y = y + HEADER_HEIGHT
    draw.line([(0, mid_y), (CARD_WIDTH, mid_y)], fill=FG_COLOR, width=INNER_LINE_WIDTH)

    # Vertical lines between columns
    for i in range(1, n_cols):
        x = col_edges[i]
        draw.line([(x, grid_top), (x, grid_bottom)], fill=FG_COLOR, width=INNER_LINE_WIDTH)

    # Header text
    for i, key in enumerate(STAT_KEYS):
        cell = (col_edges[i], grid_top, col_edges[i + 1], mid_y)
        _draw_centered_text(draw, key, cell, header_font, FG_COLOR)

    # Value text
    for i, key in enumerate(STAT_KEYS):
        cell = (col_edges[i], mid_y, col_edges[i + 1], grid_bottom)
        _draw_centered_text(draw, _fmt_stat(data["stats"][key]), cell, value_font, FG_COLOR)

    y = grid_bottom

    # ---- Detail rows -------------------------------------------------------
    for label, lines, h in detail_rows:
        row_box = (0, y, CARD_WIDTH - 1, y + h)
        _draw_rect(draw, row_box, width=LINE_WIDTH)
        # Vertically center the (label + wrapped value) block inside the row.
        # Block height equals the value column's wrapped height: line_h * N - leading.
        block_h = line_h * len(lines) - (line_h - _text_size(_tmpd, "Hg", body_font_pre)[1])
        text_top = y + (h - block_h) / 2
        # Label (bold) — top-aligned with the first value line
        _draw_left_text_top(draw, label, PADDING_X, text_top,
                            label_font, FG_COLOR)
        # Value lines (top-aligned, wrapped)
        text_x = label_col_w + 4
        text_y = text_top
        for ln in lines:
            _draw_left_text_top(draw, ln, text_x, text_y, body_font, FG_COLOR)
            text_y += line_h
        y += h

    return img


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description="Generate a unit stat card image from JSON.")
    parser.add_argument("input", type=Path, help="Path to the stat card JSON file.")
    parser.add_argument("-o", "--output", type=Path, default=None,
                        help="Output PNG path (default: <input>.png).")
    parser.add_argument("--schema", type=Path,
                        default=Path(__file__).with_name("statcard.schema.json"),
                        help="Path to JSON schema for validation.")
    args = parser.parse_args(argv)

    if not args.input.exists():
        print(f"error: input file not found: {args.input}", file=sys.stderr)
        return 2

    output = args.output or args.input.with_suffix(".png")

    data = load_and_validate(args.input, args.schema)
    img = render_card(data)
    output.parent.mkdir(parents=True, exist_ok=True)
    img.save(output, "PNG")
    print(f"Wrote {output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
