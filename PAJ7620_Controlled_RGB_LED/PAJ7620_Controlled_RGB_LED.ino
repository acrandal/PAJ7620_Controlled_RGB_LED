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

#define TIMEOUT_MILLIS 120000
long long int last_stamp = 0;

enum Mode { ACTIVE, IDLING };
Mode curr_mode = ACTIVE;

int animation = 0;
int duration = 2000;
int palette = 0;

#define LED_COUNT 64
#define LED_PIN A3

#define ALA_BRIGHTNESS 100

#define DEFAULT_ANIMATION  4
#define DEFAULT_DURATION   5000
#define DEFAULT_PALETTE    0

#define ANIMATION_CNT 12
#define PALETTE_CNT 6
#define DURATION_MAX 10000

int animList[ANIMATION_CNT] = {
    ALA_LARSONSCANNER2,
    ALA_PIXELSMOOTHSHIFTRIGHT,
    ALA_COMET,
    ALA_PIXELBOUNCE,
    ALA_SPARKLE2,
    ALA_BUBBLES,
    ALA_PLASMA,
    ALA_PIXELSHIFTRIGHT,
    ALA_COMETCOL,
    ALA_BOUNCINGBALLS,
    ALA_SPARKLE,
    ALA_MOVINGBARS,

    //ALA_ON,
    //ALA_FIRE,
    //ALA_CYCLECOLORS,
    //ALA_GLOW,
    //ALA_FADEINOUT,
    //ALA_FADECOLORS,

};

// Gonzaga University (GU) Color palette
AlaColor GUBlue = 0x0410FF;
AlaColor GURed = 0xF8100E;
AlaColor GUGrey = 0xC1C6C8;
AlaColor alaGUColors_[] =
{
  0xFFFFFF, GUBlue, GUBlue, GUGrey, GUBlue, GURed
};
AlaPalette alaGUColors = {6, alaGUColors_};

AlaPalette paletteList[PALETTE_CNT] = {
  alaGUColors,
  alaPalRgb,
  alaPalRainbow,
  alaPalFire,
  alaPalHeat,
  alaPalCool
};

/*
uint8_t gu_icon[8][8] = 
{{   0, 255, 255,   0,   0,   0,   0,   0},
 { 255,   0,   0, 255,   0,   0,   0,   0},
 { 255,   0,   0,   0,   0,   0,   0,   0},
 { 255,   0, 255, 255, 255, 255,   0, 255},
 { 255,   0,   0, 255,   0, 255,   0, 255},
 {   0, 255, 255,   0,   0, 255,   0, 255},
 {   0,   0,   0,   0,   0, 255,   0, 255},
 {   0,   0,   0,   0,   0,   0, 255,   0}};
 */

// **** ***************************************************************************
void setup()
{
  Serial.begin(115200);

  last_stamp = millis();

  pinMode(LED_BUILTIN, OUTPUT);

  led_blink(1000);

  Serial.println("Starting 7620");
  if( !sensor.begin() )           // return value of 0 == success
  {
    Serial.println("PAJ7620 I2C error - halting");
    while(true) { }
  }
  Serial.println("Done.");

  led_blink(1000);

  delay(5000);

  Serial.print("Proceeding to lighting & processing");
  rgbStrip.initWS2812(LED_COUNT, LED_PIN);
  rgbStrip.setBrightness(AlaColor(ALA_BRIGHTNESS, ALA_BRIGHTNESS, ALA_BRIGHTNESS));
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
  
  // Go into timeout idle mode
  if( ( last_stamp + TIMEOUT_MILLIS < millis() ) && curr_mode == ACTIVE ) {  
    curr_mode = IDLING;
    turnOff();
  }

  rgbStrip.runAnimation();
}


// **** ***************************************************************************
void led_blink(int ms) {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(ms);
  digitalWrite(LED_BUILTIN, LOW);
  delay(ms);
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
  updatePalette();
  //updateAnimation(); 
}

void decrementPalette() {
  palette--;
  palette < 0 ? palette = PALETTE_CNT - 1 : palette = palette;
  updatePalette();
  //updateAnimation();
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
  updateSpeed();
}

void incrementDuration(int step_up) {
  duration += step_up;
  if( duration > DURATION_MAX ) {
    duration = DURATION_MAX;
  }
  updateSpeed();
}

// **** ***************************************************************************
void updatePalette() {
  rgbStrip.setPalette(paletteList[palette % PALETTE_CNT]);
  last_stamp = millis();
  curr_mode = ACTIVE;
}

// **** ***************************************************************************
void updateSpeed() {
  rgbStrip.setSpeed(duration);
  last_stamp = millis();
  curr_mode = ACTIVE;
}

// **** ***************************************************************************
void updateAnimation()
{
  last_stamp = millis();
  curr_mode = ACTIVE;
  rgbStrip.setAnimation(animList[animation % ANIMATION_CNT], duration, paletteList[palette % PALETTE_CNT]);
}
