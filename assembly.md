# Parts List
Total cost is around $45 when you order parts in bulk.
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
1. Install [Arduino IDE](https://www.arduino.cc/en/Main/Software) (follow [these instructions to update the board manager URL in options](https://github.com/esp8266/Arduino#installing-with-boards-manager)) using version 2.4.2
1. In the Arduino IDE, I set Tools -> Board to `WeMos D1 R1`, set the Programmer to `USBtinyISP`, and left the rest of the settings as default.
1. Follow [these instructions](https://learn.adafruit.com/adafruit-all-about-arduino-libraries-install-use) to install the Arduino libraries from this repository, which are [stored here](/libraries)
1. Plug the board into your computer via micro USB
1. Turn the device on, and press the [flash button](wemos-d1-mini.png) (this seems to be optional), then you can upload your sketch!  If you run into issues uploading, you should restart your PC or try using other COM ports (under Tools).
1. I recommend starting with the [blink](/blink/blink.ino) project.  Try changing the delays and confirm that the LED blinks like you expect it to!  Congrats, you have the basics of the software sorted out!

# Hardware assembly
1. [Learn to solder](https://learn.adafruit.com/adafruit-guide-excellent-soldering) / solder practice!
1. Take a long look at this high quality, very polished [wiring diagram](wiring-diagram.png) to see how the wires are expected to hook up.
1. Take your Goggles and drill a small hole in the front right where you'd like your push button to be placed.  Place the push button on the inside of the goggles, only the yellow nub should stick out.  Confirm your hole is wide enough to fit the plastic top on.  Glue the push button in place on the inside.  You can bend or snip the standoffs you won't use below.
1. Use a multimeter in "continuity" mode to test your push button and figure out which leads are connected, and which ones will bridge when you click the button.  Make note of which side you want to tie to `GND` and which side you'll solder to `D2`.
1. Orient your LED drivers, so you know which way is "up".  Use the Adafruit star icon on the PCB to roughly find the "top".  Make sure both your LED drivers are mounted facing the same way when we put them in the goggles, so the animations make sense.
1. Solder 4 long wires directly to the GPIO on your ESP8266 PCB (`D1, D2, 5V, GND`).  These wires should approach from the battery side, since we want the populated PCB side to attach flat against the goggle strap.  If you are apprehensive about splicing the `GND` wire later to supply two different `GND` connections, you can run one additional wire to the `GND` pin at this time.  Take another look at my [wiring diagram](wiring-diagram.png)!
1. Physically place the LED driver and ESP8266 on the goggles and try to evaluate the right length of wire you'll need.  Keep in mind, the goggle strap will strech when you put on the goggles - how will your wires accomodate this streching?  Do you want to wrap them around the straps in loops, like a spring?  Will they remain fixed, but you'll mount the ESP8266 loose enough that it can flex?  Decide now.
1. Cut the wires to the length you think you'll need.  Always better to be a little longer than you think you need.  The wires should attach similar to [this tutorial on Adafruit](https://learn.adafruit.com/kaleidoscope-eyes-neopixel-led-goggles-trinket-gemma/wiring-soldering) - though I would recommend having the wires come directly into the back of the LEDs if you're comfortable trying.
1. Decide how you want your wires to wrap around the goggle strap toward the right eye.  Do you want to loop them around the goggle strap, so it can handle some stretching?  You can slide some 1/2 inch Expando over the looped wire/strap to protect this and stretch a bit.
1. Feed your wires through the side holes of the right goggle.
1. Solder the wires to their destinations, according to the [wiring diagram](wiring-diagram.png).  For the `GND` wire, you can strip the wire a bit in the  middle, wrap it around the push button pin, then continue the `GND` wire to its destination on the LED driver.
1. Measure new lengths of wire to attach the right LED driver to the left LED driver.
1. Solder these wires as well, feeding them inbetween the right and left goggles.  You can unscrew the goggle lenses and feed the wires between the nose bridge (after clipping some nubs out of the way).

# Software Part 2: Confirm everything works!
1. Now, to test our hard work!  Load up the [test-button-and-leds](/test-button-and-leds/test-button-and-leds.ino) project!  This project is a simplified version of my [old goggles project](https://github.com/rorosaurus/neopixel-goggles), which automatically cycles through 8 different animations.  It also uses the push button to change the mode, so we can test our soldering for the LED drivers and push button at the same time!
1. Do the LEDs animate?  Does the button change the modes?  Great!  If not, revisit the assembly instructions above.  Try using a digital multimeter to check the continuity between all your different connections.
1. The actual wireless connection part is still a work-in-progress.  Coming soon :)
1. anyway, here's wonderwall

# Hardware Part 2: Cleaning things up and hardening for future abuse
1. Trim exposed wires leftover from soldering.
1. Conformal coat all PCB surfaces and exposed wires. Check your coverage by using UV light!  This will mostly waterproof the electronics, so you can worry random splashes/light rain less.
1. Hot glue or epoxy everything you want to stay firmly in place!  Especially wire ends, where tension might form when goggles are streched over your head.  Hot glue is less permanent, if you think you might need to change things in the future.
1. Close up the ESP8266's metal shield with hot glue / silicone conformal coating.  We can't get under it, but we need to ensure moisture doesn't trap in there.
1. Do you want to prevent dust from entering your goggles through the sides?  Seal those holes!
1. Electric tape the battery in place, or perhaps sew it against the battery holder with some string.
1. Sew or otherwise attach the battery/microcontroller to the back of the goggle strap.  Do you want to wrap it in eletrical tape to make it cleaner?
1. Add some cushion to the sides of where your nose will rub against it.
1. Remove any tinted lenses, and glue or otherwise affix the LEDs and glass lenses from shifting/shaking around (dangerous when dancing)!
1. [Laser cut](laser-cut-diffraction-sheet.vsdx) the diffraction grating sheet to the proper diameter of the goggle lenses.  Find a way to attach them to the lenses, maybe sandwich them between the LEDs and lense?  Or we can glue them to the lense.
