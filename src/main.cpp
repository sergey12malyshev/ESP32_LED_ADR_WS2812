/******************************************************************************/
/* Test project based on FastLED library and DemoReel100.ino project 
/* Added control via Telegramm bot
/* Malyshev S.E.                                                              
/******************************************************************************/
#include <Arduino.h>
#include <FastLED.h>
#include <FastBot.h>
#include <pass.h>   //The file contains WiFi login/password + bot token/id


#define SOFTWARE_VERSION       "1.0.3"       
#define DEBUG    false         //Enable debug messages

#define RUS      true         //Enable english messages
#define EN       false        //Enable russian messages

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define DATA_PIN           13
#define LED_TYPE           WS2812
#define COLOR_ORDER        GRB
#define NUM_LEDS           300

#define UPDATE_PERIOD_BOT  6500

const uint8_t brightness = 96u;
const uint8_t framesPerSecond = 120u;
const uint8_t voltage_v = 5u;
const uint16_t current_mA = 1900u;

typedef enum {NOWORK = 0, WORK, PATTERN} modes;
modes mainModes = WORK;
FastBot bot(BOT_TOKEN);

CRGB leds[NUM_LEDS];
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

bool flagStartEff = false;

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

static inline void disableLEDs(void) // Выключение
{
  fill_solid(leds, NUM_LEDS, CRGB::Black);
}

static inline void AquaShowColor(void) //Enable
{
  fill_solid(leds, NUM_LEDS, CRGB::Aqua);
  FastLED.show();
}

static inline void GreenShowColor(void) // Wi-Fi good
{
  fill_solid(leds, NUM_LEDS, CRGB::Green);
  FastLED.show();
}

static inline void RedShowColor(void) // Wi-Fi Error
{
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  FastLED.show();
}

static inline void PurpleShowColor(void) //Upload
{
  fill_solid(leds, NUM_LEDS, CRGB::Purple);
  FastLED.show();
}

static void nextPattern(void)
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

static void rainbow(void)
{// FastLED's built-in rainbow generator
  const uint8_t deltahue = 3;
  fill_rainbow(leds, NUM_LEDS, gHue, deltahue);

  flagStartEff = true;
}

static void addGlitter(uint8_t chanceOfGlitter)
{
  if(random8() < chanceOfGlitter) 
  {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

static void confetti(void)
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

static void sinelon(void)
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1);
  leds[pos] += CHSV( gHue, 255, 192);
}

static void bpm(void)
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 30;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);

  for(int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

static void juggle(void) 
{
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  byte dothue = 0;
  for(int i = 0; i < 8; i++) 
  {
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
  if (flagStartEff)
  {
    fill_solid(leds, NUM_LEDS, CHSV(random8(), 255, 255));
    flagStartEff = false;
  }
}

static void setupFastLedLib() 
{
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setMaxPowerInVoltsAndMilliamps(voltage_v, current_mA);
  FastLED.setBrightness(brightness);
}

static void ledProcess()
{
  gPatterns[gCurrentPatternNumber]();
  FastLED.show();
  FastLED.delay(1000/framesPerSecond);

  //do some periodic updates
  EVERY_N_MILLISECONDS(25) // slowly cycle the "base color" through the rainbow
  { 
    gHue++; 
  } 
}

static void loopFastLed()
{
  ledProcess();
  EVERY_N_SECONDS(12) // change patterns periodically
  {
    nextPattern(); 
  } 
}

// telegram massage handler
static void newMsg(FB_msg& msg) 
{
#if DEBUG  
  // выводим ID чата, имя юзера и текст сообщения
  Serial.print(msg.chatID);     // ID чата 
  Serial.print(", ");
  Serial.print(msg.username);   // логин
  Serial.print(", ");
  Serial.println(msg.text);     // текст
#endif

/* Start the update process if an update message has arrived from the chat with the correct id */
  if (msg.OTA && msg.text == "update" && msg.chatID == CHAT_ID) 
  {
    bot.sendMessage("Software update...", msg.chatID);
    PurpleShowColor();
    delay(500);
    bot.update();
  }
  else if (msg.text == "/get_soft@LedLigthBot")
  {
    bot.sendMessage("Software version: ", msg.chatID);
    bot.sendMessage(SOFTWARE_VERSION, msg.chatID);
    bot.sendMessage("\r\n");
    bot.sendMessage(__DATE__, msg.chatID);
    bot.sendMessage(__TIME__, msg.chatID);
  } 
  else if (msg.text == "/led_on@LedLigthBot") 
  {
    mainModes = WORK;
    bot.sendMessage("LED enabled!", msg.chatID);
  } 
  else if (msg.text == "/led_off@LedLigthBot") 
  {
    mainModes = NOWORK;
    FastLED.clear(true);
    bot.sendMessage("LED disabled!", msg.chatID);
  }
  else if (msg.text == "/next@LedLigthBot") 
  {
    mainModes = PATTERN;
    bot.sendMessage("next pattern", msg.chatID);
    nextPattern();
  }
  else if (msg.text == "/help@LedLigthBot")
  {
#if RUS
    bot.sendMessage("Я умею:\r\n\
    next - сменить режим\r\n\
    led_on - включить ленту\r\n\
    led_off - выключить ленту\r\n\
    get_soft - запросить версию\r\n\
    hardreset - перезапуск платы\r\n\
    update + .bin - обновить ПО\r\n\
    help - доступные команды", msg.chatID);
#elif EN
    bot.sendMessage("I can:\r\n\
    next - change the mode\r\n\
    led_on - enable the feed\r\n\
    led_off - turn off the feed\r\n\
    get_soft - request version\r\n\
    hard reset - restarting the board\r\n\
    update + .bin - update software\r\n\
    help - available commands", msg.chatID);
#else
    #error "No language selected"
#endif

  }
  else if (msg.text == "/hardreset@LedLigthBot")
  {
#if RUS
    bot.sendMessage("ПЕРЕЗАГРУЖАЮСЬ...", msg.chatID);  
#elif EN
    bot.sendMessage("REBOOTING...", msg.chatID);  
#endif                        
    delay(1000);                                                               
    bot.tickManual();
    ESP.restart();                                                              
  }
  else
  {
#if RUS
    bot.sendMessage("я не знаю такой команды...", msg.chatID);  
#elif EN
    bot.sendMessage("I do not know such a command...", msg.chatID);  
#endif   
  }
}

static void connectWiFi() 
{
  delay(1100);
  Serial.begin(115200);
  Serial.println();
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(150);
    Serial.print(".");
    if (millis() > 15000)
    {
      RedShowColor();
      delay(150);
      ESP.restart();
    } 
  }
  GreenShowColor();
  Serial.println("Connected");
}

void setup() 
{
  delay(50);
  setupFastLedLib();
  AquaShowColor();

  connectWiFi();
  bot.setPeriod(UPDATE_PERIOD_BOT);
  bot.attach(newMsg);

  bot.setChatID(CHAT_ID);

#if RUS
  bot.sendMessage("Я в сети!");
#elif EN
  bot.sendMessage("I'm online!");
#endif
}

void loop() 
{
  bot.tick();

  switch (mainModes)
  {
    case WORK:
      loopFastLed();
      break;
    case PATTERN:
      ledProcess();
      break;
    default:
      break;
  }
}