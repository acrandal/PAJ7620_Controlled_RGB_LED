/**
 *  PAJ7620 Controlled RGB LEDs
 * 
 *  @author Aaron S. Crandall <crandall@gonzaga.edu>
 *  @copyright 2020
 *  
 *  @license MIT
 * 
 */

#include "AlaLedRgb.h"
#include "RevEng_PAJ7620.h"

bool g_LEDEnable = true;

RevEng_PAJ7620 sensor = RevEng_PAJ7620();

AlaLedRgb rgbStrip;

int animation = 0;
int duration = 2000;
int palette = 0;

#define DEFAULT_ANIMATION  2
#define DEFAULT_DURATION   5000
#define DEFAULT_PALETTE    0

#define ANIMATION_CNT 11
#define PALETTE_CNT 3
#define DURATION_MAX 10000

int animList[ANIMATION_CNT] = {
    //ALA_ON,
    ALA_LARSONSCANNER2,
    ALA_FADEINOUT,
    ALA_PLASMA,
    //ALA_PIXELSHIFTRIGHT,
    ALA_PIXELSMOOTHSHIFTRIGHT,
    //ALA_COMET,
    ALA_COMETCOL,
    ALA_PIXELBOUNCE,
    ALA_GLOW,
    ALA_CYCLECOLORS,
    //ALA_FADECOLORS,
    //ALA_FIRE,
    //ALA_BOUNCINGBALLS,
    //ALA_SPARKLE,
    ALA_SPARKLE2,
    ALA_MOVINGBARS,
    ALA_BUBBLES
};

AlaPalette paletteList[PALETTE_CNT] = {
    alaPalRgb,
    alaPalRainbow,
    alaPalFire
};


// **** ***************************************************************************
void setup()
{
  Serial.begin(115200);

  if( !sensor.begin() )           // return value of 0 == success
  {
    Serial.print("PAJ7620 I2C error - halting");
    while(true) { }
  }

  rgbStrip.initWS2812(37, 8);
  rgbStrip.setBrightness(AlaColor(100, 100, 100));
  setALADefaults();
  updateAnimation();
}


// **** ***************************************************************************
void loop()
{
  Gesture gesture;                  // Gesture is an enum type from RevEng_PAJ7620.h
  gesture = sensor.readGesture();   // Read back current gesture (if any) of type Gesture

  switch (gesture)
  {
    // Control animation with left & right
    //case GES_LEFT:    animation--; animation < 0 ? animation = ANIMATION_CNT - 1 : animation = animation; updateAnimation(); break;
    case GES_LEFT:    decrementAnimation(); break;
    case GES_RIGHT:   incrementAnimation(); break;

    case GES_UP:    incrementPalette(); break;
    case GES_DOWN:  decrementPalette(); break;

    // Control animation speed
    case GES_CLOCKWISE: decrementDuration(1000); break;
    case GES_ANTICLOCKWISE: incrementDuration(1000); break;

    // Toggle lights on/off
    case GES_FORWARD:
      g_LEDEnable = !g_LEDEnable;
      if( g_LEDEnable ) {
        turnOn();
      } else {
        turnOff();
      }
      break;

    case GES_WAVE:
      if( sensor.getWaveCount() > 5 ) {
        setALADefaults();
        updateAnimation();
      }
      break;

  }
  
  rgbStrip.runAnimation();
}


// **** ***************************************************************************
void setALADefaults() {
  animation = DEFAULT_ANIMATION;
  duration = DEFAULT_DURATION;
  palette = DEFAULT_PALETTE;
}

void turnOff() {
  rgbStrip.setAnimation(ALA_OFF, 500, paletteList[palette%3]);  
}

void turnOn() {
  updateAnimation();  
}


// **** ***************************************************************************
void incrementPalette() {
  palette++;
  palette >= PALETTE_CNT ? palette = 0 : palette = palette;
  updateAnimation(); 
}

void decrementPalette() {
  palette--;
  palette < 0 ? palette = PALETTE_CNT - 1 : palette = palette;
  updateAnimation();
}

void decrementAnimation() {
  animation--;
  animation < 0 ? animation = ANIMATION_CNT - 1 : animation = animation;
  updateAnimation();
}

void incrementAnimation() {
  animation++;
  animation >= ANIMATION_CNT ? animation = 0 : animation = animation;
  updateAnimation();
}

void decrementDuration(int step_down) {
  duration -= step_down;
  if( duration < 500 ) 
    { duration = 500; }
  updateAnimation();
}

void incrementDuration(int step_up) {
  duration += step_up;
  Serial.println(duration);
  updateAnimation();
}


// **** ***************************************************************************
void updateAnimation()
{
  rgbStrip.setAnimation(animList[animation % ANIMATION_CNT], duration, paletteList[palette % PALETTE_CNT]);
}
