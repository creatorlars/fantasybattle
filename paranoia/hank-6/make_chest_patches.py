"""Generate a printable letter-size sheet of six numbered HANK chest patches.

Layout (US Letter portrait, 8.5" x 11"): 2 columns x 3 rows of HELLO-MY-NAME-IS
patches in regulation HANKSYNC red, each pre-filled with HANK-1 .. HANK-6.

Cut along the solid black borders. Velcro to the regulation freshly-laundered
combat fatigues (red).
"""
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont

DPI = 300
PAGE_W_IN, PAGE_H_IN = 8.5, 11.0

# Patch sized to match the existing badge aspect ratio (~3:2). 3.6 x 2.4 in.
PATCH_W_IN, PATCH_H_IN = 3.6, 2.4
COLS, ROWS = 2, 3
COL_GAP_IN, ROW_GAP_IN = 0.25, 0.25

HANKSYNC_RED = (176, 0, 26)   # #B0001A
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)

HERE = Path(__file__).parent
OUT_PDF = HERE / "hank-chest-patches-letter.pdf"
OUT_PNG = HERE / "hank-chest-patches-letter.png"


def inch(v: float) -> int:
    return int(round(v * DPI))


def load_font(size: int, bold: bool = False) -> ImageFont.ImageFont:
    candidates = (
        ("arialbd.ttf" if bold else "arial.ttf"),
        ("DejaVuSans-Bold.ttf" if bold else "DejaVuSans.ttf"),
    )
    for name in candidates:
        try:
            return ImageFont.truetype(name, size)
        except Exception:
            continue
    return ImageFont.load_default()


def draw_patch(canvas: Image.Image, x: int, y: int, w: int, h: int, label: str) -> None:
    draw = ImageDraw.Draw(canvas)

    # Outer cut border (thin black rectangle).
    draw.rectangle([(x, y), (x + w, y + h)], outline=BLACK, width=2)

    # Rounded "patch" body fills the cut rectangle with a small inset so the
    # cut line is visible. Use full rect; rounded look comes from the red
    # banner stripes top/bottom.
    inner = (x + 2, y + 2, x + w - 2, y + h - 2)
    draw.rectangle(inner, fill=WHITE)

    # Top red banner ~28% of patch height. Bottom red banner ~10%.
    banner_top_h = int(h * 0.28)
    banner_bot_h = int(h * 0.10)
    draw.rectangle([(x + 2, y + 2),
                    (x + w - 2, y + 2 + banner_top_h)],
                   fill=HANKSYNC_RED)
    draw.rectangle([(x + 2, y + h - 2 - banner_bot_h),
                    (x + w - 2, y + h - 2)],
                   fill=HANKSYNC_RED)

    # Top-banner text: "HELLO" bold + "my name is" smaller.
    hello_size = int(h * 0.13)
    sub_size = int(h * 0.055)
    f_hello = load_font(hello_size, bold=True)
    f_sub = load_font(sub_size, bold=False)

    def centered(text, font, cy, fill=WHITE):
        tb = draw.textbbox((0, 0), text, font=font)
        tw = tb[2] - tb[0]
        th = tb[3] - tb[1]
        draw.text((x + (w - tw) // 2, cy - th // 2 - tb[1]),
                  text, font=font, fill=fill)

    # Layout inside the top banner: HELLO upper, "my name is" lower.
    hello_cy = y + 2 + int(banner_top_h * 0.40)
    sub_cy = y + 2 + int(banner_top_h * 0.78)
    centered("HELLO", f_hello, hello_cy)
    centered("my name is", f_sub, sub_cy)

    # Big label in the white middle area.
    label_size = int(h * 0.30)
    f_label = load_font(label_size, bold=True)
    label_cy = y + 2 + banner_top_h + (h - banner_top_h - banner_bot_h) // 2
    centered(label, f_label, label_cy, fill=BLACK)


def main() -> None:
    page_w, page_h = inch(PAGE_W_IN), inch(PAGE_H_IN)
    pw, ph = inch(PATCH_W_IN), inch(PATCH_H_IN)
    cgap, rgap = inch(COL_GAP_IN), inch(ROW_GAP_IN)

    grid_w = COLS * pw + (COLS - 1) * cgap
    grid_h = ROWS * ph + (ROWS - 1) * rgap
    left = (page_w - grid_w) // 2
    top = (page_h - grid_h) // 2

    page = Image.new("RGB", (page_w, page_h), WHITE)

    n = 1
    for row in range(ROWS):
        for col in range(COLS):
            x = left + col * (pw + cgap)
            y = top + row * (ph + rgap)
            draw_patch(page, x, y, pw, ph, f"HANK-{n}")
            n += 1

    # Footer caption (subtle).
    cap_font = load_font(20)
    cap = ("HANKSYNC \u00b7 numbered chest patches \u00b7 cut along solid lines "
           "\u00b7 affix Velcro to fatigues, regulation HANKSYNC red")
    cd = ImageDraw.Draw(page)
    tb = cd.textbbox((0, 0), cap, font=cap_font)
    cd.text(((page_w - (tb[2] - tb[0])) // 2, page_h - 60),
            cap, font=cap_font, fill=(140, 140, 140))

    page.save(OUT_PNG, dpi=(DPI, DPI))
    page.save(OUT_PDF, "PDF", resolution=DPI)
    print(f"wrote {OUT_PDF}")
    print(f"wrote {OUT_PNG}")


if __name__ == "__main__":
    main()
