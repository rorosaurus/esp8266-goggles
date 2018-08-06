# ESP8266 LED Goggle Squad
Code for building a squad of LED goggles that automatically synchronize their animations when they're near each other

# Parts List
* Microcontroller: [MakerFocus Wemos D1 Mini ESP8266 with integrated BMS](https://smile.amazon.com/gp/product/B075H8X7H2/)
  * The brains, wireless, battery holder and charger, all in one!
* Battery: [Panasonic NCR18650B](https://www.imrbatteries.com/panasonic-ncr18650b-18650-3350mah-protected-button-top-battery/)
  * Protected cell is important, because I don't trust the MakerFocus low voltage protection
  * These are taller than most 18650's, but they *barely* fit with a little force
* LED drivers: [Adafruit NeoPixel Ring - 16 x 5050](https://www.adafruit.com/product/1463)

# Getting started with Software
1. Install the [CH340G driver (for D1 mini)](https://github.com/rorosaurus/esp8266-goggles/tree/master/drivers/CH340G)
2. Install [Arduino IDE](https://www.arduino.cc/en/Main/Software) (follow [these instructions to update the board manager URL in options](https://github.com/wemos/Arduino_D1))
3. In the Arduino IDE, I set Tools -> Board to `WeMos D1 R2 & mini`, and left the rest of the settings as default
4. Follow [these instructions](https://learn.adafruit.com/adafruit-all-about-arduino-libraries-install-use) to install the Arduino libraries from this repository, which are [stored here](https://github.com/rorosaurus/esp8266-goggles/tree/master/libraries)
5. Plug the board into your computer via micro USB
6. Turn the device on, and press the [flash button](wemos-d1-mini.png) (this seems to be optional), then you can upload your sketch!
7. I recommends starting with the `blink` project.  Try changing the delays and confirm that the LED blinks like you expect it to!

# Hardware assembly


# Software part 2: *coming soon to a repo near you*

# General Use Tips
* Always double-check the polarity of the battery before inserting, or you'll fry it!
* When the LEDs stop animating and freeze, turn it off and recharge the battery!
  * This means the battery voltage is too low to run the microcontroller.  If it gets lower, that's fine - it will automatically cut off - but it's better (for the battery longevity) to not discharge so low.
* When charging, please keep it in the shade and make sure it isn't stuffed in a backpack or closed space.
  * Certain components on the PCB get **hot** during charging, so be careful!
* Charging maxes out at 1A, which would take ~3.4 hours to charge from worst case empty.
  * You need a micro USB cable to charge it.
  * If you unplug the charger when it's 90% done, it won't let you resume charging until it discharges more.  Try to leave it plugged in and charging (red LED indicator) until it's entirely done (green LED indicator).
  * Try not to leave it plugged in for excessive (many hours) lengths of time if it's done charging.
  * You can run it and charge it at the same time (USB battery pack)
    * Just be careful since the charging circuit on the PCB might get hot!
* Please don't just leave it sitting in direct sun.  The battery and electronics will thank you :)

# Links
* https://github.com/wemos/Arduino_D1
* https://wiki.wemos.cc/products:d1:d1_mini
* https://wiki.wemos.cc/_media/products:d1:sch_d1_mini_v3.0.0.pdf
* https://github.com/esp8266/Arduino
* https://arduino-esp8266.readthedocs.io/en/latest/
