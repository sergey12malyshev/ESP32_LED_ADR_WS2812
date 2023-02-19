/******************************************************************************/
/* Test project based on FastLED library and DemoReel100.ino project          */
/* Malyshev S.E.                                                              */
/******************************************************************************/
#include <Arduino.h>
#include <FastLED.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define DATA_PIN           13
#define LED_TYPE           WS2812
#define COLOR_ORDER        GRB
#define NUM_LEDS           300
#define VOLTAGE             5
#define CURRENT            1900

const uint8_t brightness = 96u;
const uint8_t framesPerSecond = 120u;
const uint8_t voltage_v = 5u;
const uint8_t current_mA = 120u;

CRGB leds[NUM_LEDS];
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

static void rainbowWithGlitter(void);
static void bpm(void);
static void rainbow(void);
static void confetti(void);
static void sinelon(void);
static void juggle(void);
static void staticRandomColor(void);

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, staticRandomColor};

static void nextPattern(void)
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

static void rainbow(void)
{// FastLED's built-in rainbow generator
  const uint8_t deltahue = 3;
  fill_rainbow(leds, NUM_LEDS, gHue, deltahue);
}

static void addGlitter(uint8_t chanceOfGlitter)
{
  if( random8() < chanceOfGlitter) 
  {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

static void confetti(void)
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

static void sinelon(void)
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1);
  leds[pos] += CHSV( gHue, 255, 192);
}

static void bpm(void)
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 30;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);

  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

static void juggle(void) 
{
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
static void rainbowWithGlitter(void)
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

static void staticRandomColor(void)
{
  static uint8_t devider;
  if (devider == 0)
  {
   fill_solid(leds, NUM_LEDS, CRGB(random8(),random8(),random8()));
  }
  devider++;
}

void setup() 
{
  // tell FastLED about the LED strip configuration
  delay(250);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setMaxPowerInVoltsAndMilliamps(voltage_v, 1900);
  FastLED.setBrightness(brightness);
}

void loop()
{
  gPatterns[gCurrentPatternNumber]();
  FastLED.show();
  FastLED.delay(1000/framesPerSecond);

  /*do some periodic updates */
  EVERY_N_MILLISECONDS(20) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS(10) { nextPattern(); } // change patterns periodically
}