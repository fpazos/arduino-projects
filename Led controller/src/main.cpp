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
typedef struct Animation {
  int startIndex;
  int8_t animationSpeed;
  int8_t auxAnimation;
};

Animation currentAnimation = { 0, 1, 0 };

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
struct Brightness colorsAux = {0, 0, false};

CRGB pinkViolet[3] = {CRGB::Pink, CRGB::DarkViolet, NULL};
CRGB pinkViolet2[3] = {CRGB::DeepPink, CRGB::DarkViolet, NULL};
CRGB pinkWhite[3] = {CRGB::Pink, CRGB::Grey, NULL};
CRGB (*colorCombinations[3])[3] = { &pinkViolet, &pinkViolet2, &pinkWhite };
CRGB* currentCombination;

// extern CRGBPalette16 myRedWhiteBluePalette;
// extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

// FUNCTIONS DECLARATIONS
// Fill functions
void completeFill(CRGB, CRGB, CRGB);
void gradientFill(CRGB, CRGB, CRGB);
void blackStripFill(CRGB, CRGB, CRGB);
void intermitentFill(CRGB, CRGB, CRGB);
void linearGradients(CRGB, CRGB, CRGB);
void randFill(CRGB, CRGB, CRGB);
void randomPredefinedFill();
// To be called each cycle

// Animation functions
void linearAnimation();
void backAndForthAnimation();
// void mirrorAnimation();

// Brightness functions
void staticEffect();
void pulseEffect();
void fireEffect();
void flashEffect();


void setup() {
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );

  //Serial.begin(9600); //This pipes to the serial monitor
  //Serial1.begin(9600); //This is the UART, pipes to sensors attached to board

  FastLED.clear();
}

void loop() {
  // put your main code here, to run repeatedly:

  // If cycle duration = 0 set new cycle functions, reset cycle durations, if it isn't rest 1
  if (cycleDuration == 0){
    cycleFunctions[0] = random(3);
    cycleFunctions[1] = random(8);
    cycleFunctions[2] = random(2);
    cycleDuration = random(1000, 5000);

    currentBlending = blendOptions[random(2)];

    // Selects current color combination using cycleFunctions[0]
    currentCombination = (*colorCombinations[cycleFunctions[0]]);

    currentAnimation.animationSpeed = random(4);

    // Selects current fill function
    switch (cycleFunctions[1]){
      case 0:
        completeFill(currentCombination[0], currentCombination[1], currentCombination[2]);
        break;
      case 1:
        gradientFill(currentCombination[0], currentCombination[1], currentCombination[2]);
        break;
      case 2:
        blackStripFill(currentCombination[0], currentCombination[1], currentCombination[2]);
        break;
      case 3:
        intermitentFill(currentCombination[0], currentCombination[1], currentCombination[2]);
        break;
      case 4:
        linearGradients(currentCombination[0], currentCombination[1], currentCombination[2]);
        break;
      case 5:
        randomPredefinedFill();
        break;
      case 6:
        randFill(currentCombination[0], currentCombination[1], currentCombination[2]);
        break;
      case 7:
        break;
    }
  } else {
    cycleDuration--;
  }

  // Fill functions that should be called each cycle
  if (cycleFunctions[1] == 7){
    everyLedFill(currentCombination[0], currentCombination[1], currentCombination[2]);
  }

  // Animation functions
  switch (cycleFunctions[2]){
    case 0:
      linearAnimation();
      break;
    case 1:
      backAndForthAnimation();
      break;
    case 2:
      break;
  }


  // Functions calls
  FillLEDsFromPaletteColors( currentAnimation.startIndex, currentBrightness.brightness);
  
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}



/**
 * That's what really fills the palette with the colors.
 */
void FillLEDsFromPaletteColors( uint8_t colorIndex, uint8_t brightness)
{    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}






/*
  ======================
  LED FILLING FUNCTIONS
  ======================
*/

/*
  This function takes two or three colors and creates an array of 16 values with these arguments.
  [1,2,1,2, ...]
  [1,2,3,1,2,3, ...]
*/

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

/* 
  Linear gradient fill
  1 -> 2 -> 1
  1 -> 2 -> 3 -> 1
*/
void gradientFill(CRGB color1, CRGB color2, CRGB color3){
  if (color3 == NULL){
    fill_gradient_RGB(currentPalette, 0, color1, 7, color2);
    fill_gradient_RGB(currentPalette, 8, color2, 15, color1);
  } else {
    fill_gradient_RGB(currentPalette, 0, color1, 5, color2);
    fill_gradient_RGB(currentPalette, 6, color2, 10, color3);
    fill_gradient_RGB(currentPalette, 11, color3, 15, color1);
  }
}

/*
  This function sets up a palette of black and white stripes,
  using code. Since the palette is effectively an array of
  sixteen CRGB colors, the various fill_* functions can be used
  to set them up.
*/
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

/*
  This function inserts a black between the colors, if it receives 2 arguments CRGB, CRGB, it fills the 16 values alternating both colors. If it receives 3 arguments, it fills the 16 values with the first color, then the second color and finally the third color.
*/
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

/*
  This function creates gradients with a sharp edge among gradients.
*/
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

// This function fills the palette with totally random colors.
void randFill(CRGB color1, CRGB color2, CRGB color3) {
  if (color3 == NULL){
    for( int i = 0; i < 16; i++) {
      currentPalette[i] = random8() < 128 ? color1 : color2;
    }
  } else {
    for( int i = 0; i < 16; i++) {
      uint8_t rnd = random8();
      currentPalette[i] = rnd < 85 ? color1 : rnd < 170 ? color2 : color3;
    }
  }
}

/*
  This function fills the palette with a random predefined palette
*/
void randomPredefinedFill() {
  CRGBPalette16 predefinedPalettes[8] = { RainbowColors_p, RainbowStripeColors_p, OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, PartyColors_p, HeatColors_p };
  currentPalette = predefinedPalettes[random(8)];
}


/*
  This function fills the palette with the same color and every
  time it is called it slowly changes the color to the one on the next argument.
  Should be called each cycle.
*/
void everyLedFill(CRGB color1, CRGB color2, CRGB color3) {
  fillWithBlend(color1, color2, colorsAux.brightness);

  if( colorsAux.brightness == 0) {
    colorsAux.brightnessEffectBool = true;
  }
  if( colorsAux.brightness == 255) {
    colorsAux.brightnessEffectBool = false;
  }

  colorsAux.brightness = colorsAux.brightnessEffectBool == true ? colorsAux.brightness + colorsAux.auxBrightness : colorsAux.brightness - colorsAux.auxBrightness;
}

/**
 * Fills solid with a blend of two colors.
 */
void fillWithBlend(CRGB color1, CRGB color2, fract8 blendAmount) {
  CRGB transitionColor = blend(color1, color2, blendAmount);
  fill_solid( currentPalette, 16, transitionColor);
}




/*
  ======================
  ANIMATION FUNCTIONS
  ======================
*/

/**
 * Linear animation function
 */
void linearAnimation(){
  if (currentAnimation.startIndex >= NUM_LEDS){
    currentAnimation.startIndex = 0;
  }
  currentAnimation.startIndex = currentAnimation.startIndex + currentAnimation.animationSpeed;
}

/**
 * Back and forth animation function
 */
void backAndForthAnimation(){
  if (currentAnimation.startIndex >= NUM_LEDS || currentAnimation.startIndex <= 0){
    currentAnimation.animationSpeed = -currentAnimation.animationSpeed;
  }
  currentAnimation.startIndex = currentAnimation.startIndex + currentAnimation.animationSpeed;
}








/*
  ==================
  BRIGHTNESS EFFECTS
  ==================
*/

/*
  Static brightness effect
*/
void staticEffect(){
  currentBrightness.brightness = 255;
}

/*
  Pulse mode brightness effect
*/
void pulseEffect(){
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

/*
  Fire mode brightness effect
*/
void fireEffect(){
  if (currentBrightness.auxBrightness == 0){
    currentBrightness.auxBrightness = random(9);
    currentBrightness.brightness = random(64, 220);
    currentBrightness.brightnessEffectBool = random(2) == 0 ? true : false;
  } else {
    currentBrightness.auxBrightness--;
  }
  currentBrightness.brightness = currentBrightness.brightnessEffectBool == true ? currentBrightness.brightness + currentBrightness.auxBrightness : currentBrightness.brightness - currentBrightness.auxBrightness;
}

/*
  Fire mode brightness effect
*/
void flashEffect(){
  if (currentBrightness.auxBrightness == 0){
    currentBrightness.auxBrightness = 40;
  } else {
    currentBrightness.auxBrightness--;
  }
  currentBrightness.brightness = currentBrightness.auxBrightness < 20 ? 255 : 20;
}