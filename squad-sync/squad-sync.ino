#include <ESP8266WiFi.h>
#include <ESP8266WiFiMesh.h>

#define FASTLED_ALLOW_INTERRUPTS 0 // with the esp8266, we have to bitbang the output, so disable interrupts during fastLED so we don't glitch out

#include "FastLED.h"

FASTLED_USING_NAMESPACE

// LED globals and macros
#define DATA_PIN    5 // this is the pin that is connected to data in on the first neopixel ring (pin 5 maps to "D1" labelled on our board)
#define LED_TYPE    NEOPIXEL
#define COLOR_ORDER RGB
#define NUM_LEDS    32 // 16 LEDs per eye x 2 eyes
CRGB leds[NUM_LEDS];

uint8_t currentPatternNumber = 0; // Index number of which pattern is current
uint32_t lastPatternChange = millis(); // keep track of the last time we changed patterns
uint8_t hue = 0; // rotating "base color" used by many of the patterns
uint32_t lastHueChange = millis();

// Push button information
#define BUTTON_PIN 4 // this is the pin that our push button is attached to (pin 4 maps to "D2" labelled on our board)
bool buttonOldState = HIGH;
int32_t lastButtonHigh = 999999999; // the millis of the last time we saw the button as "high" (not pressed)
#define BUTTON_MIDPRESS_MILLIS 500
#define BUTTON_LONGPRESS_MILLIS 3000 // number of millis before registering a long press

// FastLED macros
#define BRIGHTNESS 25 // 25/255 == 1/10th brightness
#define FRAMES_PER_SECOND 120
#define CHANGE_PATTERN_SECONDS 10 // automatically change to the next pattern after this many seconds
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// Globals for handling the wifi/mesh
bool wifiEnabled = true;
String masterNodeID = "";
#define nodeID "" // suffix for the wifi network, leave blank unless you're an accessory (backpack, etc.)
#define meshName "GoggleSquad_" // prefix for the wifi networks
#define meshPassword "ChangeThisWiFiPassword_TODO" // universal password for the networks, for some mild security
#define SYNC_INTERVAL_MILLIS 600000 // sync state variables every 10 minutes
#define scanIntervalMillis 30000 // scan for new master every 30 seconds
int32_t timeOfLastScan = 0;
unsigned int requestNumber = 0;
unsigned int responseNumber = 0;
String request = "";

// Mesh network function declarations
String manageRequest(const String &request, ESP8266WiFiMesh &meshInstance);
transmission_status_t manageResponse(const String &response, ESP8266WiFiMesh &meshInstance);
void networkFilter(int numberOfNetworks, ESP8266WiFiMesh &meshInstance);

/* Create the mesh node object */
ESP8266WiFiMesh meshNode = ESP8266WiFiMesh(manageRequest, manageResponse, networkFilter, meshPassword, meshName, nodeID, true, 1);

// ====================
// wifi functions
// ====================

void attemptSync(){
  Serial.println(request);
  meshNode.attemptTransmission(request, true); // try to talk to others, disconnect after
  timeOfLastScan = millis();
}

/**
   Callback for when other nodes send you a request

   @param request The request string received from another node in the mesh
   @param meshInstance The ESP8266WiFiMesh instance that called the function.
   @returns The string to send back to the other node
*/
String manageRequest(const String &request, ESP8266WiFiMesh &meshInstance) {
  /* Print out received message */
  Serial.print("Request received: ");
  Serial.println(request);

  int millisTillNextChange = (lastPatternChange + (CHANGE_PATTERN_SECONDS*1000)) - millis();
  if (millisTillNextChange < 0) millisTillNextChange = 0;

  // someone wants to be our slave!  we're in a group and we're leading!
  masterNodeID = meshNode.getNodeID();
  
  /* return a string to send back, encoding the state variables to reproduce the current LED animation */
  return ("currentPatternNumber:" + String(currentPatternNumber) + ",hue:" + String(hue) + ",millisTillNextChange:" + String(millisTillNextChange));
}

/**
   Callback used to decide which networks to connect to once a WiFi scan has been completed.

   @param numberOfNetworks The number of networks found in the WiFi scan.
   @param meshInstance The ESP8266WiFiMesh instance that called the function.
*/
void networkFilter(int numberOfNetworks, ESP8266WiFiMesh &meshInstance) {
  int lowestValidNetworkIndex = -1;
  uint64_t lowestNodeID = 0xFFFFFFFFFFFFFFFF;
  
  for (int i = 0; i < numberOfNetworks; ++i) {
    String currentSSID = WiFi.SSID(i);
    int meshNameIndex = currentSSID.indexOf(meshInstance.getMeshName());

    /* Connect to any _suitable_ APs which contain meshInstance.getMeshName() */
    if (meshNameIndex >= 0) {
      uint64_t targetNodeID = ESP8266WiFiMesh::stringToUint64(currentSSID.substring(meshNameIndex + meshInstance.getMeshName().length()));

      // see if this is the lowestID we've found yet
      if(targetNodeID < lowestNodeID){
        lowestNodeID = targetNodeID;
        lowestValidNetworkIndex = i;
      }
    }
  }
  if (lowestValidNetworkIndex == -1) { // we're all alone out here...
    masterNodeID = "";
  }
  
  // only connect to the AP with the lowest NodeID
  if (lowestNodeID < ESP8266WiFiMesh::stringToUint64(meshInstance.getNodeID())) {
    // we have found a master node!  connect!
    ESP8266WiFiMesh::connectionQueue.push_back(NetworkInfo(lowestValidNetworkIndex));
  }
}

/**
   Callback for when you get a response from other nodes

   @param response The response string received from another node in the mesh
   @param meshInstance The ESP8266WiFiMesh instance that called the function.
   @returns The status code resulting from the response, as an int
*/
transmission_status_t manageResponse(const String &response, ESP8266WiFiMesh &meshInstance) {
  transmission_status_t statusCode = TS_TRANSMISSION_COMPLETE;

  /* Print out received message */
  Serial.print("Request sent: ");
  Serial.println(meshInstance.getMessage());
  Serial.print("Response received: ");
  Serial.println(response);

  // Read the response string into the corresponding state variables
  int commaIndex = response.indexOf(",");
  String patternNumberString = response.substring(21, commaIndex);
  currentPatternNumber = atoi(patternNumberString.c_str());
  
  String remainingString = response.substring(commaIndex+1);
  commaIndex = remainingString.indexOf(",");
  String hueString = remainingString.substring(4, commaIndex);
  hue = atoi(hueString.c_str());

  remainingString = remainingString.substring(commaIndex+1);
  String nextChangeString = remainingString.substring(21);
  lastPatternChange = millis() - ((CHANGE_PATTERN_SECONDS * 1000) - (atoi(nextChangeString.c_str())));

  // we have joined a group!  keep track of our master node's ID.
  masterNodeID = meshInstance.getNodeID();
  
  // (void)meshInstance; // This is useful to remove a "unused parameter" compiler warning. Does nothing else.
  return statusCode;
}

// ====================
// main functions
// ====================

// setup function. runs once, then loop() runs forever
void setup() {
  // prep our button so we can read it later
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  // Prevents the flash memory from being worn out, see: https://github.com/esp8266/Arduino/issues/1054 .
  // This will however delay node WiFi start-up by about 700 ms. The delay is 900 ms if we otherwise would have stored the WiFi network we want to connect to.
  WiFi.persistent(false);

  Serial.begin(115200);
  delay(50); // Wait for Serial.
  Serial.println();
  Serial.println();
  Serial.println("Setting up mesh node...");
  
  /* Initialise the mesh node */
  meshNode.begin();
  meshNode.activateAP(); // Each AP requires a separate server port.

  request = "Hello world request #" + String(requestNumber) + " from " + meshNode.getMeshName() + meshNode.getNodeID() + ".";
  
  // TODO: give each device unique static IPs, saves 200ms on wifi connect
  //meshNode.setStaticIP(IPAddress(192, 168, 4, 22)); // Activate static IP mode to speed up connection times.

  // force an initial scan/sync
  attemptSync();
}

typedef void (*SimplePatternList[])(); // List of patterns to cycle through.  Each is defined as a separate function below.
SimplePatternList patterns = { rainbow, rainbowWithGlitter, confetti, oppositeSpin, oppositeSin, sinelon, juggle, bpm };

// main loop, which executes forever
void loop() {
  if (wifiEnabled) {
    if (millis() > timeOfLastScan + SYNC_INTERVAL_MILLIS) { // if we haven't sync'd in a while, search and sync
        attemptSync();
    //} else if (){ // TODO: check for new or abandoned master node
      
    } else {
      /* Accept any incoming connections */
      meshNode.acceptRequest();
    }
  }
    
  handleButton();
  
  // Call the current pattern function once, updating the 'leds' array
  patterns[currentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  if (millis() >= lastHueChange + 20) {
    hue++;
    lastHueChange = millis();
  }
  // change the animation every now and then
  if (millis() >= lastPatternChange + (CHANGE_PATTERN_SECONDS * 1000)
      && wifiEnabled) {  // if we're in wifi/group mode
    nextPattern();
  }
}

void nextPattern() {
  // add one to the current pattern number, and wrap around at the end
  currentPatternNumber = (currentPatternNumber + 1) % ARRAY_SIZE(patterns);
  lastPatternChange = millis();
}

void handleButton() {
  // Update current button state
  bool buttonNewState = digitalRead(BUTTON_PIN);
    
  // handle longpress
  if (millis() > lastButtonHigh + BUTTON_LONGPRESS_MILLIS) {
    // toggle wifiEnabled
    wifiEnabled = !wifiEnabled;
    
    // give user some feedback
    uint32_t startTime = millis();
    if (wifiEnabled) {
      fill_solid(leds, NUM_LEDS, CRGB::Green);
      FastLED.show();
      meshNode.activateAP();
      masterNodeID = "";
      if (millis() - startTime < 2000){
        FastLED.delay(2000 - (millis() - startTime));
      }
      attemptSync();
    }
    else { // wifi is disabled
      fill_solid(leds, NUM_LEDS, CRGB::Red);
      FastLED.show();
      meshNode.deactivateAP();
      WiFi.mode(WIFI_OFF);
      masterNodeID = "";
      if (millis() - startTime < 2000){
        FastLED.delay(2000 - (millis() - startTime));
      }
    }
  }

  // handle mid-press
  else if (buttonNewState == HIGH && buttonOldState == LOW // we just let go of the button
      && masterNodeID != meshNode.getNodeID() // prevent the lead node from winking, so everyone stays in sync
      && millis() > lastButtonHigh + BUTTON_MIDPRESS_MILLIS) {
    winkyFace();
  }

  // handle short press if we aren't in a group
  else if (masterNodeID == "" && buttonNewState == HIGH && buttonOldState == LOW) {
    nextPattern();
  }

  buttonNewState = digitalRead(BUTTON_PIN);
  if (buttonNewState == HIGH) lastButtonHigh = millis();
  buttonOldState = buttonNewState;  // Update button state variables
}

// ====================
// animation functions
// ====================

void winkyFace() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  // init eyes
  leds[1] = CRGB::Yellow;
  leds[(NUM_LEDS/2)-1] = CRGB::Yellow;
  leds[(NUM_LEDS/2)+1] = CRGB::Yellow;
  leds[NUM_LEDS-1] = CRGB::Yellow;
  FastLED.show();  
  FastLED.delay(100);

  // animate smile
  for (int j=5; j < 12; j++) {
    leds[j] = CRGB::Yellow;
    leds[j+(NUM_LEDS/2)] = CRGB::Yellow;
    FastLED.show();  
    FastLED.delay(50);
  }
  FastLED.show();  
  FastLED.delay(250);

  // wink
  for (int i=0; i < FRAMES_PER_SECOND/2; i++) {
    if (i < (FRAMES_PER_SECOND/4)){ // fade out wink
      leds[1]              = blend(CRGB::Yellow, CRGB::Black, map(i, 0, (FRAMES_PER_SECOND/4), 0, 255));
      leds[(NUM_LEDS/2)+1] = blend(CRGB::Yellow, CRGB::Black, map(i, 0, (FRAMES_PER_SECOND/4), 0, 255));
    }
    else { // fade in wink
      leds[1]              = blend(CRGB::Black, CRGB::Yellow, map(i, (FRAMES_PER_SECOND/4), (FRAMES_PER_SECOND/2)-1, 0, 255));
      leds[(NUM_LEDS/2)+1] = blend(CRGB::Black, CRGB::Yellow, map(i, (FRAMES_PER_SECOND/4), (FRAMES_PER_SECOND/2)-1, 0, 255));
    }
    
    FastLED.show();  
    FastLED.delay(1000/FRAMES_PER_SECOND);
  }

  uint32_t startTime = millis();
  if (wifiEnabled){
    attemptSync(); // try to resync with the group
  }
  // make sure we wait at least 1 second
  if (millis() - startTime < 1000){
    FastLED.delay(1000 - (millis() - startTime));
  }
}

void rainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, hue, 7);
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
  leds[pos] += CHSV( hue + random8(64), 200, 255);
}

void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( hue, 255, 192);
}

void oppositeSpin() {
  // each eye rotates opposite LED patches of 2-3
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  int pos = map(beat8(80), 0, 255, 0, (NUM_LEDS/2)-1);
  for (int i=0; i< (NUM_LEDS/2); i++){
    if (((pos + i) & ((NUM_LEDS/4)-1)) < 2){
      leds[i] += CHSV( hue, 255, 192);
      leds[NUM_LEDS - i] += CHSV( hue, 255, 192);
    }
  }
}

void oppositeSin() {
  // each eye rotates opposite LED patches of 2-3 to sinusoid
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  int pos = map(beatsin8(60), 0, 255, 0, (NUM_LEDS/2)-1);
  for (int i=0; i< (NUM_LEDS/2); i++){
    if (((pos + i) & ((NUM_LEDS/4)-1)) < 2){
      leds[i] += CHSV( hue, 255, 192);
      leds[NUM_LEDS - i] += CHSV( hue, 255, 192);
    }
  }
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
    leds[i] = ColorFromPalette(palette, hue+(i*2), beat-hue+(i*10));
  }
}
