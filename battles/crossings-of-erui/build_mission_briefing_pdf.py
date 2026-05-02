"""Build a single-page printable PDF from mission-briefing.md.

Visual style matches the unit statcards: black title bars with white text,
thin black grid borders, white background, sans-serif. No gold/parchment.
"""
from __future__ import annotations
import re
from pathlib import Path

from reportlab.lib.pagesizes import LETTER
from reportlab.lib.styles import ParagraphStyle
from reportlab.lib import colors
from reportlab.lib.units import inch
from reportlab.platypus import (
    BaseDocTemplate, PageTemplate, Frame, Paragraph, Spacer, Table, TableStyle,
)

HERE = Path(__file__).parent
MD = HERE / "mission-briefing.md"
OUT = HERE / "mission-briefing.pdf"

BLACK = colors.black
WHITE = colors.white
TEXT  = colors.black

def make_styles():
    base = dict(fontName="Helvetica", fontSize=9, leading=11.4, textColor=TEXT)
    return {
        "subtitle": ParagraphStyle("sub", fontName="Helvetica-Oblique",
                                   fontSize=9, leading=11, alignment=1,
                                   textColor=BLACK, spaceAfter=4,
                                   spaceBefore=2),
        "h2_white": ParagraphStyle("h2w", fontName="Helvetica-Bold",
                                   fontSize=11, leading=13, textColor=WHITE,
                                   alignment=0),
        "h3": ParagraphStyle("h3", fontName="Helvetica-Bold", fontSize=10,
                             leading=12, textColor=BLACK,
                             spaceBefore=4, spaceAfter=2, keepWithNext=True),
        "body": ParagraphStyle("body", spaceAfter=3, alignment=4, **base),
        "li":   ParagraphStyle("li", leftIndent=11, bulletIndent=1,
                               spaceAfter=0.6, **base),
        "blockquote": ParagraphStyle("bq", fontName="Helvetica-Oblique",
                                     fontSize=9, leading=11.5, textColor=BLACK,
                                     leftIndent=12, rightIndent=12,
                                     spaceBefore=3, spaceAfter=5,
                                     alignment=1),
        "tabh": ParagraphStyle("tabh", fontName="Helvetica-Bold",
                               fontSize=9, leading=11, textColor=WHITE,
                               alignment=0),
        "tab":  ParagraphStyle("tab", fontName="Helvetica",
                               fontSize=9, leading=11, textColor=TEXT),
    }

STY = make_styles()

INLINE_BOLD_ITAL = re.compile(r"\*\*\*(.+?)\*\*\*")
INLINE_BOLD      = re.compile(r"\*\*(.+?)\*\*")
INLINE_ITAL      = re.compile(r"(?<!\*)\*(?!\*)(.+?)(?<!\*)\*(?!\*)")

def inline(text: str) -> str:
    text = (text.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;"))
    text = INLINE_BOLD_ITAL.sub(r"<b><i>\1</i></b>", text)
    text = INLINE_BOLD.sub(r"<b>\1</b>", text)
    text = INLINE_ITAL.sub(r"<i>\1</i>", text)
    return text

def title_bar(text: str, font_size: float = 16):
    p = Paragraph(
        f'<para align="center"><font name="Helvetica-Bold" size="{font_size}" '
        f'color="white">{inline(text)}</font></para>',
        ParagraphStyle("tb", leading=font_size * 1.15))
    t = Table([[p]], colWidths=[7.3 * inch])
    t.setStyle(TableStyle([
        ("BACKGROUND", (0, 0), (-1, -1), BLACK),
        ("BOX", (0, 0), (-1, -1), 1.4, BLACK),
        ("LEFTPADDING", (0, 0), (-1, -1), 6),
        ("RIGHTPADDING", (0, 0), (-1, -1), 6),
        ("TOPPADDING", (0, 0), (-1, -1), 6),
        ("BOTTOMPADDING", (0, 0), (-1, -1), 6),
    ]))
    return t

def section_bar(text: str):
    p = Paragraph(inline(text), STY["h2_white"])
    t = Table([[p]], colWidths=[7.3 * inch])
    t.setStyle(TableStyle([
        ("BACKGROUND", (0, 0), (-1, -1), BLACK),
        ("BOX", (0, 0), (-1, -1), 1.2, BLACK),
        ("LEFTPADDING", (0, 0), (-1, -1), 8),
        ("RIGHTPADDING", (0, 0), (-1, -1), 6),
        ("TOPPADDING", (0, 0), (-1, -1), 4),
        ("BOTTOMPADDING", (0, 0), (-1, -1), 4),
    ]))
    return t

def parse_table(lines, i):
    header = [c.strip() for c in lines[i].strip().strip("|").split("|")]
    if not re.match(r"^\s*\|?\s*:?-+", lines[i+1]):
        return None, i
    rows = []
    j = i + 2
    while j < len(lines) and lines[j].strip().startswith("|"):
        rows.append([c.strip() for c in lines[j].strip().strip("|").split("|")])
        j += 1
    data = [[Paragraph(inline(c), STY["tabh"]) for c in header]] + \
           [[Paragraph(inline(c), STY["tab"]) for c in r] for r in rows]
    avail = 7.3 * inch
    ncols = len(header)
    if ncols == 2:
        col_w = [1.9 * inch, avail - 1.9 * inch]
    else:
        col_w = [avail / ncols] * ncols
    t = Table(data, colWidths=col_w, repeatRows=1, hAlign="LEFT")
    t.setStyle(TableStyle([
        ("BACKGROUND", (0, 0), (-1, 0), BLACK),
        ("TEXTCOLOR", (0, 0), (-1, 0), WHITE),
        ("BOX", (0, 0), (-1, -1), 1.2, BLACK),
        ("INNERGRID", (0, 0), (-1, -1), 0.6, BLACK),
        ("VALIGN", (0, 0), (-1, -1), "TOP"),
        ("LEFTPADDING", (0, 0), (-1, -1), 5),
        ("RIGHTPADDING", (0, 0), (-1, -1), 5),
        ("TOPPADDING", (0, 0), (-1, -1), 3),
        ("BOTTOMPADDING", (0, 0), (-1, -1), 3),
    ]))
    return t, j

def parse_md(md):
    lines = md.splitlines()
    out = []
    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.strip()
        if not stripped:
            i += 1
            continue
        if re.match(r"^-{3,}\s*$", stripped):
            out.append(Spacer(1, 2))
            i += 1
            continue
        m = re.match(r"^(#{1,3})\s+(.*)$", stripped)
        if m:
            level = len(m.group(1))
            text = m.group(2).strip()
            if level == 1:
                out.append(title_bar(text, font_size=16))
                if i + 1 < len(lines) and lines[i+1].strip().startswith("## "):
                    sub = lines[i+1].strip()[3:].strip()
                    out.append(Spacer(1, 3))
                    out.append(Paragraph(inline(sub), STY["subtitle"]))
                    i += 1
            elif level == 2:
                out.append(Spacer(1, 4))
                out.append(section_bar(text))
                out.append(Spacer(1, 3))
            else:
                out.append(Paragraph(inline(text), STY["h3"]))
            i += 1
            continue
        if stripped.startswith(">"):
            buf = []
            while i < len(lines) and lines[i].strip().startswith(">"):
                buf.append(lines[i].strip()[1:].lstrip())
                i += 1
            paragraphs, cur = [], []
            for b in buf:
                if not b:
                    if cur: paragraphs.append(" ".join(cur)); cur = []
                else:
                    cur.append(b)
            if cur: paragraphs.append(" ".join(cur))
            for p in paragraphs:
                out.append(Paragraph(inline(p), STY["blockquote"]))
            continue
        if stripped.startswith("|") and i+1 < len(lines) and \
                re.match(r"^\s*\|?\s*:?-+", lines[i+1]):
            flow, ni = parse_table(lines, i)
            if flow is not None:
                out.append(flow)
                i = ni
                continue
        if re.match(r"^[-*]\s+", line):
            buf = []
            while i < len(lines) and re.match(r"^[-*]\s+", lines[i]):
                buf.append(re.sub(r"^[-*]\s+", "", lines[i]))
                i += 1
            for item in buf:
                out.append(Paragraph("• " + inline(item), STY["li"]))
            continue
        buf = [line]
        i += 1
        while i < len(lines):
            ns = lines[i].strip()
            if (not ns or ns.startswith("#") or ns.startswith(">")
                    or ns.startswith("|") or re.match(r"^[-*]\s+", ns)
                    or re.match(r"^-{3,}\s*$", ns)):
                break
            buf.append(lines[i])
            i += 1
        out.append(Paragraph(inline(" ".join(b.strip() for b in buf)),
                             STY["body"]))
    return out

def on_page(canvas, doc):
    canvas.saveState()
    canvas.setFont("Helvetica-Oblique", 7)
    canvas.setFillColor(colors.HexColor("#666666"))
    canvas.drawCentredString(LETTER[0] / 2.0, 0.32 * inch,
                             "Battle of the Crossings of Erui  \u2014  Mission Briefing")
    canvas.restoreState()

def main():
    md = MD.read_text(encoding="utf-8")
    flowables = parse_md(md)
    doc = BaseDocTemplate(
        str(OUT), pagesize=LETTER,
        leftMargin=0.6 * inch, rightMargin=0.6 * inch,
        topMargin=0.5 * inch, bottomMargin=0.5 * inch,
        title="Crossings of Erui — Mission Briefing",
        author="Lars Larsen & Chase Reinhart",
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
