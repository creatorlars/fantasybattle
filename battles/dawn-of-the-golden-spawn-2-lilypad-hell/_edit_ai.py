"""Edit dawn-of-the-golden-spawn-2-lilypad-hell.ai in place.

Replaces the original (Mesoamerican-WFB-3e) bio / objective / relationship text
on each player-reference page with the rethemed (Rogue-Trader / Imperial sci-fi)
text from player-references-rethemed.md. Headers, titles, layout, and graphics
are untouched.

Strategy:
  - Single-line text-shows (objectives + relationship rows) are replaced by
    locating the exact byte sequence of the original `(...) Tj` or `[...] TJ`
    inside the page content stream and substituting a new `(...) Tj`.
  - Multi-line bios are replaced as a span: from the byte offset of the first
    bio text-show to the offset just past the last one, including all
    intervening `0 -1.2 Td` line-breaks. Substituted with N TJ lines (one Tj
    per wrapped line of new text) joined by `0 -1.2 Td`, where N is forced to
    match the original line count so the layout doesn't shift.
"""
from __future__ import annotations
import re
import shutil
import textwrap
from pathlib import Path

import pikepdf

HERE = Path(__file__).parent
SRC = HERE / "dawn-of-the-golden-spawn-2-lilypad-hell.ai"
BAK = HERE / "dawn-of-the-golden-spawn-2-lilypad-hell.original.ai"
OUT = SRC  # overwrite in place

# ---------- helpers ----------
def encode_pdf_string(s: str) -> bytes:
    """Encode a python str as a PDF literal string body (without parens).

    Uses cp1252 so smart quotes (U+2018-U+201D) and U+2019 round-trip to the
    bytes (0x91-0x94, 0x92) the font expects under WinAnsiEncoding.
    """
    try:
        encoded = s.encode("cp1252")
    except UnicodeEncodeError:
        # fall back to latin-1 for any chars cp1252 can't handle
        encoded = s.encode("latin-1", errors="replace")
    out = bytearray()
    for b in encoded:
        if b in (0x28, 0x29, 0x5C):  # ( ) \
            out.append(0x5C); out.append(b)
        elif b < 0x20 or b == 0x7F:
            out.append(0x5C)
            for d in oct(b)[2:].rjust(3, "0"):
                out.append(ord(d))
        else:
            out.append(b)
    return bytes(out)

def make_tj(text: str) -> bytes:
    return b"(" + encode_pdf_string(text) + b")Tj"

def find_unique(haystack: bytes, needle: bytes) -> int:
    """Return offset of unique occurrence of `needle` in `haystack`, else raise."""
    first = haystack.find(needle)
    if first < 0:
        raise KeyError(f"needle not found: {needle[:60]!r}...")
    second = haystack.find(needle, first + 1)
    if second >= 0:
        raise KeyError(f"needle not unique (found at {first} and {second}): {needle[:60]!r}...")
    return first

# Walk a content stream and yield (start_offset, end_offset, rendered_text)
# for every text-show (Tj literal or TJ array).
ANY_SHOW = re.compile(
    rb"(\[(?:[^\[\]]|\\.)*\]\s*TJ)|(\((?:[^()\\]|\\.)*\)\s*Tj)", re.S)
TJ_STR_INSIDE = re.compile(rb"\(((?:[^()\\]|\\.)*)\)")

def decode_pdf_string(s: bytes) -> str:
    out = bytearray(); i = 0
    while i < len(s):
        c = s[i]
        if c == 0x5C and i + 1 < len(s):
            n = s[i+1]
            if n in b"()\\":
                out.append(n); i += 2
            elif n == ord("n"): out.append(0x0A); i += 2
            elif n == ord("r"): out.append(0x0D); i += 2
            elif n == ord("t"): out.append(0x09); i += 2
            elif chr(n).isdigit():
                j = i + 1; octal = ""
                while j < len(s) and j - i <= 3 and chr(s[j]).isdigit():
                    octal += chr(s[j]); j += 1
                out.append(int(octal, 8) & 0xFF); i = j
            else:
                out.append(n); i += 2
        else:
            out.append(c); i += 1
    # Decode as Windows-1252 so 0x91-0x94 map to smart quotes (U+2018-U+201D)
    # and 0x92 to U+2019 (right single quote / apostrophe). This matches what
    # Illustrator emits when text was typed with smart quotes on.
    return bytes(out).decode("cp1252", errors="replace")

def show_text(body: bytes) -> str:
    if body.rstrip().endswith(b"Tj"):
        m = re.match(rb"\((.*)\)\s*Tj\s*$", body, re.S)
        return decode_pdf_string(m.group(1))
    m = re.match(rb"\[(.*)\]\s*TJ\s*$", body, re.S)
    return "".join(decode_pdf_string(s.group(1))
                   for s in TJ_STR_INSIDE.finditer(m.group(1)))

def all_text_shows(raw: bytes):
    """Yield (start, end, rendered_text) for every text-show op."""
    for m in ANY_SHOW.finditer(raw):
        body = m.group(0)
        yield m.start(), m.end(), show_text(body)

def find_show_by_text(raw: bytes, target: str, start_offset: int = 0):
    """Return (start, end) of the first text-show whose rendered text == target."""
    for s, e, t in all_text_shows(raw):
        if s < start_offset:
            continue
        if t == target:
            return s, e
    raise KeyError(f"no text-show matches {target!r} (after offset {start_offset})")

def find_show_by_prefix(raw: bytes, prefix: str, start_offset: int = 0):
    for s, e, t in all_text_shows(raw):
        if s < start_offset:
            continue
        if t.startswith(prefix):
            return s, e, t
    raise KeyError(f"no text-show starts with {prefix!r} (after offset {start_offset})")

# ---------- per-page edits ----------
# For BIO entries we wrap the new text to (about) the same line width as the
# original and force the same line count. Original line widths sit around
# 47-52 chars depending on font/Tm.

PAGE_EDITS: dict[int, dict] = {
    # ----- PAGE 1: KOZMOX -----
    # T1_1 italic safe caps: A B C G J K M P R S; lowercase a-i,k-p,r-u,w,y,z
    #   (no j q v x); no hyphen; smart quote only \u201d (\x94)
    # TT4 (relationships) safe caps: A F P T; no hyphen; no comma; no semicolon
    0: {
        "bio": {
            "first_text": "Kozmo has held the rank of Pond Master in Gold ",
            "last_text":  "palace, Cuiteca (\u201dGold Palace.\u201d) ",
            "new_text": (
                "Kozmo has held the rank of Pad Master aboard "
                "Cuiteca for a decade, like his father. Refusing "
                "the augmented Bull Slann bodyguards owed his "
                "station, he marches with cousins and clutchmates "
                "from his own spawning year, all of whom hold a "
                "sinecure in the court of his ancestral orbital, "
                "Cuiteca (\u201dGold Pad\u201d)."
            ),
            "lines": 7,
            "wrap_width": 50,
            "td_step": "0 -1.2 Td",
        },
        "lines": [
            ("An imbecile without appreciation for fineries.",
             "A tin toad blind to all finery."),
            ("This circus of pink skins gave my court much laughter.",
             "A pink skin show kept my court laughing for cycles."),
            ("Perhaps this wizard\u2019s power can be used to my ends.",
             "Perhaps this old psyker\u2019s gifts can serve my ends."),
            ("These devolved swampies are a disgrace to the empire.",
             "Tribal bog priests disgrace the empire."),
            ("Celebrate the healthy hatching of all eggs.",
             "Celebrate the hatching of all clutches."),
        ],
    },
    # ----- PAGE 2: TEPITRONIX -----
    # T1_1 italic safe caps: B E S T; lowercase nearly full (no j); has \x93 \x94
    # TT4 safe caps: A C H T V; no semicolon; no j q x y z lowercase
    1: {
        "bio": {
            "first_text": "Tepitepetl \u201cTepi\u201d and his Troglodyte companion ",
            "last_text":  "pinky.",
            "new_text": (
                "Tepitronix \u201cTepi\u201d and his Troglodyte companion "
                "Biglips have toured for many cycles, hauling "
                "their carnival of lobotomized humans they call "
                "\u201cpinkies.\u201d Tepi has perfected the surgery that "
                "turns feral humans into compliant servitors. "
                "Biglips lives only for his next meal of a "
                "disobedient pinky."
            ),
            "lines": 8,
            "wrap_width": 50,
            "td_step": "0 -1.2 Td",
        },
        "lines": [
            ("The gold flows when we perform for the court at Cuiteca.",
             "The coin flows when we perform for the court at Cuiteca."),
            ("He has a warrant out for the arrest of our entire circus.",
             "He has writs out for the arrest of our entire carnival."),
            ("An important customer, although a little unnerving.",
             "An important client, deals in dangerous secrets."),
            ("An advocate for uncollared pinkies, must be avoided.",
             "An advocate for uncollared pinkies, best avoided."),
            ("Valuable guides who show us where the pinkies hide.",
             "Helpful guides who show us where wild pinkies hide."),
            ("Sign new performance contracts and evade the law.",
             "Secure new performance contracts and evade the law."),
        ],
    },
    # ----- PAGE 3: KROOKOMET -----
    # T1_0 italic page 3 safe caps: A B C E H L M P S W; lowercase a-z minus b j q x
    # TT4 safe caps: A E S T W; has comma, period, hyphen, apostrophe (\x92)
    2: {
        "bio": {
            "first_text": "Lt. Croakimoto is a recently promoted soldier who ",
            "last_text":  "day of ascension to the rank of Alligator Warrior.",
            "new_text": (
                "He is a recently promoted officer who treats "
                "his oath of service to the Star Lord Mazdamundi "
                "with utmost sincerity. He deploys only with Bull "
                "Slann decanted on the same day, from the same "
                "pool. He despises aristocratic preening and is "
                "highly devout, dreaming one day of ascension to "
                "the rank of Alligator Warrior."
            ),
            "lines": 7,
            "wrap_width": 50,
            "td_step": "0 -1.2 Td",
        },
        "lines": [
            ("Evidence of the empire\u2019s crumbling aristocracy.",
             "Evidence of the throne\u2019s crumbling aristocracy."),
            ("A wanted felon who is dangerous to the empire.",
             "A wanted felon dangerous to the empire."),
            ("Another self-important diplomat or politician, probably. ",
             "Another self-important envoy, no doubt."),
            ("Traitors to the Emperor, but warriors deserving respect.",
             "Traitors to the throne, yet warriors deserving respect."),
        ],
    },
    # ----- PAGE 4: XOCHITON -----
    # T1_0 italic page 4 safe caps: D G H J L O P S W X; lowercase a-y minus j v x z
    # TT4 page 4 safe caps: A P S T; lowercase missing j q x z
    3: {
        "bio": {
            "first_text": "Xochi the Star Seeker has wandered far across ",
            "last_text":  "is accompanied by Pimbu Ono, his apprentice.",
            "new_text": (
                "Xochiton the Star Seeker has wandered the lost "
                "paths in his quest to decipher the wisdom of the "
                "Lost DataScrolls. He is hundreds of years old, "
                "and wields psyker power beyond the reckoning of "
                "most. He is here to witness whether the Dawn of "
                "the Golden Spawn dawns this time, accompanied by "
                "his apprentice Pimblar Ono."
            ),
            "lines": 7,
            "wrap_width": 50,
            "td_step": "0 -1.2 Td",
        },
        "lines": [
            ("A fine pinky monger who supplied my litter bearers.",
             "A fine pinky monger who supplied my litter bearers."),
            ("A pawn in the game of stars, a servant who must obey.",
             "A pawn on the star board, a servant who must obey."),
            ("Their important roles as guardians are now at an end. ",
             "Their long watch nears its prophesied end, again."),
            ("Peasant illusionists with no knowledge of the scrolls.",
             "Tribal shamans, ignorant of the true scrolls."),
        ],
    },
    # ----- PAGE 5: NINJORBIT -----
    # T1_0 italic page 5 safe caps: E N P R S T V; lowercase missing q v z
    # TT4 page 5 safe caps: A C I O S; lowercase missing j q x
    4: {
        "bio": {
            "first_text": "Ninjatl is the most recent leader in a line of chiefs ",
            "last_text":  "other from the past few hundred years.",
            "new_text": (
                "Ninjorbit is the latest leader in a line of "
                "sentinels extending to the beginning of time, "
                "all of whom guarded the sacred underground "
                "spawning pools of the Slann nobility. Refusing "
                "the throne, his glyph painted tribes claim sole "
                "ownership of the pools. To Ninjorbit, this "
                "hatching is like any other from the past few "
                "hundred years."
            ),
            "lines": 8,
            "wrap_width": 50,
            "td_step": "0 -1.2 Td",
        },
        "lines": [
            ("An impressive Spawnfather who fertilized all of his eggs.",
             "An impressive Spawnfather who fertilized all his eggs."),
            ("A defiler who dares to bring pink skins to a sacred place.",
             "A defiler who dares bring pinkies to a sacred place."),
            ("Imperialist scum, unnecessary and intrusive.",
             "Imperial scum, unnecessary and intrusive."),
        ],
    },
    # ----- PAGE 6: ASTRO PRIESTS -----
    # T1_0 italic page 6 safe caps: A B G K S T; lowercase missing q x z
    # TT9 page 6 safe caps: I only; lowercase missing b g j q u v x z; no comma
    5: {
        "bio": {
            "first_text": "Kukulbufo & Bubulkufo are from the deep jungle, ",
            "last_text":  "would be the caretakers of brood of Golden Slann.",
            "new_text": (
                "Kukulbufon & Bubulkufan are from the deep frond "
                "bogs, shaman leaders of small tribes hidden from "
                "the great Slann throne. Signs unseen for ages "
                "convinced them to make this trek, which they "
                "believe to be the answer of a great prophecy "
                "told throughout time, that they would be the "
                "caretakers of the brood of Golden Slann."
            ),
            "lines": 7,
            "wrap_width": 50,
            "td_step": "0 -1.2 Td",
        },
        "lines": [
            ("A hilarious entertainer, who brings trinkets to the villages.",
             "A hilarious showman who brings star trinkets to our villages."),
            ("Imperials rank with the stench of fatty wormfood.",
             "Imperials primed for sacrifice."),
        ],
    },
}

def wrap_to_lines(text: str, n_lines: int, target_width: int) -> list[str]:
    """Wrap text to exactly n_lines, adjusting width as needed."""
    lines = textwrap.wrap(text, width=target_width)
    # widen if too many lines
    w = target_width
    while len(lines) > n_lines and w < 90:
        w += 1
        lines = textwrap.wrap(text, width=w)
    # narrow if too few
    w = target_width
    while len(lines) < n_lines and w > 20:
        w -= 1
        lines = textwrap.wrap(text, width=w)
    # pad with empty lines if still short (won't render anything)
    while len(lines) < n_lines:
        lines.append("")
    # truncate if still too long (last line gets the remainder squeezed)
    if len(lines) > n_lines:
        head = lines[:n_lines - 1]
        tail = " ".join(lines[n_lines - 1:])
        lines = head + [tail]
    return lines

def build_bio_block(new_text: str, n_lines: int, target_width: int,
                    td_step: str) -> bytes:
    lines = wrap_to_lines(new_text, n_lines, target_width)
    parts = []
    for idx, line in enumerate(lines):
        if idx > 0:
            parts.append(td_step.encode("latin-1"))
            parts.append(b"\n")
        parts.append(make_tj(line))
        parts.append(b"\n")
    # strip trailing newline
    return b"".join(parts).rstrip(b"\n")

def edit_page(raw: bytes, page_idx: int) -> bytes:
    edits = PAGE_EDITS.get(page_idx)
    if not edits:
        return raw
    # ---- 1) bio block: replace span from first matching show to last ----
    bio = edits["bio"]
    f_start, _, f_text = find_show_by_prefix(raw, bio["first_text"])
    # find last text-show whose rendered text == bio["last_text"], scanning
    # from f_start onward, taking the LAST match before any
    # non-bio content (we just take all matches and keep the closest-after-first)
    last_match = None
    for s, e, t in all_text_shows(raw):
        if s < f_start:
            continue
        if t == bio["last_text"]:
            last_match = (s, e)
            break  # the first occurrence after f_start is the bio's last line
    if last_match is None:
        raise KeyError(f"page {page_idx+1}: bio last_text {bio['last_text']!r} not found after offset {f_start}")
    span_end = last_match[1]
    new_block = build_bio_block(
        bio["new_text"], bio["lines"], bio["wrap_width"], bio["td_step"])
    raw = raw[:f_start] + new_block + raw[span_end:]
    # ---- 2) per-line replacements (each is a single text-show) ----
    for old_text, new_text in edits["lines"]:
        try:
            s, e = find_show_by_text(raw, old_text)
        except KeyError as ex:
            raise KeyError(f"page {page_idx+1}: line replacement failed: {ex}")
        new_bytes = make_tj(new_text)
        raw = raw[:s] + new_bytes + raw[e:]
    return raw

def main():
    if not BAK.exists():
        shutil.copy2(SRC, BAK)
        print(f"backup written: {BAK.name}")
    pdf = pikepdf.open(str(SRC), allow_overwriting_input=True)
    for pi, page in enumerate(pdf.pages):
        stream_obj = page.Contents
        raw = bytes(stream_obj.read_bytes())
        new_raw = edit_page(raw, pi)
        if new_raw != raw:
            stream_obj.write(new_raw)
            print(f"  page {pi+1}: edited "
                  f"({len(raw):,} -> {len(new_raw):,} bytes)")
        else:
            print(f"  page {pi+1}: unchanged")
    pdf.save(str(OUT))
    print(f"saved: {OUT.name} ({OUT.stat().st_size:,} bytes)")

if __name__ == "__main__":
    main()
