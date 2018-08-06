# ESP8266 LED Goggle Squad
Code for building a squad of LED goggles that automatically synchronize their animations when they're near each other.  Total cost is around $45 when you order parts in bulk.

# Parts List
* Microcontroller: [MakerFocus Wemos D1 Mini ESP8266 with integrated BMS](https://smile.amazon.com/gp/product/B075H8X7H2/)
  * The brains, wireless, battery holder and charger, all in one!
* Battery: [Panasonic NCR18650B](https://www.imrbatteries.com/panasonic-ncr18650b-18650-3350mah-protected-button-top-battery/)
  * Protected cell is important, because I don't trust the MakerFocus low voltage protection
  * These are taller than most 18650's, but they *barely* fit with a little force
* LED drivers: [Adafruit NeoPixel Ring - 16 x 5050](https://www.adafruit.com/product/1463)
* [Goggles](https://www.banggood.com/Welding-Cutting-Welders-Industrial-Safety-Goggles-Steampunk-Cup-Goggles-p-1135671.html?cur_warehouse=CN): cheap Chinesium, lots of them available on Amazon too
* [Diffraction Grating sheet](https://smile.amazon.com/gp/product/B007FZT3Y2/)
* [Waterproof coating](https://smile.amazon.com/gp/product/B008O9YIV6/)
* [Push buttons](https://smile.amazon.com/gp/product/B01E38OS7K/)
* [E6000 Craft Adhesive](https://smile.amazon.com/gp/product/B007TSYNG8/)

# Getting started with Software
1. Install the [CH340G driver (for D1 mini)](/drivers/CH340G)
2. Install [Arduino IDE](https://www.arduino.cc/en/Main/Software) (follow [these instructions to update the board manager URL in options](https://github.com/wemos/Arduino_D1))
3. In the Arduino IDE, I set Tools -> Board to `WeMos D1 R2 & mini`, and left the rest of the settings as default
4. Follow [these instructions](https://learn.adafruit.com/adafruit-all-about-arduino-libraries-install-use) to install the Arduino libraries from this repository, which are [stored here](/libraries)
5. Plug the board into your computer via micro USB
6. Turn the device on, and press the [flash button](wemos-d1-mini.png) (this seems to be optional), then you can upload your sketch!
7. I recommend starting with the [blink](/blink/blink.ino) project.  Try changing the delays and confirm that the LED blinks like you expect it to!  Congrats, you have the basics of the software sorted out!

# Hardware assembly
1. Put a battery into the holder, turn it on and confirm you see a light blinking on the PCB.  Test the battery charger too!
2. [Learn to solder](https://learn.adafruit.com/adafruit-guide-excellent-soldering) / solder practice!
3. `<insert everything here>`
4. Conformal coat everything, check under UV light
5. Hot glue or epoxy everything you want to stay in place!
6. Remember to close up the metal shield with hot glue / silicone conformal coating
7. Electric tape the battery in place?
8. Sew or otherwise attach the battery/microcontroller to the back of the goggles

# Software Part 2
1. Now, to test our hard work!  Load up the [lone-wolf-test-button](/lone-wolf-test-button/lone-wolf-test-button.ino) project!  This project is a simplified version of my [old goggles project](https://github.com/rorosaurus/neopixel-goggles), which automatically cycles through 8 different animations.  It also uses the push button to change the mode, so we can test our soldering for the LED drivers and push button at the same time!
2. Do the LEDs animate?  Does the button change the modes?  Great!  If not, revisit the assembly instructions above.  Try using a digital multimeter to check the continuity between all your different connections.
3. The actual wireless connection part is still a work-in-progress.  Coming soon :)
4. anyway, here's wonderwall

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
* [Wemos D1 GitHub](https://github.com/wemos/Arduino_D1)
* [Wemos D1 Wiki Page (Pins, etc.)](https://wiki.wemos.cc/products:d1:d1_mini)
* [Wemos D1 Schematic](https://wiki.wemos.cc/_media/products:d1:sch_d1_mini_v3.0.0.pdf)
* [ESP8266 Arduino GitHub](https://github.com/esp8266/Arduino)
* [ESP8266 Arduino Documentation](https://arduino-esp8266.readthedocs.io/en/latest/)
