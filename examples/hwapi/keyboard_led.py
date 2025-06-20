import sys
from hwconfig import LED

CAPS_ON = False
shift = False
LED.off()
print("Type letters. 'c' toggles caps lock, 's' is shift for next letter, 'q' quits.")

while True:
    ch = sys.stdin.read(1)
    if not ch:
        continue
    if ch.lower() == 'q':
        break
    if ch.lower() == 'c':
        CAPS_ON = not CAPS_ON
        if CAPS_ON:
            LED.on()
        else:
            LED.off()
        continue
    if ch.lower() == 's':
        shift = True
        continue

    if shift:
        shift = False
        if CAPS_ON:
            ch = ch.lower()
        else:
            ch = ch.upper()
    else:
        if CAPS_ON:
            ch = ch.upper()
        else:
            ch = ch.lower()
    sys.stdout.write(ch)
    sys.stdout.flush()
