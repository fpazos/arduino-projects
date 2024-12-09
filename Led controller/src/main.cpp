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
typedef struct Speed {
  int8_t animationSpeed;
  int8_t auxSpeed;
};

Speed currenSpeed = {1, 0};

uint8_t minBrightness = 10;
// Current Brightness and aux values for functions
struct Brightness {
  int brightness;
  int auxBrightness;
  bool brightnessEffectBool;
};

struct Brightness currentBrightness = {255, 0, false};


// Tipos de blend
TBlendType currentBlending;
TBlendType blendOptions[2] = {LINEARBLEND, NOBLEND};

// Definiciones de paletas
CRGBPalette16 currentPalette;
struct Brightness currentColors = {0, 0, false};

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
void intermitentFill(CRGB, CRGB, CRGB);
void linearGradients(CRGB, CRGB, CRGB);
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
  startIndex = startIndex + currenSpeed.animationSpeed; /* motion speed */
  
  FillLEDsFromPaletteColors( startIndex, currentBrightness.brightness);
  
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
void SetupRosaVioleta() {
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Pink);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::Violet;
    currentPalette[4] = CRGB::Violet;
    currentPalette[8] = CRGB::Violet;
    currentPalette[12] = CRGB::Violet;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette() {
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}

// This function takes two or three colors and creates an array of 16 values with these arguments.
void completeFill(CRGB color1, CRGB color2, CRGB color3) {
  if (color3 == NULL){
    CRGB colors[2] = {color1, color2};
    for (uint8_t i = 0; i < 16; i++){
      currentPalette[i] = colors[i % 2];
    }
  } else {
    CRGB colors[3] = {color1, color2, color3};
    for (uint8_t i = 0; i < 16; i++){
      currentPalette[i] = colors[i % 3];
    }
  }
  
}

// This function creates a sequence of 16 colors, if it receives 2 arguments CRGB, CRGB, it fills the 16 values alternating both colors. If it receives 3 arguments, it fills the 16 values with the first color, then the second color and finally the third color.
void partialFill(CRGB color1, CRGB color2, CRGB color3){
  if (color3 == NULL){
    fill_gradient_RGB(currentPalette, 0, color1, 7, color2);
    fill_gradient_RGB(currentPalette, 8, color2, 15, color1);
  } else {
    fill_gradient_RGB(currentPalette, 0, color1, 5, color2);
    fill_gradient_RGB(currentPalette, 6, color2, 10, color3);
    fill_gradient_RGB(currentPalette, 11, color3, 15, color1);
  }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void blackStripFill(CRGB color1, CRGB color2, CRGB color3) {
  // 'black out' all 16 palette entries...
  fill_solid( currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = color1;
  currentPalette[4] = color2;
  if (color3 != NULL){
    currentPalette[8] = color3;
    currentPalette[12] = color1;
  } else {
    currentPalette[8] = color1;
    currentPalette[12] = color2;
  }
}

// This function inserts a black between the colors, if it receives 2 arguments CRGB, CRGB, it fills the 16 values alternating both colors. If it receives 3 arguments, it fills the 16 values with the first color, then the second color and finally the third color.
void intermitentFill(CRGB color1, CRGB color2, CRGB color3){
  fill_solid( currentPalette, 16, CRGB::Black);
  if (color3 == NULL){
    CRGB colors[2] = {color1, color2};
    for (uint8_t i = 0; i < 8; i++) {
      currentPalette[i * 2] = colors[i % 2];
    }
  } else {
    CRGB colors[3] = {color1, color2, color3};
    for (uint8_t i = 0; i < 8; i++) {
      currentPalette[i * 2] = colors[i % 3];
    }
  }
}

// This function creates gradients with a sharp edge among gradients
void linearGradients(CRGB color1, CRGB color2, CRGB color3){
  if (color3 == NULL){
    fill_gradient_RGB(currentPalette, 0, color1, 7, color2);
    fill_gradient_RGB(currentPalette, 8, color1, 15, color2);
  } else {
    fill_gradient_RGB(currentPalette, 0, color1, 5, color2);
    fill_gradient_RGB(currentPalette, 6, color3, 10, color1);
    fill_gradient_RGB(currentPalette, 11, color2, 15, color3);
  }
}

// This function fills the palette with the same color and every time it is called it slowly changes the color to the one on the next argument
void everyLedFill(CRGB color1, CRGB color2, CRGB color3) {
  fillWithBlend(color1, color2, currentColors.brightness);

  if( currentColors.brightness == 0) {
    currentColors.brightnessEffectBool = true;
  }
  if( currentColors.brightness == 255) {
    currentColors.brightnessEffectBool = false;
  }

  currentColors.brightness = currentColors.brightnessEffectBool == true ? currentColors.brightness + currentColors.auxBrightness : currentColors.brightness - currentColors.auxBrightness;
}

void fillWithBlend(CRGB color1, CRGB color2, fract8 blendAmount){
  CRGB transitionColor = blend(color1, color2, blendAmount);
  fill_solid( currentPalette, 16, transitionColor);
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
/* const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
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
}; */

// Pulse mode brightness effect
Brightness pulseEffect(){
  if (currentBrightness.brightness >= 250){
      currentBrightness.brightnessEffectBool = false;
  }
  if (currentBrightness.brightness <= minBrightness){
      currentBrightness.brightnessEffectBool = true;
  }
  if(currentBrightness.brightnessEffectBool == true){
      currentBrightness.brightness = currentBrightness.brightness + currentBrightness.auxBrightness;
  }
  if(currentBrightness.brightnessEffectBool == false) {
      currentBrightness.brightness = currentBrightness.brightness - currentBrightness.auxBrightness;
  }
}