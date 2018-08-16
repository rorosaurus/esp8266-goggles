#include <ESP8266WiFi.h>
#include <ESP8266WiFiMesh.h>
#include "FastLED.h"

FASTLED_USING_NAMESPACE

// LED globals and macros
#define DATA_PIN    5 // this is the pin that is connected to data in on the first neopixel ring (pin 5 maps to "D1" labelled on our board)
#define LED_TYPE    NEOPIXEL
#define COLOR_ORDER RGB
#define NUM_LEDS    32 // 16 LEDs per eye x 2 eyes
CRGB leds[NUM_LEDS];

// Push button information
#define BUTTON_PIN 4 // this is the pin that our push button is attached to (pin 4 maps to "D2" labelled on our board)
bool buttonOldState = HIGH;

// FastLED macros
#define BRIGHTNESS 25 // 25/255 == 1/10th brightness
#define FRAMES_PER_SECOND 120
#define CHANGE_PATTERN_SECONDS 10 // automatically change to the next pattern after this many seconds

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))


// Globals for handling the wifi/mesh
#define nodeID "" // suffix for the wifi network, leave blank unless you're an accessory (backpack, etc.)
#define meshName "GoggleSquad_" // prefix for the wifi networks
#define meshPassword "ChangeThisWiFiPassword_TODO" // universal password for the networks, for some mild security
int32_t timeOfLastScan = -10000;
unsigned int requestNumber = 0;
unsigned int responseNumber = 0;

// Mesh network function declarations
String manageRequest(const String &request, ESP8266WiFiMesh &meshInstance);
transmission_status_t manageResponse(const String &response, ESP8266WiFiMesh &meshInstance);
void networkFilter(int numberOfNetworks, ESP8266WiFiMesh &meshInstance);

/* Create the mesh node object */
ESP8266WiFiMesh meshNode = ESP8266WiFiMesh(manageRequest, manageResponse, networkFilter, meshPassword, meshName, nodeID, true, 1);

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

  /* return a string to send back */
  return ("Hello world response #" + String(responseNumber++) + " from " + meshInstance.getMeshName() + meshInstance.getNodeID() + ".");
}

/**
   Callback used to decide which networks to connect to once a WiFi scan has been completed.

   @param numberOfNetworks The number of networks found in the WiFi scan.
   @param meshInstance The ESP8266WiFiMesh instance that called the function.
*/
void networkFilter(int numberOfNetworks, ESP8266WiFiMesh &meshInstance) {
  for (int i = 0; i < numberOfNetworks; ++i) {
    String currentSSID = WiFi.SSID(i);
    int meshNameIndex = currentSSID.indexOf(meshInstance.getMeshName());

    /* Connect to any _suitable_ APs which contain meshInstance.getMeshName() */
    if (meshNameIndex >= 0) {
      uint64_t targetNodeID = ESP8266WiFiMesh::stringToUint64(currentSSID.substring(meshNameIndex + meshInstance.getMeshName().length()));

      if (targetNodeID < ESP8266WiFiMesh::stringToUint64(meshInstance.getNodeID())) {
        ESP8266WiFiMesh::connectionQueue.push_back(NetworkInfo(i));
      }
    }
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

  // Our last request got a response, so time to create a new request.
  meshInstance.setMessage("Hello world request #" + String(++requestNumber) + " from " + meshInstance.getMeshName() + meshInstance.getNodeID() + ".");

  // (void)meshInstance; // This is useful to remove a "unused parameter" compiler warning. Does nothing else.
  return statusCode;
}

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
  //meshNode.setStaticIP(IPAddress(192, 168, 4, 22)); // Activate static IP mode to speed up connection times.
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint32_t lastPatternChange = millis(); // keep track of the last time we changed patterns
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

// main loop, which executes forever
void loop() {
  if (millis() - timeOfLastScan > 3000 // Give other nodes some time to connect between data transfers.
      || (WiFi.status() != WL_CONNECTED && millis() - timeOfLastScan > 2000)) { // Scan for networks with two second intervals when not already connected.
    String request = "Hello world request #" + String(requestNumber) + " from " + meshNode.getMeshName() + meshNode.getNodeID() + ".";
    meshNode.attemptTransmission(request, true); // try to talk to others, disconnect after
    timeOfLastScan = millis();

    if (ESP8266WiFiMesh::latestTransmissionOutcomes.empty()) {
      Serial.println("No mesh AP found.");
    } else {
      for (TransmissionResult &transmissionResult : ESP8266WiFiMesh::latestTransmissionOutcomes) {
        if (transmissionResult.transmissionStatus == TS_TRANSMISSION_FAILED) {
          Serial.println("Transmission failed to mesh AP " + transmissionResult.SSID);
        } else if (transmissionResult.transmissionStatus == TS_CONNECTION_FAILED) {
          Serial.println("Connection failed to mesh AP " + transmissionResult.SSID);
        } else if (transmissionResult.transmissionStatus == TS_TRANSMISSION_COMPLETE) {
          // No need to do anything, transmission was successful.
        } else {
          Serial.println("Invalid transmission status for " + transmissionResult.SSID + "!");
        }
      }
    }
    Serial.println();
  } else {
    /* Accept any incoming connections */
    meshNode.acceptRequest();
  }
  
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
