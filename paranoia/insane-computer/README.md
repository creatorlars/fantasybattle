# Friend Computer Terminal

A minimal PARANOIA "insane computer" front-end. You type at your laptop;
each sentence (terminated by `.`, `!`, `?`, `;`, or `:`) is shown centered in
big bold white-on-black on the secondary monitor and spoken through an
offline TTS voice. Bonus: vibrato/FM on the audio and zalgo-style glitching
of the displayed text so it feels like a corrupted subroutine.

## Install

```powershell
cd paranoia\insane-computer
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install -r requirements.txt
```

Everything is offline. TTS uses Windows SAPI5 via `pyttsx3` (sounds
appropriately robotic out of the box).

## Run

```powershell
# Default: fullscreen on your secondary monitor (or only monitor if just one).
python insane_computer.py

# See which monitors were detected.
python insane_computer.py --list-monitors

# Force a specific monitor.
python insane_computer.py --monitor 1

# Test mode -- run in a resizable window on the primary monitor.
python insane_computer.py --windowed

# Tone things down.
python insane_computer.py --no-fm --no-glitch
python insane_computer.py --glitch 0.02      # subtle
python insane_computer.py --glitch 0.25      # very corrupted
python insane_computer.py --rate 140 --font "OCR A Extended"
python insane_computer.py --mute             # visual only
```

## Hotkeys

| Key            | Action                                           |
| -------------- | ------------------------------------------------ |
| `ESC`          | quit                                             |
| `F11`          | toggle fullscreen                                |
| `TAB`          | move window to the next detected monitor         |
| `Backspace`    | delete a character                               |
| `Enter`        | force-commit the current sentence                |
| `Ctrl+U`       | erase the current input line                     |
| `Ctrl+L`       | clear the centered display                       |
| `. ! ? ; :`    | commit current sentence -> display + speak       |

## Notes

- If `numpy`/`soundfile`/`sounddevice` aren't installed, audio still works
  via plain `pyttsx3.say()` -- just no vibrato or stutter glitches.
- If `screeninfo` isn't installed, monitor selection is disabled and the
  window opens wherever the OS puts it; use `--windowed` for testing.
- The TTS voice can be changed system-wide in Windows
  *Settings > Time & Language > Speech*. Picking a different SAPI voice
  changes the Computer's character.
