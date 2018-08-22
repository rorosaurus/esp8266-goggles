#define FASTLED_ALLOW_INTERRUPTS 0

#include "FastLED.h"

FASTLED_USING_NAMESPACE

#define DATA_PIN    5 // this is the pin that is connected to data in on the first neopixel ring (pin 5 maps to "D1" labelled on our board)
#define LED_TYPE    NEOPIXEL
#define COLOR_ORDER RGB
#define NUM_LEDS    32 // 16 LEDs per eye x 2 eyes
CRGB leds[NUM_LEDS];

#define BUTTON_PIN 4 // this is the pin that our push button is attached to (pin 4 maps to "D2" labelled on our board)
bool buttonOldState = HIGH;

#define BRIGHTNESS 25 // 25/255 = 1/10th brightness
#define FRAMES_PER_SECOND 120
#define CHANGE_PATTERN_SECONDS 10 // automatically change to the next pattern after this many seconds

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// setup function. runs once, then loop() runs forever
void setup() {
  // prep our button so we can read it later
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint32_t lastPatternChange = millis(); // keep track of the last time we changed patterns
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

// main loop, which executes forever
void loop() {
  // Update current button state
  bool buttonNewState = digitalRead(BUTTON_PIN);
  if (buttonNewState == LOW && buttonOldState == HIGH) nextPattern();
  buttonOldState = buttonNewState;  // Update button state variables
  
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS(20) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS(CHANGE_PATTERN_SECONDS) { patternTimer(); } // change patterns periodically
}

void patternTimer() {
  if (lastPatternChange + (CHANGE_PATTERN_SECONDS * 1000) <= millis()) {
    nextPattern();
  }
}

void nextPattern() {
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
  lastPatternChange = millis();
}

// ====================
// animation functions
// ====================

void rainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() {
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter(fract8 chanceOfGlitter) {
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void bpm() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}
