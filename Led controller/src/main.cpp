#include <Arduino.h>
#include <FastLED.h>
#include "paletteFunctions.h"
#include "animationFunctions.h"


// FASTLED DEFINITIONS
// Leds definitions
#define LED_PIN     6
#define NUM_LEDS    30
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

// Buttons
#define CHANGE_PIN  3
#define FUNC_PIN1   4
#define FUNC_PIN2   5
#define FUNC_PIN3   8
#define FUNC_PIN4   7

CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

// VARIABLES
uint16_t cycleDuration = 0;
uint8_t cycleFunctions[3] = {0,0,0};

// SPEED
// Animation speed and aux values for functions
struct Speed {
  int8_t animationSpeed;
  int8_t auxSpeed;
};

static uint8_t startIndex = 0;
Speed currentSpeed = {1, 0};

// Current Brightness and aux values for functions
struct Brightness {
  uint8_t currentBrightness;
  uint8_t auxBrightness;
  bool brightnessEffectBool;
};

Brightness currentBrightness = {64, 0, false};

// Tipos de blend
TBlendType currentBlending;
TBlendType blendOptions[2] = {LINEARBLEND, NOBLEND};

// Definiciones de paletas
CRGBPalette16 currentPalette;

CRGB pinkViolet[2] = {CRGB::Pink, CRGB::DarkViolet};
CRGB pinkViolet2[2] = {CRGB::DeepPink, CRGB::DarkViolet};
CRGB pinkWhite[2] = {CRGB::Pink, CRGB::Grey};

void setup() {
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );

  //Serial.begin(9600); //This pipes to the serial monitor
  //Serial1.begin(9600); //This is the UART, pipes to sensors attached to board
  
  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;

  FastLED.clear();
}

void loop() {
  // put your main code here, to run repeatedly:

  // If cycle duration = 0 set new cycle functions, reset cycle durations, if it isn't rest 1
  if (cycleDuration == 0){
    cycleFunctions[0] = random(0, 10);
    cycleFunctions[1] = random(0, 5);
    cycleFunctions[2] = random(0, 3);
    cycleDuration = random(1000, 5000);

    currentBlending = blendOptions[random(0, 2)];
    currentSpeed.animationSpeed = random(0, 2);
    currentSpeed.auxSpeed = random(0, 2);

    SelectRandomPalette(cycleFunctions[0]);
    // SelectRandomFill(cycleFunctions[1]);
  } else {
    startIndex = ProcessRandomAnimation(cycleFunctions[2]);
    cycleDuration--;
  }


  // Functions calls
  startIndex = startIndex + currentSpeed.animationSpeed; /* motion speed */
  
  // Update LED colors from palette
  for( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, startIndex + (i * 256 / NUM_LEDS), currentBrightness.currentBrightness, currentBlending);
  }
  
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}
