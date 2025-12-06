uint8_t minBrightness = 10;
bool pulseDirection = true;
int8_t pulseSpeed = 5;
bool pulse = false;

// Speed functions
struct Speed linearAnimation(struct Speed);
struct Speed backAndForthAnimation(struct Speed);
struct Speed mirrorAnimation(struct Speed);

// Brightness functions
struct Brightness staticEffect(struct Brightness);
struct Brightness pulseEffect(struct Brightness);
struct Brightness fireEffect(struct Brightness);
struct Brightness flashEffect(struct Brightness);

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
uint8_t ProcessRandomAnimation(uint8_t currentAnimation)
{
  switch(currentAnimation) {
    case 0:
      cycleFunctions[0] = 0; // Complete fill
      break;
    case 1:
      cycleFunctions[0] = 1; // Partial fill
      break;
    case 2:
      cycleFunctions[0] = 2; // Black strip fill
      break;
  }
}

#ifndef ANIMATION_FUNCTIONS_H
#define ANIMATION_FUNCTIONS_H

#include <FastLED.h>

extern CRGBPalette16 currentPalette;
extern uint8_t cycleFunctions[3];

uint8_t ProcessRandomAnimation(uint8_t currentAnimation);
struct Brightness pulseEffect(struct Brightness brightness);

#endif