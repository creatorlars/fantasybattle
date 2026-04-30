"""
Extract unit stat cards from the 'Crossings of Erui' Army Statblocks workbook
and emit one JSON file per card matching scripts/statcards/statcard.schema.json.

Spreadsheet layout (Sheet1):
  - Eldacar's army: cols F..S
        F=variant/unit-name, G..R = M WS BS S T W I A Ld Int Cl WP, S=SV
  - Castamir's army: cols V..AI  (same column pattern, shifted)
  - Each unit block has:
      * a unit-name row (single text cell on F or V)
      * a stat-header row (M WS BS ... SV)
      * one or more stat rows whose first column holds the armor variant
        (e.g. "Light", "Light + Shield", "Heavy + Shield", "Warhorse")
      * 0+ trailing rule-text rows (free text spanning F or V)

Mounted units have two stat rows (rider then mount). We emit one card per
stat row; mount rows are named "<Unit> Mount".

Quantities and weapons for Eldacar's foot/hero units come from cols A/B,
and for Castamir's units from cols C/D, where they are listed as e.g.
"18 Spearmen" and "Spears (86)". We pair them up positionally.
"""

from __future__ import annotations

import json
import re
from pathlib import Path

import openpyxl

WORKBOOK = Path("battles/crossings-of-erui/army-statblocks-spreadsheet.xlsx")
OUT_DIR  = Path("battles/crossings-of-erui/statcards")

STAT_KEYS = ["M", "WS", "BS", "S", "T", "W", "I", "A", "Ld", "Int", "Cl", "WP"]

# Column letters for each army's 14-column block: name + 12 stats + SV
ARMIES = {
    "eldacar":  {"name_col": "F", "stat_cols": list("GHIJKLMNOPQR"), "sv_col": "S"},
    "castamir": {"name_col": "V", "stat_cols": ["W", "X", "Y", "Z", "AA", "AB", "AC", "AD", "AE", "AF", "AG", "AH"], "sv_col": "AI"},
}

# ---- Manually authored unit blocks (derived from a deep read of the sheet) -

# Each entry: (unit_name_row, list of stat_rows, list of rule_rows, quantity, weapons_text)
# Stat rows: row numbers whose F/V column contains the armour-variant label and
# whose stat columns contain numeric values.
# Rule rows: trailing free-text rows in column F/V belonging to this unit.

ELDACAR_UNITS = [
    # name,                      qty,  weapons,                      name_row, stat_rows, rule_rows
    ("Rohan Spearmen",            18,  "Spears (8\")",                   16,  [17],        [18, 19, 20]),
    ("Rohan Archers",             18,  "Longbows (30\")",                23,  [24],        [25]),
    ("Rohan Swordsmen",           15,  "Hand Weapons",                   27,  [28],        [29]),
    ("Prince Vidustain",           1,  "Hand Weapon",                    32,  [33, 34],    [35, 36]),
    ("Rohirrim Spearmen (Mounted)", 3, "Spears (8\")",                  38,  [39, 40],    [41, 42, 43, 44]),
    ("Rohirrim Swordsmen (Mounted)", 9, "Hand Weapons",                  46,  [47, 48],    [49, 50]),
    ("Eldacar",                    1,  "Hand Weapon",                    52,  [53],        [54]),
    ("Honor Guard",                6,  "Hand Weapons",                   56,  [57],        [58]),
    ("Aldamir, son of Eldacar",    1,  "Hand Weapon",                    61,  [62],        [63]),
    ("Vidurafin",                  1,  "Hand Weapon",                    65,  [66],        [67]),
]

CASTAMIR_UNITS = [
    ("Gondor Spearmen",           21,  "Spears (8\")",                   16,  [17],        [18, 19, 20]),
    ("Gondor Archers",            18,  "Longbows (30\")",                23,  [24],        [25]),
    ("Corsair Spearmen",          18,  "Spears (8\")",                   27,  [28],        [29, 30, 31]),
    ("Corsair Archers",           15,  "Bows (24\")",                    33,  [34],        [35]),
    ("Castamir",                   1,  "Hand Weapon",                    37,  [38],        [39]),
    ("Captain Aurandir",           1,  "Hand Weapon",                    41,  [42],        [43]),
    ("Brute!",                     1,  "Hand Weapon",                    45,  [46],        [47]),
]


# Heuristic classifier for rule text -> psychology / special / weapons-extra
PSYCHOLOGY_KEYWORDS = ("fear", "stupidity", "terror", "frenzy", "panic", "hate", "hatred")


def classify_rules(rule_texts: list[str]) -> tuple[list[str], list[str]]:
    """Split free-form rule text into (psychology, special) buckets."""
    psy, spc = [], []
    for t in rule_texts:
        low = t.lower()
        if any(k in low for k in PSYCHOLOGY_KEYWORDS):
            psy.append(t)
        else:
            spc.append(t)
    return psy, spc


def cell(ws, col: str, row: int):
    return ws[f"{col}{row}"].value


def extract_stat_row(ws, army_key: str, row: int) -> tuple[str, dict, str]:
    """Return (variant_label, {stat: value}, sv_text) for one stat row."""
    cfg = ARMIES[army_key]
    variant = cell(ws, cfg["name_col"], row)
    stats: dict = {}
    for key, col in zip(STAT_KEYS, cfg["stat_cols"]):
        v = cell(ws, col, row)
        if v is None:
            v = "-"
        elif isinstance(v, float) and v.is_integer():
            v = int(v)
        stats[key] = v
    sv = cell(ws, cfg["sv_col"], row)
    if isinstance(sv, float) and sv.is_integer():
        sv = int(sv)
    return (str(variant) if variant is not None else ""), stats, ("" if sv is None else str(sv))


def slugify(s: str) -> str:
    s = s.lower()
    s = re.sub(r"[^a-z0-9]+", "_", s).strip("_")
    return s or "unit"


def build_card(name: str, qty, variant: str, stats: dict, sv: str,
               weapons: str | None, psychology: list[str], special: list[str]) -> dict:
    armor = variant
    if sv:
        armor = f"{variant} (Save {sv})" if variant else f"Save {sv}"
    card = {
        "quantity": qty,
        "name": name.upper(),
        "stats": stats,
    }
    if weapons:
        card["weapons"] = weapons
    if armor:
        card["armor"] = armor
    if psychology:
        card["psychology"] = "; ".join(psychology)
    if special:
        # join but keep readable
        card["special"] = " ".join(special)
    return card


def process_unit(ws, army_key: str, name, qty, weapons, name_row, stat_rows, rule_rows, out_dir: Path) -> list[Path]:
    cfg = ARMIES[army_key]
    rule_texts = [str(cell(ws, cfg["name_col"], r)) for r in rule_rows
                  if cell(ws, cfg["name_col"], r) is not None]
    psy, spc = classify_rules(rule_texts)

    written: list[Path] = []
    is_mounted = len(stat_rows) > 1

    for idx, row in enumerate(stat_rows):
        variant, stats, sv = extract_stat_row(ws, army_key, row)

        if is_mounted and idx == 1:
            # Mount row -> derived card
            card_name = f"{name} Mount"
            card_qty  = qty
            card = build_card(card_name, card_qty, variant, stats, sv,
                              weapons=None, psychology=[], special=[])
            slug = slugify(card_name)
        else:
            card = build_card(name, qty, variant, stats, sv,
                              weapons=weapons, psychology=psy, special=spc)
            slug = slugify(name)

        path = out_dir / f"{army_key}_{slug}.json"
        path.write_text(json.dumps(card, indent=2), encoding="utf-8")
        written.append(path)
    return written


def main() -> None:
    wb = openpyxl.load_workbook(WORKBOOK, data_only=True)
    ws = wb.active

    OUT_DIR.mkdir(parents=True, exist_ok=True)

    all_paths: list[Path] = []
    for army_key, units in (("eldacar", ELDACAR_UNITS), ("castamir", CASTAMIR_UNITS)):
        for (name, qty, weapons, name_row, stat_rows, rule_rows) in units:
            paths = process_unit(ws, army_key, name, qty, weapons,
                                 name_row, stat_rows, rule_rows, OUT_DIR)
            all_paths.extend(paths)
            for p in paths:
                print(f"wrote {p}")

    print(f"\n{len(all_paths)} card JSON file(s) written to {OUT_DIR}")


if __name__ == "__main__":
    main()
