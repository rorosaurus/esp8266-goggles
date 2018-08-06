// 0 is right eye top (between star and data out)
// 16 is left eye top (between star and data out)
// inc clockwise looking out
// todo: figure out this uint32_t, etc shit.  can i save more space?

#include <Adafruit_NeoPixel.h>

#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
#include <avr/power.h>
#endif

#define LED_PIN 5 // pin 5 maps to "D1" label on our board
#define LED_LENGTH 32 // total number of LEDs in our strip.  2x16
#define BUTTON_PIN 4 // this is the pin that our push button is attached to (pin 4 maps to "D2" labelled on our board)
#define MODE_SWITCH_MILLIS 10000 // how many milliseconds between automatically switching modes

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_LENGTH, LED_PIN);

int horizontalLEDs[] = { 4,  4,  3,  5,  2,  6,  1,  7,  0,  8, 15,  9, 14, 10, 13, 11, 12, 12,
                         20, 20, 19, 21, 18, 22, 17, 23, 16, 24, 31, 25, 30, 26, 29, 27, 28, 28
                       };
int verticalLEDs[] = { 0,  0, 15,  1, 14,  2, 13,  3, 12,  4, 11,  5, 10,  6,  9,  7,  8,  8,
                       16, 16, 31, 17, 30, 18, 29, 19, 28, 20, 27, 21, 26, 22, 25, 23, 24, 24
                     };

bool buttonOldState = HIGH;

// cycle through the rainbow
int hue = 0;
// keep track of each mode's progress
int modeCounter = 0;
// fall back brightness, default to ~1/10th max
int brightness = 25;

int mode = 0;
uint32_t lastModeChange;

void setup() {
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  pixels.begin();
  pixels.setBrightness(brightness);
  lastModeChange = millis();
}

void loop() {
  // Initialize all LEDs to off and reset brightness
  //setAllPixels(0x000000);
  pixels.setBrightness(brightness);

  // Update current button state
  bool buttonNewState = digitalRead(BUTTON_PIN);
  
  // if we press the bottom button or run out of time, switch between the visualizations
  if ((buttonNewState == LOW && buttonOldState == HIGH) || (millis() > lastModeChange + MODE_SWITCH_MILLIS)) {
    if (mode < 0 || mode >= 7) mode = 0;
    else mode++;
    lastModeChange = millis();
  }

  // Flashy light stuff time!
  switch (mode) {
    // uniform colors, a rainbow cycling
    case 0: setAllPixels(getRainbow(hue)); break;
    
    // colorwipe the rainbow cycle... like windshield wipers going around
    case 1: colorWipe(getRainbow(hue)); break;

    // rotate a rainbow
    case 2: rainbowRotate(); break;

    // opposite spinners slowly rotating through the rainbow
    case 3: oppositeSpin(getRainbow(hue)); break;

    // rainbow gradient going horizontal forwards
    case 4: rainbowGradient(horizontalLEDs); break;

    // rainbow gradient going horizontal backwards
    case 5: rainbowGradient(horizontalLEDs); break;

    // rainbow gradient going vertical forwards
    case 6: rainbowGradient(verticalLEDs); break;

    // rainbow gradient going vertical backwards
    case 7: rainbowGradient(verticalLEDs); break;
  }

  // Update the LEDs
  pixels.show();

  // Update button state variables
  buttonOldState = buttonNewState;
  
  // Rotate through our rainbow always
  hue++;
  if (hue > 255) hue = 0;

  delay(10);
}

void rainbowRotate() {
  if (modeCounter >= 16)  modeCounter = 0;
  for (int i = 0; i < 16; i++) {
    int leftLED = i + modeCounter;
    int rightLED = 16 + i + modeCounter;
    if (leftLED > 15) leftLED = leftLED - 16;
    if (rightLED > 31) rightLED = rightLED - 16;
    pixels.setPixelColor(leftLED, getRainbow(i * 16));
    pixels.setPixelColor(rightLED, getRainbow(i * 16));
  }
  modeCounter++;
  delay(20);
}

// rainbow gradient, switching direction every 5 seconds
void rainbowGradient(int LEDmapping[]) {
  if (mode % 2 == 0) {
    for (int i = 0; i < 18; i++) {
      pixels.setPixelColor(LEDmapping[i * 2], getRainbow(i * 14 + hue));
      pixels.setPixelColor(LEDmapping[i * 2 + 1], getRainbow(i * 14 + hue));
    }
  }
  else {
    for (int i = 0; i < 18; i++) {
      pixels.setPixelColor(LEDmapping[i * 2], getRainbow((18 - i) * 14 + hue));
      pixels.setPixelColor(LEDmapping[i * 2 + 1], getRainbow((18 - i) * 14 + hue));
    }
  }
  delay(30);
  hue += 10;
}

// each eye rotates three opposite LED patches
// todo: randomize the number of LED patches
void oppositeSpin(uint32_t color) {
  if (modeCounter >= 16) modeCounter = 0;
  for (int i = 0; i < 16; i++) {
    uint32_t c = 0;
    if (((modeCounter + i) & 7) < 2) c = color; // 3 pixels on
    pixels.setPixelColor(   i, c); // First eye
    pixels.setPixelColor(31 - i, c); // Second eye (flipped)
  }
  modeCounter++;
  hue++;
  delay(40);
}

// sets all the LEDs on both eyes
void setAllPixels(uint32_t color) {
  for (int i = 0; i < LED_LENGTH; i++) {
    pixels.setPixelColor(i, color);
  }
}

void colorWipe(uint32_t color) {
  if (modeCounter >= pixels.numPixels() / 2) {
    modeCounter = 0;
    //    newRandColor();
  }
  pixels.setPixelColor(modeCounter, color);
  pixels.setPixelColor(31 - modeCounter, color);
  pixels.show();
  hue += 10; // the delta between this value and 14 is what makes this interesting visually
  delay(50);
  modeCounter++;
}

// Input a value 0 to 255 to get a color value from the color wheel
// this literally saves 40% of my ROM space, so... shamelessly reusing the sample code
uint32_t getRainbow(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
