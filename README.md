# ESP8266 LED Goggle Squad
Code for building a squad of LED goggles that automatically synchronize their animations when they're near each other

![Wemos D1 Mini](wemos-d1-mini.png)

# Parts List
* Microcontroller: [MakerFocus Wemos D1 Mini ESP8266 with integrated BMS](https://smile.amazon.com/gp/product/B075H8X7H2/)
  * The brains, wireless, battery holder and charger, all in one!
* Battery: [Panasonic NCR18650B](https://www.imrbatteries.com/panasonic-ncr18650b-18650-3350mah-protected-button-top-battery/)
  * Protected cell is important, because I don't trust the MakerFocus low voltage protection
  * These are taller than most 18650's, but they *barely* fit with a little force
* LED drivers: [Adafruit NeoPixel Ring - 16 x 5050](https://www.adafruit.com/product/1463)

# Install drivers
1. Install the [CH340G driver for D1 mini](https://wiki.wemos.cc/tutorials:get_started:get_started_in_arduino#get_started_in_arduino)
2. Install [Arduino IDE](https://www.arduino.cc/en/Main/Software), follow [these instructions to update the board manager](https://github.com/wemos/Arduino_D1)
3. In the Arduino IDE, I used board `WeMos D1 R2 & mini`, and left the settings default
4. Plug in USB to the board via micro USB
5. Turn the device on, and press the flash button (this seems to be optional), then you can upload your sketch

# Links
* https://github.com/wemos/Arduino_D1
* https://wiki.wemos.cc/products:d1:d1_mini
* https://wiki.wemos.cc/_media/products:d1:sch_d1_mini_v3.0.0.pdf
* https://github.com/esp8266/Arduino
* https://arduino-esp8266.readthedocs.io/en/latest/
