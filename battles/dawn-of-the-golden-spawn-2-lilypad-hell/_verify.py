"""Verify the EDITED .ai file: walk every text-show, identify the font in effect,
and warn about any chars not in the original font's char set (per page audit).
"""
import pikepdf, re, sys

ORIG = 'dawn-of-the-golden-spawn-2-lilypad-hell.original.ai'
EDIT = 'dawn-of-the-golden-spawn-2-lilypad-hell.ai'


def font_ranges(raw: bytes):
    out = []
    cur = None; start = 0
    for m in re.finditer(rb'/(T[T1]_?\d+) [\d. ]+Tf', raw):
        if cur is not None:
            out.append((cur, start, m.start()))
        cur = m.group(1).decode(); start = m.end()
    if cur is not None:
        out.append((cur, start, len(raw)))
    return out


def chars_in_literals(seg: bytes) -> set[int]:
    chars = set(); i = 0
    while i < len(seg):
        c = seg[i]
        if c == 0x28:
            j = i + 1; depth = 1; lit = bytearray()
            while j < len(seg) and depth > 0:
                cj = seg[j]
                if cj == 0x5C and j+1 < len(seg):
                    n = seg[j+1]
                    if chr(n).isdigit():
                        k = j+1; octs = ''
                        while k < len(seg) and k - j <= 3 and chr(seg[k]).isdigit():
                            octs += chr(seg[k]); k += 1
                        lit.append(int(octs, 8) & 0xFF)
                        j = k; continue
                    else:
                        lit.append(n); j += 2; continue
                if cj == 0x28:
                    depth += 1; lit.append(cj); j += 1; continue
                if cj == 0x29:
                    depth -= 1
                    if depth == 0: break
                    lit.append(cj); j += 1; continue
                lit.append(cj); j += 1
            for b in lit:
                chars.add(b)
            i = j + 1
        else:
            i += 1
    return chars


def font_at_offset(raw: bytes, off: int):
    cur = None
    for m in re.finditer(rb'/(T[T1]_?\d+) [\d. ]+Tf', raw):
        if m.start() > off:
            break
        cur = m.group(1).decode()
    return cur


def all_paren_literals_with_offsets(raw: bytes):
    """Yield (start, end, decoded_str, raw_paren_body_bytes)."""
    i = 0
    while i < len(raw):
        c = raw[i]
        if c == 0x28:
            j = i + 1; depth = 1; lit = bytearray()
            while j < len(raw) and depth > 0:
                cj = raw[j]
                if cj == 0x5C and j+1 < len(raw):
                    n = raw[j+1]
                    if chr(n).isdigit():
                        k = j+1; octs = ''
                        while k < len(raw) and k - j <= 3 and chr(raw[k]).isdigit():
                            octs += chr(raw[k]); k += 1
                        lit.append(int(octs, 8) & 0xFF)
                        j = k; continue
                    else:
                        lit.append(n); j += 2; continue
                if cj == 0x28:
                    depth += 1; lit.append(cj); j += 1; continue
                if cj == 0x29:
                    depth -= 1
                    if depth == 0: break
                    lit.append(cj); j += 1; continue
                lit.append(cj); j += 1
            yield (i, j+1, bytes(lit).decode('cp1252', errors='replace'), bytes(lit))
            i = j + 1
        else:
            i += 1


def main():
    orig = pikepdf.open(ORIG)
    edit = pikepdf.open(EDIT)
    # Build per-page per-font allowed char sets from original
    for pi in range(6):
        oraw = bytes(orig.pages[pi].Contents.read_bytes())
        eraw = bytes(edit.pages[pi].Contents.read_bytes())
        by_font = {}
        for fname, s, e in font_ranges(oraw):
            by_font.setdefault(fname, set()).update(chars_in_literals(oraw[s:e]))
        # walk edited literals, check
        problems = []
        for s, e, text, body in all_paren_literals_with_offsets(eraw):
            font = font_at_offset(eraw, s)
            if font is None: continue
            allowed = by_font.get(font, set())
            for b in body:
                if b not in allowed:
                    problems.append((font, text, hex(b)))
                    break
        print(f'=== PAGE {pi+1} ===')
        if not problems:
            print('  OK')
        else:
            seen = set()
            for font, text, badhex in problems:
                key = (font, text)
                if key in seen: continue
                seen.add(key)
                print(f'  [{font}] bad char {badhex} in: {text!r}')


if __name__ == '__main__':
    main()
