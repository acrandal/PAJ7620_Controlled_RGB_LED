#include "AlaLedRgb.h"

#include "RevEng_PAJ7620.h"

RevEng_PAJ7620 sensor = RevEng_PAJ7620();

AlaLedRgb rgbStrip;

int animation = 7;
int duration = 2000;
int palette = 0;

#define ANIMATION_CNT 14
#define PALETTE_CNT 3
#define DURATION_MAX 10000

int animList[14] = {
    ALA_ON,
    ALA_SPARKLE,
    ALA_SPARKLE2,
    ALA_PIXELSHIFTRIGHT,
    ALA_PIXELSMOOTHSHIFTRIGHT,
    ALA_MOVINGBARS,
    ALA_COMET,
    ALA_COMETCOL,
    ALA_GLOW,
    ALA_CYCLECOLORS,
    ALA_FADECOLORS,
    ALA_FIRE,
    ALA_BOUNCINGBALLS,
    ALA_BUBBLES
};

AlaPalette paletteList[3] = {
    alaPalRgb,
    alaPalRainbow,
    alaPalFire
};

int durationList[4] = {
    1000,
    2000,
    5000,
    10000
};


void setup()
{
  Serial.begin(115200);

  if( !sensor.begin() )           // return value of 0 == success
  {
    Serial.print("PAJ7620 I2C error - halting");
    while(true) { }
  }

  rgbStrip.initWS2812(37, 8);
  updateAnimation();
}


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


  }
  rgbStrip.runAnimation();
}

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



void updateAnimation()
{
    //rgbStrip.setAnimation(animList[animation%14], durationList[duration%4], paletteList[palette%3]);
  rgbStrip.setAnimation(animList[animation%14], duration, paletteList[palette%3]);

}
