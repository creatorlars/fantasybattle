"""Build a printable PDF from gm-reference.md using reportlab Platypus.

Avoids xhtml2pdf/renderPM (which require cairo/_rl_renderPM not available on
Python 3.14 Windows installs). Parses a deliberately small, controlled subset
of Markdown that matches what gm-reference.md actually uses.
"""
from __future__ import annotations
import re
from pathlib import Path

from reportlab.lib.pagesizes import LETTER
from reportlab.lib.styles import ParagraphStyle
from reportlab.lib.enums import TA_LEFT, TA_CENTER
from reportlab.lib import colors
from reportlab.lib.units import inch
from reportlab.platypus import (
    BaseDocTemplate, PageTemplate, Frame, Paragraph, Spacer, Table, TableStyle,
    Preformatted, KeepTogether, HRFlowable, PageBreak,
)

HERE = Path(__file__).parent
MD = HERE / "gm-reference.md"
OUT = HERE / "gm-reference.pdf"

# ---------- colors (statcard aesthetic: black/white only) ----------
BLACK      = colors.black
WHITE      = colors.white
TEXT       = colors.black
MUTED      = colors.HexColor("#444444")
CODE_BG    = colors.HexColor("#f2f2f2")
ROW_ALT    = colors.HexColor("#f4f4f4")

# ---------- styles ----------
def make_styles():
    base = dict(fontName="Helvetica", fontSize=9.5, leading=12, textColor=TEXT)
    s = {
        "h1": ParagraphStyle("h1", fontName="Helvetica-Bold", fontSize=18,
                             leading=22, textColor=WHITE, alignment=TA_CENTER,
                             spaceAfter=0, spaceBefore=0),
        "subtitle": ParagraphStyle("sub", fontName="Helvetica-Oblique",
                                   fontSize=10, leading=13, textColor=BLACK,
                                   alignment=TA_CENTER, spaceAfter=8,
                                   spaceBefore=4),
        "h2_white": ParagraphStyle("h2w", fontName="Helvetica-Bold",
                                   fontSize=12, leading=14, textColor=WHITE,
                                   alignment=TA_LEFT),
        "h3": ParagraphStyle("h3", fontName="Helvetica-Bold", fontSize=11,
                             leading=14, textColor=BLACK,
                             spaceBefore=6, spaceAfter=3, keepWithNext=True),
        "body": ParagraphStyle("body", spaceAfter=4, **base),
        "li":   ParagraphStyle("li", leftIndent=14, bulletIndent=2,
                               spaceAfter=2, **base),
        "blockquote": ParagraphStyle("bq", fontName="Helvetica-Oblique",
                                     fontSize=9.5, leading=12, textColor=BLACK,
                                     leftIndent=14, rightIndent=14,
                                     spaceBefore=6, spaceAfter=6,
                                     alignment=TA_CENTER),
        "tabh": ParagraphStyle("tabh", fontName="Helvetica-Bold",
                               fontSize=9, leading=11, textColor=WHITE),
        "tab":  ParagraphStyle("tab", fontName="Helvetica",
                               fontSize=9, leading=11, textColor=TEXT),
        "code": ParagraphStyle("code", fontName="Courier", fontSize=8.5,
                               leading=10.5, textColor=TEXT,
                               backColor=CODE_BG,
                               borderColor=BLACK, borderWidth=0.5,
                               borderPadding=(6, 8, 6, 8),
                               spaceBefore=6, spaceAfter=6),
    }
    return s

STY = make_styles()

# ---------- inline markdown -> reportlab mini-html ----------
INLINE_BOLD_ITAL = re.compile(r"\*\*\*(.+?)\*\*\*")
INLINE_BOLD      = re.compile(r"\*\*(.+?)\*\*")
INLINE_ITAL      = re.compile(r"(?<!\*)\*(?!\*)(.+?)(?<!\*)\*(?!\*)")
INLINE_CODE      = re.compile(r"`([^`]+)`")

def inline(text: str) -> str:
    # escape XML special chars first
    text = (text.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;"))
    text = INLINE_CODE.sub(
        r'<font name="Courier" backColor="#f2f2f2">\1</font>', text)
    text = INLINE_BOLD_ITAL.sub(r"<b><i>\1</i></b>", text)
    text = INLINE_BOLD.sub(r"<b>\1</b>", text)
    text = INLINE_ITAL.sub(r"<i>\1</i>", text)
    return text

# ---------- markdown parser ----------
def parse_table(lines: list[str], i: int):
    """Parse a GH-flavored table starting at i. Returns (flowable, new_i)."""
    header = [c.strip() for c in lines[i].strip().strip("|").split("|")]
    sep = lines[i+1]
    if not re.match(r"^\s*\|?\s*:?-+", sep):
        return None, i
    rows = []
    j = i + 2
    while j < len(lines) and lines[j].strip().startswith("|"):
        rows.append([c.strip() for c in lines[j].strip().strip("|").split("|")])
        j += 1
    data = [[Paragraph(inline(c), STY["tabh"]) for c in header]] + \
           [[Paragraph(inline(c), STY["tab"]) for c in r] for r in rows]
    ncols = len(header)
    avail = 7.0 * inch
    # content-aware column widths: estimate per-column max text length, then
    # allocate proportionally with a minimum floor so narrow stat columns
    # don't squeeze a wide label column into character-by-character wrapping.
    raw_rows = [header] + rows
    # strip simple markdown emphasis for width estimation
    def _plain(s: str) -> str:
        return re.sub(r"[*`]", "", s)
    max_len = [max(len(_plain(row[c])) if c < len(row) else 0
                   for row in raw_rows) for c in range(ncols)]
    # weight = max(min_chars, max_len); convert to width fractions
    weights = [max(3, ml) for ml in max_len]
    total_w = sum(weights) or 1
    col_w = [avail * (w / total_w) for w in weights]
    # per-column minimum: enough to fit the longest single word in the column
    # at body font (~0.08" per char @ 9pt Helvetica bold + padding), capped at 1.6"
    char_w = 0.08 * inch
    def _longest_word(c):
        words = []
        for row in raw_rows:
            if c < len(row):
                words.extend(_plain(row[c]).split())
        return max((len(w) for w in words), default=3)
    min_per_col = [min(1.6 * inch, max(0.32 * inch, _longest_word(c) * char_w + 0.18 * inch))
                   for c in range(ncols)]
    deficit = sum(max(0, mn - w) for mn, w in zip(min_per_col, col_w))
    if deficit > 0:
        # take width back from columns that have the most slack above their own min
        order = sorted(range(ncols), key=lambda c: -(col_w[c] - min_per_col[c]))
        for idx in order:
            if deficit <= 0:
                break
            slack = col_w[idx] - min_per_col[idx]
            if slack > 0:
                give = min(slack, deficit)
                col_w[idx] -= give
                deficit -= give
        for c in range(ncols):
            col_w[c] = max(col_w[c], min_per_col[c])
    t = Table(data, colWidths=col_w, repeatRows=1, hAlign="LEFT")
    style = [
        ("BACKGROUND", (0, 0), (-1, 0), CREAM),
        ("TEXTCOLOR", (0, 0), (-1, 0), GOLD_DARK),
        ("GRID", (0, 0), (-1, -1), 0.4, colors.HexColor("#999999")),
        ("VALIGN", (0, 0), (-1, -1), "TOP"),
        ("LEFTPADDING", (0, 0), (-1, -1), 4),
        ("RIGHTPADDING", (0, 0), (-1, -1), 4),
        ("TOPPADDING", (0, 0), (-1, -1), 3),
        ("BOTTOMPADDING", (0, 0), (-1, -1), 3),
    ]
    for r in range(2, len(data), 2):
        style.append(("BACKGROUND", (0, r), (-1, r), CREAM_LITE))
    t.setStyle(TableStyle(style))
    return t, j

def parse_md(md: str):
    lines = md.splitlines()
    out = []
    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.strip()

        # blank line
        if not stripped:
            i += 1
            continue

        # horizontal rule
        if re.match(r"^-{3,}\s*$", stripped):
            out.append(Spacer(1, 4))
            out.append(HRFlowable(width="100%", thickness=0.7, color=GOLD,
                                  spaceBefore=2, spaceAfter=4))
            i += 1
            continue

        # heading
        m = re.match(r"^(#{1,3})\s+(.*)$", stripped)
        if m:
            level = len(m.group(1))
            text = m.group(2).strip()
            if level == 1:
                # split off "##" subtitle if next line is "## something"
                out.append(Paragraph(inline(text), STY["h1"]))
                out.append(HRFlowable(width="100%", thickness=1.5,
                                      color=GOLD, spaceBefore=2, spaceAfter=4))
                # peek for italic subtitle line "## *...*"
                if i+1 < len(lines):
                    nxt = lines[i+1].strip()
                    if nxt.startswith("## "):
                        sub = nxt[3:].strip()
                        out.append(Paragraph(inline(sub), STY["subtitle"]))
                        i += 1
            elif level == 2:
                out.append(Paragraph(inline(text), STY["h2"]))
            else:
                out.append(Paragraph(inline(text), STY["h3"]))
            i += 1
            continue

        # fenced code
        if stripped.startswith("```"):
            j = i + 1
            buf = []
            while j < len(lines) and not lines[j].strip().startswith("```"):
                buf.append(lines[j])
                j += 1
            code_text = "\n".join(buf)
            out.append(Preformatted(code_text, STY["code"]))
            i = j + 1
            continue

        # blockquote
        if stripped.startswith(">"):
            buf = []
            while i < len(lines) and lines[i].strip().startswith(">"):
                bl = lines[i].strip()[1:].lstrip()
                buf.append(bl)
                i += 1
            # join paragraphs separated by blank quote-lines
            paragraphs = []
            cur = []
            for b in buf:
                if not b:
                    if cur:
                        paragraphs.append(" ".join(cur))
                        cur = []
                else:
                    cur.append(b)
            if cur:
                paragraphs.append(" ".join(cur))
            for p in paragraphs:
                out.append(Paragraph(inline(p), STY["blockquote"]))
            continue

        # table
        if stripped.startswith("|") and i+1 < len(lines) and \
                re.match(r"^\s*\|?\s*:?-+", lines[i+1]):
            flow, ni = parse_table(lines, i)
            if flow is not None:
                out.append(flow)
                i = ni
                continue

        # ordered list
        m = re.match(r"^(\d+)\.\s+(.*)$", line)
        if m:
            buf = []
            while i < len(lines):
                m2 = re.match(r"^(\d+)\.\s+(.*)$", lines[i])
                if not m2:
                    break
                buf.append(m2.group(2))
                i += 1
            for n, item in enumerate(buf, start=1):
                out.append(Paragraph(f"<b>{n}.</b> " + inline(item), STY["li"]))
            continue

        # unordered list
        if re.match(r"^[-*]\s+", line):
            buf = []
            while i < len(lines) and re.match(r"^[-*]\s+", lines[i]):
                buf.append(re.sub(r"^[-*]\s+", "", lines[i]))
                i += 1
            for item in buf:
                out.append(Paragraph("• " + inline(item), STY["li"]))
            continue

        # paragraph (gather following non-blank, non-special lines)
        buf = [line]
        i += 1
        while i < len(lines):
            nxt = lines[i]
            ns = nxt.strip()
            if (not ns
                    or ns.startswith("#")
                    or ns.startswith(">")
                    or ns.startswith("|")
                    or ns.startswith("```")
                    or re.match(r"^[-*]\s+", ns)
                    or re.match(r"^\d+\.\s+", ns)
                    or re.match(r"^-{3,}\s*$", ns)):
                break
            buf.append(nxt)
            i += 1
        para_text = " ".join(b.strip() for b in buf)
        out.append(Paragraph(inline(para_text), STY["body"]))

    return out

# ---------- footer ----------
def on_page(canvas, doc):
    canvas.saveState()
    canvas.setFont("Helvetica-Oblique", 7.5)
    canvas.setFillColor(colors.HexColor("#888888"))
    txt = (f"Dawn of the Golden Spawn 2: Lilypad Hell  —  GM Reference  —  "
           f"for GM eyes only  —  page {doc.page}")
    canvas.drawCentredString(LETTER[0] / 2.0, 0.35 * inch, txt)
    canvas.restoreState()

def main():
    md = MD.read_text(encoding="utf-8")
    flowables = parse_md(md)

    doc = BaseDocTemplate(
        str(OUT), pagesize=LETTER,
        leftMargin=0.7 * inch, rightMargin=0.7 * inch,
        topMargin=0.6 * inch, bottomMargin=0.6 * inch,
        title="Lilypad Hell — GM Reference",
        author="Lars Larsen",
    )
    frame = Frame(doc.leftMargin, doc.bottomMargin,
                  doc.width, doc.height, id="normal",
                  leftPadding=0, rightPadding=0,
                  topPadding=0, bottomPadding=0)
    doc.addPageTemplates([PageTemplate(id="main", frames=[frame],
                                       onPage=on_page)])
    doc.build(flowables)
    print(f"Wrote {OUT} ({OUT.stat().st_size:,} bytes)")

if __name__ == "__main__":
    main()
