import time
import Tiger
from Tiger import Keyboard, LCD1

# Initialize display and keyboard
lcd = LCD1()
kbd = Keyboard()


def init_keyboard():
    # NOTE: Replace the placeholder tables below with the byte sequences
    # that describe your keyboard layout.
    default_table = bytes(128)  # TODO: fill with key codes
    shift_table = bytes(128)    # TODO: fill with key codes when Shift is held
    attr_table = bytes(128)     # TODO: fill with key attribute codes

    kbd.setKeyTable(default_table)
    Tiger.delay(10)
    kbd.setKeyShiftTable(shift_table)
    Tiger.delay(10)
    kbd.setKeyAttributeCode(attr_table)
    Tiger.delay(10)

    kbd.keyClickOn()
    kbd.setRepeat(20, 5)
    kbd.setDipSwitch([1] * 16)


init_keyboard()
lcd.clear()
print("Keyboard test - press F1 to exit")

mode = 0

while True:
    # Wait for a key in the buffer
    while kbd.getBufferSize() == 0:
        pass

    key = kbd.read(1)

    if key == b"\xF1":  # F1 toggles the click sound and ends the test
        mode ^= 1
        if mode:
            kbd.keyClickOn()
        else:
            kbd.keyClickOff()
            break
    else:
        # Display the character on the LCD
        lcd.writeString(key)

lcd.clear()
lcd.writeString("End Program")