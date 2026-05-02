"""
Extract player handouts from hanksync_convention_packet_v3.md and render to a
print-ready PDF for convention play.

Handouts in order:
  1. Title page
  2. Six persona briefings (Sec 4)        -- one per page
  3. Lyric sheet: All Hail The Computer   (Sec 5.1, after the print-cut marker)
  4. Lyric sheet: Hanks Of The Heart      (Sec 5.2, after the print-cut marker)
  5. Twelve society envelope notes        (Sec 6.2)  -- two per page, with cut line

Run:
    python generate_handouts_pdf.py
Output:
    hanksync_player_handouts.pdf
"""

from __future__ import annotations

import re
from pathlib import Path

from reportlab.lib.pagesizes import LETTER
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import inch
from reportlab.lib.enums import TA_CENTER, TA_LEFT
from reportlab.lib import colors
from reportlab.platypus import (
    BaseDocTemplate, Frame, PageTemplate, Paragraph, Spacer,
    PageBreak, KeepTogether, FrameBreak, HRFlowable,
)
from reportlab.pdfgen import canvas


HERE = Path(__file__).parent
SRC = HERE / "hanksync_convention_packet_v3.md"
OUT = HERE / "hanksync_player_handouts.pdf"

HANKSYNC_RED = colors.HexColor("#B0001A")
INK = colors.HexColor("#1a1a1a")
GREY = colors.HexColor("#666666")


# ---------------------------------------------------------------------------
# Markdown extraction
# ---------------------------------------------------------------------------

def md_to_inline_html(text: str) -> str:
    """Convert the limited markdown inline syntax we use to reportlab HTML."""
    # Escape ampersands first, then re-introduce entities we want to keep.
    text = text.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
    # Bold-italic ***x***
    text = re.sub(r"\*\*\*(.+?)\*\*\*", r"<b><i>\1</i></b>", text, flags=re.S)
    # Bold **x**
    text = re.sub(r"\*\*(.+?)\*\*", r"<b>\1</b>", text, flags=re.S)
    # Italic *x*
    text = re.sub(r"(?<!\*)\*([^*\n]+?)\*", r"<i>\1</i>", text)
    # Inline code `x`
    text = re.sub(r"`([^`]+?)`", r"<font face='Courier'>\1</font>", text)
    return text


def extract_section(md: str, header_re: str, end_re: str) -> str:
    """Slice md from the start-of-line header match to the next end_re match."""
    m_start = re.search(header_re, md, flags=re.M)
    if not m_start:
        raise SystemExit(f"Section start not found: {header_re!r}")
    rest = md[m_start.end():]
    m_end = re.search(end_re, rest, flags=re.M)
    body = rest if not m_end else rest[: m_end.start()]
    return body.strip("\n")


def split_personas(sec4_body: str) -> list[tuple[str, str]]:
    """Section 4 -> [(title, body), ...] for HANK-1..HANK-6."""
    # Split on the persona ### headers.
    parts = re.split(r"(?m)^### (HANK-\d \u00b7 .*)$", sec4_body)
    out: list[tuple[str, str]] = []
    # parts[0] is the GM-note preamble + "PLAYER HANDOUTS BEGIN" cut, discard.
    for i in range(1, len(parts), 2):
        title = parts[i].strip()
        body = parts[i + 1].strip("\n")
        # Trim trailing horizontal-rule separators.
        body = re.sub(r"\n+---\s*$", "", body)
        out.append((title, body))
    return out


def split_society_notes(sec62_body: str) -> list[tuple[str, str]]:
    """Section 6.2 -> [(society_title, note_text), ...]"""
    parts = re.split(r"(?m)^#### (.+? \u2014 for one Hank)$", sec62_body)
    out: list[tuple[str, str]] = []
    for i in range(1, len(parts), 2):
        title = parts[i].strip()
        body = parts[i + 1].strip("\n")
        body = re.sub(r"\n+---\s*$", "", body)
        out.append((title, body))
    return out


def extract_lyric_sheet(song_body: str) -> str:
    """A song body in Sec 5 -- keep only what comes after the LYRIC SHEET cut."""
    cut = re.search(r"LYRIC SHEET \(PRINT\) BEGINS BELOW.*?\u2702", song_body, flags=re.S)
    if cut:
        song_body = song_body[cut.end():]
    return song_body.strip("\n")


# ---------------------------------------------------------------------------
# Markdown -> Platypus flowables (very small subset)
# ---------------------------------------------------------------------------

class HandoutStyles:
    def __init__(self):
        s = getSampleStyleSheet()
        self.title = ParagraphStyle(
            "Title", parent=s["Title"], fontName="Helvetica-Bold",
            fontSize=22, leading=26, textColor=HANKSYNC_RED,
            alignment=TA_CENTER, spaceAfter=4,
        )
        self.subtitle = ParagraphStyle(
            "Sub", parent=s["Normal"], fontName="Helvetica-Oblique",
            fontSize=10, leading=12, textColor=GREY,
            alignment=TA_CENTER, spaceAfter=14,
        )
        self.classif = ParagraphStyle(
            "Classif", parent=s["Normal"], fontName="Helvetica-Bold",
            fontSize=9, leading=11, textColor=HANKSYNC_RED,
            alignment=TA_CENTER, spaceAfter=2, borderPadding=4,
        )
        self.persona_h = ParagraphStyle(
            "PersonaH", parent=s["Heading1"], fontName="Helvetica-Bold",
            fontSize=18, leading=22, textColor=HANKSYNC_RED,
            alignment=TA_CENTER, spaceBefore=8, spaceAfter=10,
        )
        self.body = ParagraphStyle(
            "Body", parent=s["BodyText"], fontName="Helvetica",
            fontSize=10.5, leading=14, textColor=INK,
            alignment=TA_LEFT, spaceAfter=6,
        )
        self.bullet = ParagraphStyle(
            "Bullet", parent=self.body, leftIndent=14, bulletIndent=2,
            fontSize=10.5, leading=13.5, spaceAfter=2,
        )
        self.quote = ParagraphStyle(
            "Quote", parent=self.body, fontName="Helvetica-Oblique",
            fontSize=10.5, leading=14, textColor=INK,
            leftIndent=18, rightIndent=18, spaceBefore=4, spaceAfter=8,
        )
        self.lyric = ParagraphStyle(
            "Lyric", parent=self.body, fontName="Helvetica",
            fontSize=10, leading=13, leftIndent=14, rightIndent=8,
            spaceAfter=4,
        )
        self.lyric_h = ParagraphStyle(
            "LyricH", parent=self.body, fontName="Helvetica-Bold",
            fontSize=11, leading=14, textColor=HANKSYNC_RED,
            spaceBefore=8, spaceAfter=2,
        )
        self.stage = ParagraphStyle(
            "Stage", parent=self.body, fontName="Helvetica-Oblique",
            fontSize=9, leading=11.5, textColor=GREY,
            leftIndent=14, rightIndent=8, spaceAfter=4,
        )
        self.envelope_h = ParagraphStyle(
            "EnvH", parent=s["Heading2"], fontName="Helvetica-Bold",
            fontSize=14, leading=18, textColor=HANKSYNC_RED,
            alignment=TA_LEFT, spaceAfter=6,
        )
        self.envelope_body = ParagraphStyle(
            "EnvBody", parent=self.body, fontName="Helvetica",
            fontSize=10.5, leading=14, alignment=TA_LEFT,
            leftIndent=4, rightIndent=4, spaceAfter=4,
        )
        self.envelope_sig = ParagraphStyle(
            "EnvSig", parent=self.envelope_body, fontName="Helvetica-Oblique",
            alignment=TA_LEFT, textColor=GREY, spaceBefore=4,
        )


def flow_persona_briefing(title: str, body: str, st: HandoutStyles) -> list:
    """Render one HANK-X briefing as flowables."""
    flow: list = []
    flow.append(Paragraph(title.replace("\u00b7", "&middot;"), st.persona_h))
    flow.append(Spacer(1, 4))

    # The briefing body has the header block (>...) then prose, lists, > quote.
    lines = body.split("\n")
    i = 0
    buf: list[str] = []

    def flush_para():
        nonlocal buf
        if buf:
            txt = " ".join(s.strip() for s in buf if s.strip())
            if txt:
                flow.append(Paragraph(md_to_inline_html(txt), st.body))
            buf = []

    while i < len(lines):
        ln = lines[i]
        s = ln.rstrip()
        if not s.strip():
            flush_para()
            i += 1
            continue
        if s.lstrip().startswith("> "):
            flush_para()
            # Collect block-quote (classification or final benediction).
            qbuf: list[str] = []
            while i < len(lines) and lines[i].lstrip().startswith(">"):
                qln = lines[i].lstrip()[1:].strip()
                qbuf.append(qln)
                i += 1
            qtext = "<br/>".join(md_to_inline_html(x) for x in qbuf if x)
            # Heuristic: if it contains "CLASSIFICATION" treat as classification block.
            if "CLASSIFICATION" in " ".join(qbuf):
                flow.append(Paragraph(qtext, st.classif))
                flow.append(Spacer(1, 6))
            else:
                flow.append(Paragraph(qtext, st.quote))
            continue
        if s.lstrip().startswith("- "):
            flush_para()
            item = s.lstrip()[2:].strip()
            flow.append(Paragraph(md_to_inline_html(item), st.bullet, bulletText="\u2022"))
            i += 1
            continue
        buf.append(s)
        i += 1
    flush_para()
    return flow


def flow_lyric_sheet(song_title: str, song_subtitle: str, lyric_body: str,
                     st: HandoutStyles) -> list:
    flow: list = [
        Paragraph(song_title, st.persona_h),
        Paragraph(song_subtitle, st.subtitle),
    ]
    lines = lyric_body.split("\n")
    i = 0
    para_buf: list[str] = []

    def flush_para():
        nonlocal para_buf
        if para_buf:
            txt = "<br/>".join(md_to_inline_html(x) for x in para_buf)
            if txt.strip():
                flow.append(Paragraph(txt, st.lyric))
            para_buf = []

    while i < len(lines):
        ln = lines[i].rstrip()
        if not ln.strip():
            flush_para()
            i += 1
            continue
        # Bold "header" lines like **VERSE 1:** or **CHORUS:**
        if ln.startswith("**") and ln.endswith("**") and ":" in ln:
            flush_para()
            flow.append(Paragraph(md_to_inline_html(ln), st.lyric_h))
            i += 1
            continue
        # Bold header followed by stage direction on the same line.
        m = re.match(r"\*\*(.+?)\*\*\s*(.*)$", ln)
        if m and m.group(2).startswith("*(") and not para_buf:
            flush_para()
            flow.append(Paragraph(md_to_inline_html("**" + m.group(1) + "**"),
                                  st.lyric_h))
            stage = m.group(2)
            flow.append(Paragraph(md_to_inline_html(stage), st.stage))
            i += 1
            continue
        # Stage direction line entirely in *(...)*
        if ln.lstrip().startswith("*(") and ln.rstrip().endswith(")*"):
            flush_para()
            flow.append(Paragraph(md_to_inline_html(ln.strip()), st.stage))
            i += 1
            continue
        # Block quote line
        if ln.lstrip().startswith("> "):
            content = ln.lstrip()[2:]
            # Skip GM-only callouts that are technically inside the print
            # zone but tagged "do not print on lyric sheet".
            if ("GM staging cue" in content
                    or "GM CUES" in content
                    or "do not print" in content.lower()):
                # Swallow the rest of this blockquote.
                while i < len(lines) and lines[i].lstrip().startswith(">"):
                    i += 1
                continue
            para_buf.append(content)
            i += 1
            continue
        if ln.lstrip().startswith(">"):
            i += 1
            continue
        # Section break / horizontal rule
        if re.match(r"^-{3,}\s*$", ln):
            flush_para()
            flow.append(Spacer(1, 6))
            i += 1
            continue
        para_buf.append(ln)
        i += 1
    flush_para()
    return flow


def make_communist_variant(lyric_song1: str) -> str:
    """Return the Song 1 lyric body with one Verse 2 line swapped for the
    Communist propaganda line. The swap is silent on the page \u2014 only the GM
    knows it is here."""
    target = ("Some citizens, they *question,* and the *questioners* "
              "get *queued,*")
    replacement = ("The means belong to those who *sing them,* "
                   "and the *song* is what is *true,*")
    if target not in lyric_song1:
        # Fallback: prepend a marker so we notice in proof.
        return ("> [VARIANT BUILD WARN: target line not found]\n\n"
                + lyric_song1)
    return lyric_song1.replace(target, replacement, 1)


def flow_form_cs4_hsyd(st: HandoutStyles) -> list:
    """Render Form CS/4-HSY-D (one form per page; print 6 copies).

    Deliberately bureaucratic and slightly off. The form itself is a bit. The
    chain-sword acknowledgement clauses match the briefing in Sec 3, Act 1
    Beat 2 (Requisitions).
    """
    from reportlab.platypus import Table as _Table, TableStyle as _TableStyle
    flow: list = []

    # Header band.
    form_title = ParagraphStyle(
        "FormTitle", parent=st.body, fontName="Helvetica-Bold",
        fontSize=14, leading=17, textColor=HANKSYNC_RED,
        alignment=TA_LEFT, spaceAfter=2,
    )
    form_meta = ParagraphStyle(
        "FormMeta", parent=st.body, fontName="Helvetica",
        fontSize=8.5, leading=10.5, textColor=GREY,
        alignment=TA_LEFT, spaceAfter=2,
    )
    form_clause = ParagraphStyle(
        "FormClause", parent=st.body, fontName="Helvetica",
        fontSize=10, leading=13, alignment=TA_LEFT, spaceAfter=2,
        leftIndent=18, bulletIndent=2,
    )
    form_inst = ParagraphStyle(
        "FormInst", parent=st.body, fontName="Helvetica-Oblique",
        fontSize=8.5, leading=11, textColor=GREY,
        alignment=TA_LEFT, spaceAfter=4,
    )

    flow.append(Paragraph(
        "FORM CS/4-HSY-D \u2014 CHAIN-SWORD REQUISITION ACKNOWLEDGEMENT",
        form_title))
    flow.append(Paragraph(
        "HPD&amp;MC \u00b7 R&amp;D / Tech Services counter \u00b7 "
        "rev. 14-C \u00b7 file under: PERFORMANCE-MORALE-INSTRUMENT, "
        "PROP/WEAPON, BOTH",
        form_meta))
    flow.append(HRFlowable(
        width="100%", thickness=0.6, color=HANKSYNC_RED,
        spaceBefore=4, spaceAfter=8))

    flow.append(Paragraph(
        "<b>INSTRUCTIONS:</b> Complete in <b>triplicate</b>, "
        "in <b>pencil</b>, in <b>cursive</b>. Do not press too hard. "
        "Do not press too softly. Pressing correctly is treason if performed "
        "incorrectly. The Tech at the counter will know.",
        form_inst))

    # Identification block: a small table of fill-in lines.
    label_p = ParagraphStyle(
        "Lbl", parent=st.body, fontName="Helvetica-Bold",
        fontSize=9.5, leading=12, alignment=TA_LEFT,
    )
    line_p = ParagraphStyle(
        "Line", parent=st.body, fontName="Helvetica",
        fontSize=10, leading=14, alignment=TA_LEFT, textColor=INK,
    )

    def field(label: str, blanks: int = 1, width: str = "wide"):
        ws = "\u00a0" * (40 if width == "wide" else 20)
        return [Paragraph(label, label_p),
                Paragraph(("&nbsp;" * 2) + ("_" * (60 if width == "wide" else 24)),
                          line_p)]

    rows = [
        field("DESIGNATION (HANK-N):"),
        field("PRIOR PLC POSTING:"),
        field("CHAIN-SWORD SERIAL # (Mark IV-_):"),
        field("ASSIGNED PERSONA (LEADER / BAD BOY / SWEET / "
              "INTENSE / WEIRD / REPLACEMENT \u2014 circle one):"),
    ]

    t = _Table(rows, colWidths=[2.4 * inch, 4.4 * inch],
               hAlign="LEFT")
    t.setStyle(_TableStyle([
        ("VALIGN", (0, 0), (-1, -1), "MIDDLE"),
        ("LEFTPADDING", (0, 0), (-1, -1), 0),
        ("RIGHTPADDING", (0, 0), (-1, -1), 0),
        ("TOPPADDING", (0, 0), (-1, -1), 6),
        ("BOTTOMPADDING", (0, 0), (-1, -1), 6),
    ]))
    flow.append(t)
    flow.append(Spacer(1, 6))

    flow.append(Paragraph(
        "<b>I, the undersigned citizen, acknowledge that:</b>", st.body))

    clauses = [
        ("My HANKSYNC-pattern Morale-Enhancement Combat Instrument "
         "(\"chain sword\") is a <b>performance prop only</b>."),
        ("My HANKSYNC-pattern Morale-Enhancement Combat Instrument "
         "is <b>fully treason-rated</b> for symbolic combat against "
         "unspecified enemies of morale."),
        ("My chain sword's pull-cord must be pulled on every chorus, "
         "every solo, and at <b>emotionally appropriate moments</b> "
         "as determined by Friend Computer in real time."),
        ("My chain sword <b>is not</b> a weapon."),
        ("My chain sword <b>is</b> a weapon."),
        ("Both of the above clauses are simultaneously in effect. "
         "I will not seek clarification."),
        ("Approximately <b>one in six</b> chain swords in the current "
         "issue lot exhibits a \"minor manufacturing variance\" and is, "
         "in fact, real. I have not been informed which. I "
         "<b>will not ask.</b>"),
        ("Loss, damage, or non-revving of my chain sword is a "
         "<b>Form-37/B-deductible</b> offense against my clone allowance. "
         "Penalty applies retroactively to all six (6) of my issued clones, "
         "including those not yet decanted."),
        ("Failure to sign this form is treason. Signing this form is "
         "treason if performed in the wrong cursive. Refusing to read "
         "this clause is treason. Reading it correctly was also treason. "
         "I have read it."),
    ]
    for i, c in enumerate(clauses, start=1):
        flow.append(Paragraph(f"{i}. {c}", form_clause))

    flow.append(Spacer(1, 10))
    flow.append(HRFlowable(width="100%", thickness=0.4, color=GREY,
                           spaceBefore=2, spaceAfter=8))

    sig_label = ParagraphStyle(
        "SigLbl", parent=st.body, fontName="Helvetica-Bold",
        fontSize=9.5, leading=12, alignment=TA_LEFT, textColor=INK,
    )
    sig_line = ParagraphStyle(
        "SigLine", parent=st.body, fontName="Helvetica",
        fontSize=10, leading=14, alignment=TA_LEFT,
    )
    sig_rows = [
        [Paragraph("SIGNATURE (cursive):", sig_label),
         Paragraph("&nbsp;" * 2 + "_" * 50, sig_line)],
        [Paragraph("DATE-CYCLE:", sig_label),
         Paragraph("&nbsp;" * 2 + "_" * 24, sig_line)],
        [Paragraph("WITNESS (Tech Services):", sig_label),
         Paragraph("&nbsp;" * 2 + "_" * 50, sig_line)],
        [Paragraph("COUNTER-WITNESS (Internal Security):", sig_label),
         Paragraph("&nbsp;" * 2 + "_" * 44, sig_line)],
    ]
    st_t = _Table(sig_rows, colWidths=[2.6 * inch, 4.2 * inch], hAlign="LEFT")
    st_t.setStyle(_TableStyle([
        ("VALIGN", (0, 0), (-1, -1), "MIDDLE"),
        ("LEFTPADDING", (0, 0), (-1, -1), 0),
        ("RIGHTPADDING", (0, 0), (-1, -1), 0),
        ("TOPPADDING", (0, 0), (-1, -1), 6),
        ("BOTTOMPADDING", (0, 0), (-1, -1), 6),
    ]))
    flow.append(st_t)

    flow.append(Spacer(1, 10))
    flow.append(Paragraph(
        "<b>FOR OFFICE USE ONLY:</b> &nbsp; Copy 1 \u2192 PLC archive "
        "(filed). &nbsp; Copy 2 \u2192 HPD&amp;MC archive (lost). "
        "&nbsp; Copy 3 \u2192 Internal Security (always). "
        "&nbsp; Copy 4 \u2192 there is no copy 4. If you have a copy 4, "
        "please report yourself.",
        form_inst))

    return flow


def flow_envelope_note(title: str, body: str, st: HandoutStyles) -> list:
    """Render a society envelope note as a single block (used twice per page)."""
    flow: list = []
    flow.append(Paragraph(title.replace("\u2014 for one Hank", "").strip(),
                          st.envelope_h))
    # Body is a single block-quote paragraph + a sig line, sometimes multi-line.
    qlines: list[str] = []
    sig: str | None = None
    for ln in body.split("\n"):
        s = ln.lstrip()
        if not s:
            continue
        if s.startswith("> "):
            content = s[2:].rstrip()
            if content.lstrip().startswith("*\u2014") or content.startswith("*\u2014"):
                sig = content
            else:
                qlines.append(content)
        elif s.startswith(">"):
            continue
        else:
            qlines.append(s.rstrip())
    text = "<br/>".join(md_to_inline_html(x) for x in qlines if x.strip())
    flow.append(Paragraph(text, st.envelope_body))
    if sig:
        flow.append(Paragraph(md_to_inline_html(sig), st.envelope_sig))
    return flow


# ---------------------------------------------------------------------------
# Page templates / canvas decorations
# ---------------------------------------------------------------------------

def draw_page_chrome(canv: canvas.Canvas, doc, label: str | None = None):
    canv.saveState()
    # Top bar
    canv.setFillColor(HANKSYNC_RED)
    canv.rect(0, LETTER[1] - 18, LETTER[0], 18, fill=1, stroke=0)
    canv.setFillColor(colors.white)
    canv.setFont("Helvetica-Bold", 9)
    canv.drawString(0.5 * inch, LETTER[1] - 13, "HANKSYNC \u2014 PLAYER HANDOUT")
    canv.drawRightString(LETTER[0] - 0.5 * inch, LETTER[1] - 13,
                          "FRIEND COMPUTER \u2022 HPD&MC")
    # Footer
    canv.setFillColor(GREY)
    canv.setFont("Helvetica-Oblique", 8)
    canv.drawCentredString(LETTER[0] / 2, 0.4 * inch,
                            "TRUST NO ONE \u2022 KEEP YOUR LASER HANDY \u2022 STAY ALERT \u2022 THE COMPUTER IS YOUR FRIEND")
    if label:
        canv.setFont("Helvetica", 8)
        canv.drawString(0.5 * inch, 0.4 * inch, label)
    canv.restoreState()


def draw_envelope_page(canv: canvas.Canvas, doc):
    draw_page_chrome(canv, doc, label="Cut along the dashed line. Fold. Seal.")
    # Dashed cut line between the two frames.
    canv.saveState()
    canv.setDash(6, 4)
    canv.setStrokeColor(GREY)
    canv.setLineWidth(0.6)
    y = LETTER[1] / 2
    canv.line(0.4 * inch, y, LETTER[0] - 0.4 * inch, y)
    # Scissors glyph at left margin
    canv.setFont("Helvetica", 10)
    canv.setFillColor(GREY)
    canv.drawString(0.25 * inch, y - 3, "\u2702")
    canv.restoreState()


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def build():
    md = SRC.read_text(encoding="utf-8")

    # --- Extract Section 4 (persona briefings) ----------------------------
    sec4 = extract_section(
        md,
        r"^## 4 \u00b7 HANK PERSONA BRIEFINGS",
        r"^## 5 \u00b7 SONGS",
    )
    personas = split_personas(sec4)
    if len(personas) != 6:
        raise SystemExit(f"Expected 6 personas, got {len(personas)}")

    # --- Extract Section 5.1 lyric sheet ---------------------------------
    sec51 = extract_section(
        md,
        r"^### 5\.1 \u2014 \"ALL HAIL THE COMPUTER\"",
        r"^### 5\.2 \u2014 \"HANKS OF THE HEART\"",
    )
    lyric_song1 = extract_lyric_sheet(sec51)

    # --- Extract Section 5.2 lyric sheet ---------------------------------
    sec52 = extract_section(
        md,
        r"^### 5\.2 \u2014 \"HANKS OF THE HEART\"",
        r"^### 5\.3 \u2014",
    )
    lyric_song2 = extract_lyric_sheet(sec52)

    # --- Extract Section 6.2 envelope notes ------------------------------
    sec62 = extract_section(
        md,
        r"^### 6\.2 \u2014 Player Handout Notes",
        r"^## 7 \u00b7 NPC ROSTER",
    )
    notes = split_society_notes(sec62)
    if len(notes) != 12:
        raise SystemExit(f"Expected 12 society notes, got {len(notes)}")

    # ---------------------------------------------------------------------
    # Build the document. Use multiple page templates: standard single-frame
    # for briefings and lyrics; split-frame for the envelope notes.
    # ---------------------------------------------------------------------
    st = HandoutStyles()

    margin = 0.6 * inch
    top_margin = 0.5 * inch + 18  # below the red bar
    bot_margin = 0.6 * inch
    frame_w = LETTER[0] - 2 * margin
    frame_h = LETTER[1] - top_margin - bot_margin

    standard_frame = Frame(
        margin, bot_margin, frame_w, frame_h,
        leftPadding=0, rightPadding=0, topPadding=4, bottomPadding=4,
        showBoundary=0,
    )
    half_h = (LETTER[1] - top_margin - bot_margin - 0.4 * inch) / 2
    upper_frame = Frame(
        margin, LETTER[1] / 2 + 0.05 * inch, frame_w, half_h,
        leftPadding=0, rightPadding=0, topPadding=4, bottomPadding=4,
        showBoundary=0,
    )
    lower_frame = Frame(
        margin, bot_margin, frame_w, half_h,
        leftPadding=0, rightPadding=0, topPadding=10, bottomPadding=4,
        showBoundary=0,
    )

    templates = [
        PageTemplate(id="standard", frames=[standard_frame],
                      onPage=draw_page_chrome),
        PageTemplate(id="envelope", frames=[upper_frame, lower_frame],
                      onPage=draw_envelope_page),
    ]

    doc = BaseDocTemplate(
        str(OUT), pagesize=LETTER,
        leftMargin=margin, rightMargin=margin,
        topMargin=top_margin, bottomMargin=bot_margin,
        title="HANKSYNC \u2014 Player Handouts",
        author="HPD&MC Performance Optimization Subroutine",
        subject="Print one set per table.",
    )
    doc.addPageTemplates(templates)

    flow: list = []

    # --- Title page ------------------------------------------------------
    flow.append(Spacer(1, 1.4 * inch))
    flow.append(Paragraph("HANKSYNC", st.title))
    flow.append(Paragraph("Player Handouts", st.subtitle))
    flow.append(Spacer(1, 0.4 * inch))
    flow.append(Paragraph(
        "Print this packet once per table. Cut along the marked lines where "
        "indicated. Distribute the persona briefings and society envelope "
        "notes privately to each player. Print the lyric sheets in multiples "
        "of six (one per Hank, one for the Music Director food-vat).",
        st.body))
    flow.append(Spacer(1, 0.3 * inch))
    flow.append(Paragraph(
        "<b>Contents:</b><br/>"
        "&middot; Six persona briefings (one per Hank)<br/>"
        "&middot; Lyric sheet \u2014 \u201cAll Hail The Computer\u201d (group number)<br/>"
        "&middot; Lyric sheet \u2014 \u201cAll Hail The Computer\u201d \u2014 <b>Communist variant</b> (GM swap prop)<br/>"
        "&middot; Lyric sheet \u2014 \u201cHanks Of The Heart\u201d (solo suite)<br/>"
        "&middot; Form CS/4-HSY-D \u2014 chain-sword requisition (print 6 copies)<br/>"
        "&middot; Twelve society envelope notes (assign one per Hank; "
        "remaining six are unused this run)",
        st.body))
    flow.append(Spacer(1, 0.5 * inch))
    flow.append(Paragraph(
        "<i>The Computer is your friend. Have a nice daycycle.</i>",
        st.subtitle))
    flow.append(PageBreak())

    # --- Persona briefings ----------------------------------------------
    for title, body in personas:
        flow.extend(flow_persona_briefing(title, body, st))
        flow.append(PageBreak())

    # --- Lyric sheets ----------------------------------------------------
    flow.extend(flow_lyric_sheet(
        "ALL HAIL THE COMPUTER",
        "HANKSYNC \u00b7 Group Number \u00b7 Lyric Sheet",
        lyric_song1, st))
    flow.append(PageBreak())

    # --- Communist-variant Song 1 sheet (GM swap prop) -------------------
    # GM uses this to silently replace one Hank's sheet mid-Verse 2 of the
    # group number per Sec 5.1 sabotage hook. Header is identical to the
    # clean sheet; the swap is a single line.
    flow.extend(flow_lyric_sheet(
        "ALL HAIL THE COMPUTER",
        "HANKSYNC \u00b7 Group Number \u00b7 Lyric Sheet",
        make_communist_variant(lyric_song1), st))
    flow.append(PageBreak())

    flow.extend(flow_lyric_sheet(
        "HANKS OF THE HEART",
        "HANKSYNC \u00b7 Solo Suite \u00b7 Lyric Sheet",
        lyric_song2, st))
    flow.append(PageBreak())

    # --- Form CS/4-HSY-D (chain-sword requisition; print 6 copies) ------
    flow.extend(flow_form_cs4_hsyd(st))

    # --- Envelope notes (switch to two-up template) ---------------------
    from reportlab.platypus.doctemplate import NextPageTemplate
    flow.append(NextPageTemplate("envelope"))
    flow.append(PageBreak())

    for idx, (title, body) in enumerate(notes):
        flow.extend(flow_envelope_note(title, body, st))
        if idx % 2 == 0:
            flow.append(FrameBreak())
        else:
            flow.append(PageBreak())

    doc.build(flow)
    print(f"Wrote: {OUT}")


if __name__ == "__main__":
    build()
