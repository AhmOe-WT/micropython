"""Simple test script for the TD01 LCD1 display on the Tiger board."""

import Tiger
from Tiger import LCD1

lcd = LCD1(kbd_on=False)
lcd.reset()
lcd.clear()
lcd.cursorOff()
lcd.cursorPosition(0, 0)
lcd.writeString("TD01 LCD1 Test")

lcd.cursorPosition(0, 1)
for i in range(10):
    lcd.writeString("Count: {}".format(i))
    Tiger.delay(1000)
    lcd.cursorPosition(0, 1)
    lcd.writeString("        ")
    lcd.cursorPosition(0, 1)

lcd.clear()
lcd.writeString("Done")