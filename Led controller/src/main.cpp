#include <Arduino.h>
#include <FastLED.h>

// FASTLED DEFINITIONS
// Leds definitions
#define LED_PIN     6
#define NUM_LEDS    34
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
uint8_t cycleFunctions[4] = {0,0,0,0};

// SPEED
// Animation speed and aux values for functions
struct Animation {
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

CRGB favouriteColors[24] = {
  CRGB::Pink,             CRGB::DarkViolet,           CRGB::Amethyst,          CRGB::DeepPink,
  CRGB::Grey,             CRGB::Aqua,                 CRGB::Aquamarine,     CRGB::Cyan,
  CRGB::MediumSpringGreen,CRGB::BlueViolet,           CRGB::LavenderBlush,  CRGB::DarkOrchid,
  CRGB::HotPink,          CRGB::LightPink,            CRGB::Violet,         CRGB::Magenta,
  CRGB::Indigo,           CRGB::LightGreen,           CRGB::MediumOrchid,   CRGB::MediumPurple,
  CRGB::Plum,             CRGB::Purple,               CRGB::Teal,           CRGB::Turquoise,
};
CRGB currentCombination[3];

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
void everyLedFill(CRGB, CRGB, CRGB);
void FillLEDsFromPaletteColors(uint8_t, uint8_t);
// To be called each cycle

// Animation functions
Animation linearAnimation(Animation);
Animation backAndForthAnimation(Animation);
// void mirrorAnimation();

// Brightness functions
void staticEffect();
void pulseEffect();
void fireEffect();
void flashEffect();

void selectRandomCombination() {
  uint8_t randomIndex = random(4);
  CRGB colors[3];
  
  if (randomIndex == 0) {
    colors[0] = CHSV(random(0, 256), 255, 255);
    colors[1] = CHSV(random(0, 256), 255, 255);
    colors[2] = CRGB(1, 1, 1);
  } else if (randomIndex == 1) {
    colors[0] = CHSV(random(0, 256), 255, 255);
    colors[1] = CHSV(random(0, 256), 255, 255);
    colors[2] = CHSV(random(0, 256), 255, 255);
  } else if (randomIndex == 2) {
    colors[0] = favouriteColors[random(24)];
    colors[1] = favouriteColors[random(24)];
    colors[2] = CRGB(1, 1, 1);
  } else {
    colors[0] = favouriteColors[random(24)];
    colors[1] = favouriteColors[random(24)];
    colors[2] = favouriteColors[random(24)];
  }
  
  memcpy(currentCombination, colors, sizeof(colors));
}


void setup() {
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); // pylance: ignore
  FastLED.setBrightness(  BRIGHTNESS );

  // Serial.begin(9600); //This pipes to the serial monitor
  // Serial1.begin(9600); //This is the UART, pipes to sensors attached to board

  FastLED.clear();
}

void loop() {
  // put your main code here, to run repeatedly:

  // If cycle duration = 0 set new cycle functions, reset cycle durations, if it isn't rest 1
  if (cycleDuration == 0){
    cycleFunctions[0] = random(0, 3);
    cycleFunctions[1] = random(0, 7);
    cycleFunctions[2] = random(0, 2);
    cycleFunctions[3] = random(0, 3);
    cycleDuration = random(500, 2000);

    currentBlending = blendOptions[random(0, 2)];

    // Selects current color combination using cycleFunctions[0]
    selectRandomCombination();

    // Sets animation values
    currentAnimation.animationSpeed = random(1, 4);
    if (cycleFunctions[2] == 1){
      currentAnimation.auxAnimation = random(3, 10);
    }

    /* SERIAL INFORMATION */
    /* Serial.println("New cycle:");
    Serial.print(" Color combination function: ");
    Serial.println(cycleFunctions[0]);
    Serial.print(" Fill function: ");
    Serial.println(cycleFunctions[1]);
    Serial.print(" Animation function: ");
    Serial.println(cycleFunctions[2]);
    Serial.print(" Brightness function: ");
    Serial.println(cycleFunctions[3]);
    Serial.print(" Cycle duration (ms): ");
    Serial.println(cycleDuration);
    Serial.print(" Animation speed: ");
    Serial.println(currentAnimation.animationSpeed);
    Serial.print(" Color 1: ");
    Serial.print(currentCombination[0].r);
    Serial.print(", ");
    Serial.print(currentCombination[0].g);
    Serial.print(", ");
    Serial.println(currentCombination[0].b);
    Serial.print(" Color 2: ");
    Serial.print(currentCombination[1].r);
    Serial.print(", ");
    Serial.print(currentCombination[1].g);
    Serial.print(", ");
    Serial.println(currentCombination[1].b);
    Serial.print(" Color 3: ");
    Serial.print(currentCombination[2].r);
    Serial.print(", ");
    Serial.print(currentCombination[2].g);
    Serial.print(", ");
    Serial.println(currentCombination[2].b); */


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
        break;/* 
      case 7:
        break; */
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
      currentAnimation = linearAnimation(currentAnimation);
      break;
    case 1:
      currentAnimation = backAndForthAnimation(currentAnimation);
      break;
    case 2:
      break;
  }

  // Brightness functions
  switch (cycleFunctions[3]){
    case 0:
      staticEffect();
      break;
    case 1:
      pulseEffect();
      break;
    case 2:
      fireEffect();
      break;
    case 3:
      flashEffect();
      break;
  }

  // Serial information starting index
  /* if (cycleDuration < 10){
    Serial.print("Start index: ");
    Serial.println(currentAnimation.startIndex);
  } */


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
  if (color3 == CRGB(1,1,1)){
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
  if (color3 == CRGB(1,1,1)){
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
  if (color3 != CRGB(1,1,1)){
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
  if (color3 == CRGB(1,1,1)){
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
  if (color3 == CRGB(1,1,1)){
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
  if (color3 == CRGB(1,1,1)){
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
  currentPalette = predefinedPalettes[random(0, 8)];
}


/**
 * Fills solid with a blend of two colors.
 */
void fillWithBlend(CRGB color1, CRGB color2, fract8 blendAmount) {
  CRGB transitionColor = blend(color1, color2, blendAmount);
  fill_solid( currentPalette, 16, transitionColor);
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




/*
  ======================
  ANIMATION FUNCTIONS
  ======================
*/

/**
 * Linear animation function
 */
Animation linearAnimation(Animation currentAnimation){
  if (currentAnimation.startIndex >= NUM_LEDS){
    currentAnimation.startIndex = 0;
  }
  currentAnimation.startIndex = currentAnimation.startIndex + currentAnimation.animationSpeed;
  return currentAnimation;
}

/**
 * Back and forth animation function
 */
Animation backAndForthAnimation(Animation currentAnimation){
  static bool goingForward = true;
  /* if (currentAnimation.startIndex >= NUM_LEDS || currentAnimation.startIndex <= 0){
    currentAnimation.animationSpeed = -currentAnimation.animationSpeed;
  } */
  if (currentAnimation.startIndex >= currentAnimation.auxAnimation){
    goingForward = false;
  } else if (currentAnimation.startIndex <= 0){
    goingForward = true;
  }
  if (goingForward){
    currentAnimation.startIndex = currentAnimation.startIndex + currentAnimation.animationSpeed;
  } else {
    currentAnimation.startIndex = currentAnimation.startIndex - currentAnimation.animationSpeed;
  }
  // Serial animation debugging
  /* if (cycleDuration < 10){
    Serial.print("Going forward: ");
    Serial.println(goingForward);
    Serial.print("auxAnimation: ");
    Serial.println(currentAnimation.auxAnimation);
  } */
  return currentAnimation;
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
  } else if (currentBrightness.brightness <= minBrightness){
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