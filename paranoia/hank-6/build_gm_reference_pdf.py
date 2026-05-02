"""Build the HANKSYNC GM Reference PDF.

Loads `hanksync_convention_packet_v3.md`, surgically excises the player-print
zones (which already live in `hanksync_player_handouts.pdf`), and renders the
remainder through a small reportlab Platypus pipeline themed in HANKSYNC red.

Excised regions, each replaced by a one-block pointer to the player handouts:
    Sec 4   — six persona briefings
    Sec 5.1 — "All Hail The Computer" lyric sheet
    Sec 5.2 — "Hanks Of The Heart" lyric sheet
    Sec 6.2 — twelve sealed-envelope society notes

Section 5.3 (Finale Reprise) is GM-only and stays.

GM gets to keep: contents, reading order, safety, scaling, rules, mission
briefing, premise, Codex, distracted-Computer note, Quickstart, the Hanks +
character sheets, structure, song GM cues, society GM table (6.1), NPC roster,
escalation, techniques, gear, props checklist, quick lines, master tables,
closing line.
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
    Preformatted, HRFlowable,
)

HERE = Path(__file__).parent
SRC = HERE / "hanksync_convention_packet_v3.md"
OUT = HERE / "hanksync_gm_reference.pdf"

# ---------- colors (HANKSYNC red on white) ----------
HANKSYNC_RED   = colors.HexColor("#B0001A")
HANKSYNC_DARK  = colors.HexColor("#7A0012")
HANKSYNC_TINT  = colors.HexColor("#F8E5E8")
HANKSYNC_TINT2 = colors.HexColor("#FBF1F3")
BLACK          = colors.black
WHITE          = colors.white
TEXT           = colors.black
MUTED          = colors.HexColor("#444444")
CODE_BG        = colors.HexColor("#f2f2f2")

# ---------- styles ----------
def make_styles():
    base = dict(fontName="Helvetica", fontSize=9.5, leading=12, textColor=TEXT)
    return {
        "h1": ParagraphStyle(
            "h1", fontName="Helvetica-Bold", fontSize=20, leading=24,
            textColor=HANKSYNC_RED, alignment=TA_CENTER,
            spaceBefore=2, spaceAfter=4),
        "subtitle": ParagraphStyle(
            "sub", fontName="Helvetica-Oblique", fontSize=10.5, leading=13,
            textColor=BLACK, alignment=TA_CENTER, spaceAfter=8, spaceBefore=2),
        "h2": ParagraphStyle(
            "h2", fontName="Helvetica-Bold", fontSize=14, leading=17,
            textColor=WHITE, alignment=TA_LEFT, backColor=HANKSYNC_RED,
            borderPadding=(4, 6, 4, 6), spaceBefore=14, spaceAfter=6,
            keepWithNext=True),
        "h3": ParagraphStyle(
            "h3", fontName="Helvetica-Bold", fontSize=11.5, leading=14,
            textColor=HANKSYNC_DARK, spaceBefore=8, spaceAfter=3,
            keepWithNext=True),
        "h4": ParagraphStyle(
            "h4", fontName="Helvetica-Bold", fontSize=10.5, leading=13,
            textColor=BLACK, spaceBefore=6, spaceAfter=2, keepWithNext=True),
        "body": ParagraphStyle("body", spaceAfter=4, **base),
        "li":   ParagraphStyle(
            "li", leftIndent=14, bulletIndent=2, spaceAfter=2, **base),
        "blockquote": ParagraphStyle(
            "bq", fontName="Helvetica-Oblique", fontSize=9.5, leading=12.5,
            textColor=BLACK, leftIndent=14, rightIndent=14,
            backColor=HANKSYNC_TINT2, borderColor=HANKSYNC_RED,
            borderWidth=0, borderPadding=(4, 8, 4, 8),
            spaceBefore=6, spaceAfter=6),
        "pointer": ParagraphStyle(
            "pointer", fontName="Helvetica-Bold", fontSize=10, leading=13,
            textColor=HANKSYNC_DARK, alignment=TA_CENTER,
            backColor=HANKSYNC_TINT, borderColor=HANKSYNC_RED, borderWidth=0.6,
            borderPadding=(8, 10, 8, 10),
            spaceBefore=8, spaceAfter=8),
        "tabh": ParagraphStyle(
            "tabh", fontName="Helvetica-Bold", fontSize=8.5, leading=10.5,
            textColor=WHITE),
        "tab":  ParagraphStyle(
            "tab", fontName="Helvetica", fontSize=8.5, leading=10.5,
            textColor=TEXT),
        "code": ParagraphStyle(
            "code", fontName="Courier", fontSize=8.5, leading=10.5,
            textColor=TEXT, backColor=CODE_BG, borderColor=BLACK,
            borderWidth=0.5, borderPadding=(6, 8, 6, 8),
            spaceBefore=6, spaceAfter=6),
    }

STY = make_styles()

# ---------- inline markdown -> reportlab mini-html ----------
INLINE_BOLD_ITAL = re.compile(r"\*\*\*(.+?)\*\*\*")
INLINE_BOLD      = re.compile(r"\*\*(.+?)\*\*")
INLINE_ITAL      = re.compile(r"(?<!\*)\*(?!\*)(.+?)(?<!\*)\*(?!\*)")
INLINE_CODE      = re.compile(r"`([^`]+)`")

def inline(text: str) -> str:
    text = (text.replace("&", "&amp;")
                .replace("<", "&lt;").replace(">", "&gt;"))
    text = INLINE_CODE.sub(
        r'<font name="Courier" backColor="#f2f2f2">\1</font>', text)
    text = INLINE_BOLD_ITAL.sub(r"<b><i>\1</i></b>", text)
    text = INLINE_BOLD.sub(r"<b>\1</b>", text)
    text = INLINE_ITAL.sub(r"<i>\1</i>", text)
    return text

# ---------- markdown parser ----------
def parse_table(lines, i):
    header = [c.strip() for c in lines[i].strip().strip("|").split("|")]
    if i + 1 >= len(lines) or not re.match(r"^\s*\|?\s*:?-+", lines[i + 1]):
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
    raw = [header] + rows
    def _plain(s): return re.sub(r"[*`]", "", s)
    max_len = [max(len(_plain(r[c])) if c < len(r) else 0 for r in raw)
               for c in range(ncols)]
    weights = [max(3, m) for m in max_len]
    total = sum(weights) or 1
    col_w = [avail * (w / total) for w in weights]
    char_w = 0.075 * inch
    def longest_word(c):
        words = []
        for r in raw:
            if c < len(r):
                words.extend(_plain(r[c]).split())
        return max((len(w) for w in words), default=3)
    min_per = [min(1.6 * inch, max(0.32 * inch,
                                   longest_word(c) * char_w + 0.16 * inch))
               for c in range(ncols)]
    deficit = sum(max(0, mn - w) for mn, w in zip(min_per, col_w))
    if deficit > 0:
        order = sorted(range(ncols), key=lambda c: -(col_w[c] - min_per[c]))
        for k in order:
            if deficit <= 0:
                break
            slack = col_w[k] - min_per[k]
            if slack > 0:
                give = min(slack, deficit)
                col_w[k] -= give
                deficit -= give
        for c in range(ncols):
            col_w[c] = max(col_w[c], min_per[c])
    t = Table(data, colWidths=col_w, repeatRows=1, hAlign="LEFT")
    style = [
        ("BACKGROUND", (0, 0), (-1, 0), HANKSYNC_RED),
        ("TEXTCOLOR", (0, 0), (-1, 0), WHITE),
        ("GRID", (0, 0), (-1, -1), 0.4, colors.HexColor("#999999")),
        ("VALIGN", (0, 0), (-1, -1), "TOP"),
        ("LEFTPADDING", (0, 0), (-1, -1), 4),
        ("RIGHTPADDING", (0, 0), (-1, -1), 4),
        ("TOPPADDING", (0, 0), (-1, -1), 3),
        ("BOTTOMPADDING", (0, 0), (-1, -1), 3),
    ]
    for r in range(2, len(data), 2):
        style.append(("BACKGROUND", (0, r), (-1, r), HANKSYNC_TINT2))
    t.setStyle(TableStyle(style))
    return t, j

def parse_md(md):
    lines = md.splitlines()
    out = []
    i = 0
    seen_h1 = False
    while i < len(lines):
        line = lines[i]
        stripped = line.strip()

        if not stripped:
            i += 1
            continue

        # horizontal rule
        if re.match(r"^-{3,}\s*$", stripped):
            out.append(HRFlowable(width="100%", thickness=0.6,
                                  color=HANKSYNC_RED,
                                  spaceBefore=2, spaceAfter=4))
            i += 1
            continue

        # heading
        m = re.match(r"^(#{1,4})\s+(.*)$", stripped)
        if m:
            level = len(m.group(1))
            text = m.group(2).strip()
            if level == 1:
                out.append(Paragraph(inline(text), STY["h1"]))
                # collect any consecutive `### ...` lines that follow as
                # subtitle (matches the packet's title block).
                j = i + 1
                while j < len(lines) and lines[j].strip().startswith("### "):
                    sub = lines[j].strip()[4:].strip()
                    out.append(Paragraph(inline(sub), STY["subtitle"]))
                    j += 1
                # optional italic version line
                if j < len(lines) and lines[j].strip().startswith("*"):
                    out.append(Paragraph(inline(lines[j].strip()),
                                         STY["subtitle"]))
                    j += 1
                out.append(HRFlowable(width="100%", thickness=1.2,
                                      color=HANKSYNC_RED,
                                      spaceBefore=2, spaceAfter=8))
                i = j
                seen_h1 = True
                continue
            elif level == 2:
                out.append(Paragraph(inline(text), STY["h2"]))
            elif level == 3:
                # Title block subtitle handled above; here it's a real h3.
                if not seen_h1:
                    out.append(Paragraph(inline(text), STY["subtitle"]))
                else:
                    out.append(Paragraph(inline(text), STY["h3"]))
            else:
                out.append(Paragraph(inline(text), STY["h4"]))
            i += 1
            continue

        # fenced code
        if stripped.startswith("```"):
            j = i + 1
            buf = []
            while j < len(lines) and not lines[j].strip().startswith("```"):
                buf.append(lines[j])
                j += 1
            out.append(Preformatted("\n".join(buf), STY["code"]))
            i = j + 1
            continue

        # blockquote (consecutive lines starting with >)
        if stripped.startswith(">"):
            buf = []
            while i < len(lines) and lines[i].strip().startswith(">"):
                bl = lines[i].strip()[1:].lstrip()
                buf.append(bl)
                i += 1
            paragraphs = []
            cur = []
            for b in buf:
                if not b:
                    if cur:
                        paragraphs.append(" ".join(cur))
                        cur = []
                else:
                    # honor inline bullet lines inside a blockquote
                    cur.append(b)
            if cur:
                paragraphs.append(" ".join(cur))
            for p in paragraphs:
                out.append(Paragraph(inline(p), STY["blockquote"]))
            continue

        # table
        if stripped.startswith("|") and i + 1 < len(lines) and \
                re.match(r"^\s*\|?\s*:?-+", lines[i + 1]):
            flow, ni = parse_table(lines, i)
            if flow is not None:
                out.append(flow)
                i = ni
                continue

        # ordered list
        m = re.match(r"^(\d+)\.\s+(.*)$", line)
        if m:
            n_buf = []
            while i < len(lines):
                m2 = re.match(r"^(\d+)\.\s+(.*)$", lines[i])
                if not m2:
                    break
                n_buf.append(m2.group(2))
                i += 1
            for n, item in enumerate(n_buf, start=1):
                out.append(Paragraph(f"<b>{n}.</b> " + inline(item),
                                     STY["li"]))
            continue

        # unordered list
        if re.match(r"^[-*]\s+", line):
            buf = []
            while i < len(lines) and re.match(r"^[-*]\s+", lines[i]):
                buf.append(re.sub(r"^[-*]\s+", "", lines[i]))
                i += 1
            for item in buf:
                out.append(Paragraph("\u2022 " + inline(item), STY["li"]))
            continue

        # paragraph (gather following non-special lines)
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
        out.append(Paragraph(inline(" ".join(b.strip() for b in buf)),
                             STY["body"]))
    return out

# ---------- excisions: surgically remove player-print zones ----------
def excise(md: str) -> str:
    lines = md.splitlines()

    def find(predicate, start=0):
        for k in range(start, len(lines)):
            if predicate(lines[k]):
                return k
        raise ValueError("anchor not found")

    # --- Section 4: persona briefings ----------------------------------
    a = find(lambda s: s.startswith(
        "> \u2702 \u2014 \u2014 \u2014 \u2014 \u2014 \u2014 \u2014 \u2014"
        " \u2014 \u2014 PLAYER HANDOUTS BEGIN BELOW"))
    b = find(lambda s: s.startswith("## 5 \u00b7 SONGS"), a)
    pointer4 = [
        "",
        "> \u2192 **Player handouts.** The six in-fiction persona briefings"
        " (HANK-1 through HANK-6) are printed in"
        " `hanksync_player_handouts.pdf`, pages 2\u20137 \u2014 one per Hank."
        " Distribute privately during Act 1 Beat 2 (Sec 3).",
        "",
    ]
    lines = lines[:a] + pointer4 + lines[b:]

    # --- Section 5.1: "All Hail The Computer" lyrics -------------------
    a = find(lambda s: s.startswith(
        "> \u2702 \u2014 \u2014 \u2014 \u2014 \u2014 \u2014 \u2014 \u2014"
        " \u2014 \u2014 LYRIC SHEET (PRINT) BEGINS BELOW"))
    b = find(lambda s: s.startswith("### 5.2 \u2014"), a)
    pointer51 = [
        "",
        "> \u2192 **Player handout.** Full lyric sheet for"
        " *\u201cAll Hail The Computer\u201d* (Group Number) is in"
        " `hanksync_player_handouts.pdf`, pages 8\u20139. Print one per Hank"
        " plus one for the Music Director food-vat.",
        "",
    ]
    lines = lines[:a] + pointer51 + lines[b:]

    # --- Section 5.2: "Hanks Of The Heart" lyrics ----------------------
    a = find(lambda s: s.startswith(
        "> \u2702 \u2014 \u2014 \u2014 \u2014 \u2014 \u2014 \u2014 \u2014"
        " \u2014 \u2014 LYRIC SHEET (PRINT) BEGINS BELOW"))
    b = find(lambda s: s.startswith("### 5.3 \u2014"), a)
    pointer52 = [
        "",
        "> \u2192 **Player handout.** Full lyric sheet for"
        " *\u201cHanks Of The Heart\u201d* (Solo Suite, six verses) is in"
        " `hanksync_player_handouts.pdf`, pages 10\u201312. Print one per"
        " Hank.",
        "",
    ]
    lines = lines[:a] + pointer52 + lines[b:]

    # --- Section 6.2: society envelope notes ---------------------------
    a = find(lambda s: s.startswith(
        "> \u2702 \u2014 \u2014 \u2014 \u2014 \u2014 \u2014 \u2014 \u2014"
        " \u2014 \u2014 PLAYER HANDOUTS \u2014 PRINT ONE PER PLAYER"))
    b = find(lambda s: s.startswith("## 7 \u00b7 NPC ROSTER"), a)
    pointer62 = [
        "",
        "> \u2192 **Player handouts.** The twelve in-fiction sealed-envelope"
        " society notes are printed in"
        " `hanksync_player_handouts.pdf`, pages 13\u201318 (two-up with"
        " dashed cut line). Cut, fold, seal in unmarked envelopes labeled"
        " only with the recipient Hank's number. Hand the matching"
        " contraband from Sec 10.5 to the player when he opens his.",
        "",
    ]
    lines = lines[:a] + pointer62 + lines[b:]

    return "\n".join(lines)

# ---------- page chrome ----------
def on_page(canvas, doc):
    canvas.saveState()
    # top bar
    canvas.setFillColor(HANKSYNC_RED)
    canvas.rect(0, LETTER[1] - 0.32 * inch, LETTER[0], 0.32 * inch,
                stroke=0, fill=1)
    canvas.setFillColor(WHITE)
    canvas.setFont("Helvetica-Bold", 9)
    canvas.drawString(0.5 * inch, LETTER[1] - 0.21 * inch,
                      "HANKSYNC \u2014 GM REFERENCE")
    canvas.drawRightString(LETTER[0] - 0.5 * inch, LETTER[1] - 0.21 * inch,
                           "FOR GM EYES ONLY \u00b7 ULTRAVIOLET CLEARANCE")
    # footer
    canvas.setFont("Helvetica-Oblique", 7.5)
    canvas.setFillColor(colors.HexColor("#888888"))
    canvas.drawCentredString(
        LETTER[0] / 2.0, 0.32 * inch,
        f"HANKSYNC \u00b7 v3.1 \u00b7 GM Reference \u00b7 page {doc.page} \u00b7"
        f" The Computer is your friend.")
    canvas.restoreState()

def main():
    md = SRC.read_text(encoding="utf-8")
    md = excise(md)
    flowables = parse_md(md)

    doc = BaseDocTemplate(
        str(OUT), pagesize=LETTER,
        leftMargin=0.7 * inch, rightMargin=0.7 * inch,
        topMargin=0.7 * inch, bottomMargin=0.6 * inch,
        title="HANKSYNC \u2014 GM Reference",
        author="HPD&MC Performance Optimization Subroutine",
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
