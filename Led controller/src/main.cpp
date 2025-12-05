#include <Arduino.h>
#include <FastLED.h>

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

Speed currentSpeed = {1, 0};

uint8_t minBrightness = 10;
bool pulseDirection = true;
int8_t pulseSpeed = 5;
bool pulse = false;
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

// extern CRGBPalette16 myRedWhiteBluePalette;
// extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

// FUNCTIONS DECLARATIONS
// Fill functions
void completeFill(CRGB, CRGB, CRGB);
void partialFill(CRGB, CRGB, CRGB);
void blackStripFill(CRGB, CRGB, CRGB);
void everyLedFill(CRGB, CRGB, CRGB);
void randFill(CRGB, CRGB, CRGB);

// Speed functions
struct Speed linearAnimation(struct Speed);
struct Speed backAndForthAnimation(struct Speed);
struct Speed mirrorAnimation(struct Speed);

// Brightness functions
struct Brightness staticEffect(struct Brightness);
struct Brightness pulseEffect(struct Brightness);
struct Brightness fireEffect(struct Brightness);
struct Brightness flashEffect(struct Brightness);


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
    cycleFunctions[0] = random(0, 5);
    cycleFunctions[1] = random(0, 5);
    cycleFunctions[2] = random(0, 5);
    cycleDuration = random(1000, 5000);

    currentBlending = blendOptions[random(0, 2)];
  } else {
    cycleDuration--;
  }


  // Functions calls

  static uint8_t startIndex = 0;
  startIndex = startIndex + currentSpeed.animationSpeed; /* motion speed */
  
  // Update LED colors from palette
  for( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, startIndex + (i * 256 / NUM_LEDS), currentBrightness.currentBrightness, currentBlending);
  }
  
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupRosaVioleta()

{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Pink);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::Violet;
    currentPalette[4] = CRGB::Violet;
    currentPalette[8] = CRGB::Violet;
    currentPalette[12] = CRGB::Violet;
    
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};

// Pulse mode brightness effect
struct Brightness pulseEffect(struct Brightness brightness){
  // Pulse mode
  if (pulse == true){
      if (brightness.currentBrightness >= 250){
          pulseDirection = false;
      }
      if (brightness.currentBrightness <= minBrightness){
          pulseDirection = true;
      }
      if(pulseDirection == true){
          brightness.currentBrightness = brightness.currentBrightness + pulseSpeed;
      }
      if(pulseDirection == false) {
          brightness.currentBrightness = brightness.currentBrightness - pulseSpeed;
      }
  }
  return brightness;
}