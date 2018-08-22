To edit sketches to work for backpack and other accessories,

````
#define DATA_PIN    15 // pin 15 maps to "D8" labelled on our board
#define LED_TYPE    WS2812B
#define NUM_LEDS    64 // 40 leds back + (12x2) on the front sides
````

You can also use something like below to force the backpack to be a slave, not a master

````
#define nodeID "000000"
````

Make sure the nodeID is unique for each backpack/accessory.  Just increment it in hex.


You might want to make your single long strip into smaller, sub-strips.  I'd recommend looking at [this gist](https://gist.github.com/andre-geldenhuis/58435a7816f8b4575ea659164495b663) to get an idea of how you can modify the LED strip declaration to get started with that.  Then you'd need to change all places where the `leds[]` is updated, to include an additional update for each new LED array you add.