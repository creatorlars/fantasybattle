"""Generate a printable letter-size tri-fold table tent for the Hank badge.

Layout (US Letter, portrait, 8.5" x 11"):

    +---------------------------+   <- top of page
    |        (margin)           |
    |   +-------------------+   |
    |   |    GLUE TAB       |   |   0.5"
    |   +-------------------+   |   <-- cut line (top of tab)
    |   |                   |   |
    |   |   PANEL A (back)  |   |   3.0"  badge rotated 180 deg
    |   |                   |   |
    |   +- - - - - - - - - -+   |   <-- fold line
    |   |                   |   |
    |   |  PANEL B (front)  |   |   3.0"  badge upright
    |   |                   |   |
    |   +- - - - - - - - - -+   |   <-- fold line
    |   |                   |   |
    |   |   PANEL C (base)  |   |   3.0"  blank
    |   |                   |   |
    |   +-------------------+   |   <-- cut line (bottom)
    |        (margin)           |
    +---------------------------+

Assembly:
  1. Cut along the solid outer rectangle.
  2. Score and fold along the two dashed fold lines (mountain folds).
  3. Form a triangular prism: panel B faces forward, panel A wraps over
     the top to face backward, panel C tucks underneath as the base.
  4. Apply glue/tape to the GLUE TAB and stick it to the underside of
     panel C to lock the prism shape.
"""

from pathlib import Path
from PIL import Image, ImageDraw

DPI = 300
PAGE_W_IN, PAGE_H_IN = 8.5, 11.0
PANEL_W_IN, PANEL_H_IN = 4.4, 3.0  # ~3" tall badge (1.5:1) plus thin side margins
TAB_H_IN = 0.5
BADGE_PAD_IN = 0.15  # padding inside panel around the badge

HERE = Path(__file__).parent
BADGE_PATH = HERE / "hank-badge.png"
OUT_PDF = HERE / "hank-tent-letter.pdf"
OUT_PNG = HERE / "hank-tent-letter.png"


def inch(v: float) -> int:
    return int(round(v * DPI))


def draw_dashed_line(draw: ImageDraw.ImageDraw, p0, p1, dash=12, gap=8, width=2, fill=(0, 0, 0)):
    x0, y0 = p0
    x1, y1 = p1
    if y0 == y1:  # horizontal
        x = x0
        while x < x1:
            x_end = min(x + dash, x1)
            draw.line([(x, y0), (x_end, y0)], fill=fill, width=width)
            x += dash + gap
    else:  # vertical (not used here, but supported)
        y = y0
        while y < y1:
            y_end = min(y + dash, y1)
            draw.line([(x0, y), (x0, y_end)], fill=fill, width=width)
            y += dash + gap


def fit_badge(badge: Image.Image, max_w: int, max_h: int) -> Image.Image:
    bw, bh = badge.size
    scale = min(max_w / bw, max_h / bh)
    nw, nh = max(1, int(bw * scale)), max(1, int(bh * scale))
    return badge.resize((nw, nh), Image.LANCZOS)


def main() -> None:
    page_w, page_h = inch(PAGE_W_IN), inch(PAGE_H_IN)
    panel_w, panel_h = inch(PANEL_W_IN), inch(PANEL_H_IN)
    tab_h = inch(TAB_H_IN)
    pad = inch(BADGE_PAD_IN)

    total_h = tab_h + 3 * panel_h
    left = (page_w - panel_w) // 2
    top = (page_h - total_h) // 2

    page = Image.new("RGB", (page_w, page_h), (255, 255, 255))
    draw = ImageDraw.Draw(page)

    # Load and prep badge
    badge = Image.open(BADGE_PATH).convert("RGBA")
    sized = fit_badge(badge, panel_w - 2 * pad, panel_h - 2 * pad)

    # Y positions of horizontal boundaries
    y_tab_top = top
    y_tab_bot = top + tab_h          # cut line: top edge of panel A (and bottom of tab)
    y_a_bot = y_tab_bot + panel_h    # fold line between A and B
    y_b_bot = y_a_bot + panel_h      # fold line between B and C
    y_c_bot = y_b_bot + panel_h      # cut line: bottom edge of panel C
    x_left = left
    x_right = left + panel_w

    # Place badge on Panel B (upright)
    bx = x_left + (panel_w - sized.width) // 2
    by_b = y_a_bot + (panel_h - sized.height) // 2
    page.paste(sized, (bx, by_b), sized)

    # Place badge on Panel A (rotated 180 so it reads upright when folded over)
    sized_180 = sized.rotate(180)
    by_a = y_tab_bot + (panel_h - sized.height) // 2
    page.paste(sized_180, (bx, by_a), sized_180)

    # Cut lines (solid black) — outer rectangle including glue tab
    cut_w = 3
    draw.rectangle([(x_left, y_tab_top), (x_right, y_c_bot)], outline=(0, 0, 0), width=cut_w)

    # Glue-tab divider (cut/score line between tab and panel A) — solid
    draw.line([(x_left, y_tab_bot), (x_right, y_tab_bot)], fill=(0, 0, 0), width=cut_w)

    # Fold lines (dashed)
    draw_dashed_line(draw, (x_left, y_a_bot), (x_right, y_a_bot), dash=18, gap=10, width=2)
    draw_dashed_line(draw, (x_left, y_b_bot), (x_right, y_b_bot), dash=18, gap=10, width=2)

    # Labels
    try:
        from PIL import ImageFont
        font = ImageFont.truetype("arial.ttf", 28)
        small = ImageFont.truetype("arial.ttf", 20)
    except Exception:
        font = ImageFont.load_default()
        small = font

    def centered_text(y, text, f=small, fill=(120, 120, 120)):
        bbox = draw.textbbox((0, 0), text, font=f)
        tw = bbox[2] - bbox[0]
        draw.text((x_left + (panel_w - tw) // 2, y), text, font=f, fill=fill)

    # Tab label
    centered_text(y_tab_top + tab_h // 2 - 14, "GLUE TAB", f=small)

    # Fold line labels (just above each fold)
    centered_text(y_a_bot - 26, "fold", f=small, fill=(150, 150, 150))
    centered_text(y_b_bot - 26, "fold", f=small, fill=(150, 150, 150))

    # Panel C label
    centered_text(y_b_bot + panel_h // 2 - 14, "BASE", f=small, fill=(180, 180, 180))

    # Save outputs
    page.save(OUT_PNG, dpi=(DPI, DPI))
    page.save(OUT_PDF, "PDF", resolution=DPI)
    print(f"wrote {OUT_PDF}")
    print(f"wrote {OUT_PNG}")


if __name__ == "__main__":
    main()
