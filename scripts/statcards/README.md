# Stat Card Generator

Generates a unit stat card PNG from a JSON description, mimicking the layout
of classic tabletop wargame unit cards.

## Files

- `generate_statcard.py` — image generator script
- `statcard.schema.json` — JSON Schema for input validation
- `samples/cold_one_mount.json` — example input

## Install

```
pip install pillow jsonschema
```

`jsonschema` is optional; if missing, a minimal built-in validator is used.

## Run

```
python generate_statcard.py samples/cold_one_mount.json -o cold_one_mount.png
```

## JSON format

```jsonc
{
  "quantity": 1,                // dynamic: integer >= 0
  "name": "COLD ONE MOUNT",     // dynamic: unit name
  "stats": {                    // all 12 stats required
    "M": 8, "WS": 3, "BS": 0, "S": 4, "T": 4, "W": 2,
    "I": 1, "A": 2, "Ld": 2, "Int": 4, "Cl": 9, "WP": 9
  },
  "weapons":    "Claws (-2 to enemy saves)",  // optional
  "armor":      "None",                       // optional
  "psychology": "Fear, stupidity",            // optional
  "special":    "Slow charge, stinky"         // optional
}
```

The four detail rows (`weapons`, `armor`, `psychology`, `special`) are each
optional and are only rendered when present.
