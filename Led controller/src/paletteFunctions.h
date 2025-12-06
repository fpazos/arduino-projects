
// Fill functions
/* void completeFill(CRGB, CRGB, CRGB);
void partialFill(CRGB, CRGB, CRGB);
void blackStripFill(CRGB, CRGB, CRGB);
void everyLedFill(CRGB, CRGB, CRGB);
void randFill(CRGB, CRGB, CRGB);
 */
// Palette functions
void SelectRandomPalette();
void SetupTotallyRandomPalette();
void SetupRosaVioleta();
void SetupColorAndWhiteStripedPalette();
void SetupTwoColorsPalette();

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
void SetupColorAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    CRGB color1 = CHSV( random(0, 256), 255, 255);
    // and set every fourth one to white.
    currentPalette[0] = color1;
    currentPalette[4] = color1;
    currentPalette[8] = color1;
    currentPalette[12] = color1;
    
}

// This function sets up a palette of purple and green stripes.
void SetupTwoColorsPalette()
{
    CRGB color1 = CHSV( random(0, 256), 255, 255);
    CRGB color2  = CHSV( random(0, 256), 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   color1,  color1,  black,  black,
                                   color2, color2, black,  black,
                                   color1,  color1,  black,  black,
                                   color2, color2, black,  black );
}
void SelectRandomPalette(uint8_t paletteChoice)
{
  switch(paletteChoice) {
    case 0:
      SetupTotallyRandomPalette();
      break;
    case 1:
      SetupRosaVioleta();
      break;
    case 2:
      SetupColorAndWhiteStripedPalette();
      break;
    case 3:
      SetupTwoColorsPalette();
      break;
    default:
      SetupTotallyRandomPalette();
      break;
  }
}

#ifndef PALETTE_FUNCTIONS_H
#define PALETTE_FUNCTIONS_H

#include <FastLED.h>

extern CRGBPalette16 currentPalette;
extern uint8_t cycleFunctions[3];

void SelectRandomPalette(uint8_t paletteChoice);
uint8_t SelectRandomFill(uint8_t fillChoice);

#endif