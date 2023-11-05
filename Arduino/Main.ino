#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <FastLED.h>

#include <arduino_homekit_server.h>

#include "wifi_info.h"

#define LOG_D(fmt, ...)       printf_P(PSTR(fmt "\n") , ##__VA_ARGS__);

#define NEOPIN                D4
#define NUMPIXELS             43
Adafruit_NeoPixel pixels      =   Adafruit_NeoPixel(NUMPIXELS, NEOPIN, NEO_GRB + NEO_KHZ800);

#define MAX_POWER_MILLIAMPS   500
#define LED_TYPE              WS2812B
#define COLOR_ORDER           GRB

CRGB leds[NUMPIXELS];

bool received_sat = false;
bool received_hue = false;

bool is_on                = false;
float current_brightness  = 0.0;
float current_sat         = 0.0;
float current_hue         = 0.0;
int rgb_colors[3];

bool animation_switch_is_on = false;

void setup() {
  Serial.begin(115200);
  

  pixels.begin(); 

  FastLED.addLeds<LED_TYPE,NEOPIN,COLOR_ORDER>(leds, NUMPIXELS)
        .setCorrection( TypicalLEDStrip );
  FastLED.setMaxPowerInVoltsAndMilliamps( 5, MAX_POWER_MILLIAMPS);
  
  for(int i = 0; i < NUMPIXELS; i++)
  
  {
    pixels.setPixelColor(i, 0, 0, 0);
  }
  
  pixels.show();
  
  delay(1000);
  
  rgb_colors[0] = 255;
  rgb_colors[1] = 255;
  rgb_colors[2] = 255;


  wifi_connect();


  //homekit_storage_reset();

  
  my_homekit_setup();
  
}


CRGBPalette16 pacifica_palette_1 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50 };
CRGBPalette16 pacifica_palette_2 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F };
CRGBPalette16 pacifica_palette_3 = 
    { 0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33, 
      0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF };


void pacifica_loop()
{
  // Increment the four "color index start" counters, one for each wave layer.
  // Each is incremented at a different speed, and the speeds vary over time.
  static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
  static uint32_t sLastms = 0;
  uint32_t ms = GET_MILLIS();
  uint32_t deltams = ms - sLastms;
  sLastms = ms;
  uint16_t speedfactor1 = beatsin16(3, 179, 269);
  uint16_t speedfactor2 = beatsin16(4, 179, 269);
  uint32_t deltams1 = (deltams * speedfactor1) / 256;
  uint32_t deltams2 = (deltams * speedfactor2) / 256;
  uint32_t deltams21 = (deltams1 + deltams2) / 2;
  sCIStart1 += (deltams1 * beatsin88(1011,10,13));
  sCIStart2 -= (deltams21 * beatsin88(777,8,11));
  sCIStart3 -= (deltams1 * beatsin88(501,5,7));
  sCIStart4 -= (deltams2 * beatsin88(257,4,6));

  // Clear out the LED array to a dim background blue-green
  fill_solid( leds, NUMPIXELS, CRGB( 2, 6, 10));

  // Render each of four layers, with different scales and speeds, that vary over time
  pacifica_one_layer( pacifica_palette_1, sCIStart1, beatsin16( 3, 11 * 256, 14 * 256), beatsin8( 10, 70, 130), 0-beat16( 301) );
  pacifica_one_layer( pacifica_palette_2, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401) );
  pacifica_one_layer( pacifica_palette_3, sCIStart3, 6 * 256, beatsin8( 9, 10,38), 0-beat16(503));
  pacifica_one_layer( pacifica_palette_3, sCIStart4, 5 * 256, beatsin8( 8, 10,28), beat16(601));

  // Add brighter 'whitecaps' where the waves lines up more
  pacifica_add_whitecaps();

  // Deepen the blues and greens a bit
  pacifica_deepen_colors();
}

// Add one layer of waves into the led array
void pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff)
{
  uint16_t ci = cistart;
  uint16_t waveangle = ioff;
  uint16_t wavescale_half = (wavescale / 2) + 20;
  for( uint16_t i = 0; i < NUMPIXELS; i++) {
    waveangle += 250;
    uint16_t s16 = sin16( waveangle ) + 32768;
    uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
    ci += cs;
    uint16_t sindex16 = sin16( ci) + 32768;
    uint8_t sindex8 = scale16( sindex16, 240);
    CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
    leds[i] += c;
  }
}

// Add extra 'white' to areas where the four layers of light have lined up brightly
void pacifica_add_whitecaps()
{
  uint8_t basethreshold = beatsin8( 9, 55, 65);
  uint8_t wave = beat8( 7 );
  
  for( uint16_t i = 0; i < NUMPIXELS; i++) {
    uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
    wave += 7;
    uint8_t l = leds[i].getAverageLight();
    if( l > threshold) {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8( overage, overage);
      leds[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
    }
  }
}

// Deepen the blues and greens
void pacifica_deepen_colors()
{
  for( uint16_t i = 0; i < NUMPIXELS; i++) {
    leds[i].blue = scale8( leds[i].blue,  145); 
    leds[i].green= scale8( leds[i].green, 200); 
    leds[i] |= CRGB( 2, 5, 7);
  }
}

      

void loop() {
  my_homekit_loop();
  delay(10);


  if(animation_switch_is_on){
    EVERY_N_MILLISECONDS( 20) {
      pacifica_loop();
      FastLED.show();
    }
  }
  
}

//==============================
// HomeKit setup and loop
//==============================

// Access HomeKit characteristics defined in my_accessory.c
extern "C" homekit_server_config_t accessory_config;

// LIGHTBULB
extern "C" homekit_characteristic_t cha_on;
extern "C" homekit_characteristic_t cha_bright;
extern "C" homekit_characteristic_t cha_sat;
extern "C" homekit_characteristic_t cha_hue;

// SWITCH
extern "C" homekit_characteristic_t cha_switch_on;

static uint32_t next_heap_millis = 0;


bool animationIsRunning = false;

//Called when the switch value is changed by iOS Home APP
void cha_switch_on_setter(const homekit_value_t value) {
  bool on = value.bool_value;
  cha_switch_on.value.bool_value = on;  //sync the value
  LOG_D("Switch: %s", on ? "ON" : "OFF");

  animation_switch_is_on = value.bool_value;


  if(animation_switch_is_on){
    // TURN OFF LAMP
    cha_on.value.bool_value = false;
    homekit_characteristic_notify(&cha_on, cha_on.value);
  }

  if(!animation_switch_is_on){
    is_on = false;
    updateColor();
  }
  
 
}



void my_homekit_setup() {

  // LIGHTBULD
  cha_on.setter       = set_on;
  cha_bright.setter   = set_bright;
  cha_sat.setter      = set_sat;
  cha_hue.setter      = set_hue;

  // SWITCH
  cha_switch_on.setter = cha_switch_on_setter;
  
  arduino_homekit_setup(&accessory_config);

}

void my_homekit_loop() {
  arduino_homekit_loop();
  const uint32_t t = millis();
  if (t > next_heap_millis) {
    // show heap info every 5 seconds
    next_heap_millis = t + 5 * 1000;
    LOG_D("Free heap: %d, HomeKit clients: %d",
        ESP.getFreeHeap(), arduino_homekit_connected_clients_count());

  }
}

void set_on(const homekit_value_t v) {
    bool on = v.bool_value;
    cha_on.value.bool_value = on; //sync the value

    if(on) {
        is_on = true;
        Serial.println("On");
    } else  {
        is_on = false;
        Serial.println("Off");
    }

    
    updateColor();
    
    
}

void set_hue(const homekit_value_t v) {
    Serial.println("set_hue");
    float hue = v.float_value;
    cha_hue.value.float_value = hue; //sync the value

    current_hue = hue;
    received_hue = true;
    
    updateColor();
}

void set_sat(const homekit_value_t v) {
    Serial.println("set_sat");
    float sat = v.float_value;
    cha_sat.value.float_value = sat; //sync the value

    current_sat = sat;
    received_sat = true;
    
    updateColor();

}

void set_bright(const homekit_value_t v) {
    Serial.println("set_bright");
    int bright = v.int_value;
    cha_bright.value.int_value = bright; //sync the value

    current_brightness = bright;

    updateColor();
}

void updateColor()
{
  if(is_on)
  {

      if(animation_switch_is_on){
        animation_switch_is_on = false; 
        cha_switch_on.value.bool_value = animation_switch_is_on;
        homekit_characteristic_notify(&cha_switch_on, cha_switch_on.value);
      }
   
      if(received_hue && received_sat)
      {
        HSV2RGB(current_hue, current_sat, current_brightness);
        received_hue = false;
        received_sat = false;
      }
      
      int b = map(current_brightness,0, 100,75, 255);
      Serial.println(b);
  
      pixels.setBrightness(b);
      for(int i = 0; i < NUMPIXELS; i++)
      {
  
        pixels.setPixelColor(i, pixels.Color(rgb_colors[0],rgb_colors[1],rgb_colors[2]));
  
      }

    
      
      pixels.show();

    }
  else if(!is_on) //lamp - switch to off
  {
      Serial.println("is_on == false");

      if(animation_switch_is_on){
        animation_switch_is_on = false; 
        cha_switch_on.value.bool_value = animation_switch_is_on;
        homekit_characteristic_notify(&cha_switch_on, cha_switch_on.value);
      }
      
      pixels.setBrightness(0);
      for(int i = 0; i < NUMPIXELS; i++)
      {
        pixels.setPixelColor(i, pixels.Color(0,0,0));
      }
      pixels.show();
  }
}

void HSV2RGB(float h,float s,float v) {

  int i;
  float m, n, f;

  s/=100;
  v/=100;

  if(s==0){
    rgb_colors[0]=rgb_colors[1]=rgb_colors[2]=round(v*255);
    return;
  }

  h/=60;
  i=floor(h);
  f=h-i;

  if(!(i&1)){
    f=1-f;
  }

  m=v*(1-s);
  n=v*(1-s*f);

  switch (i) {

    case 0: case 6:
      rgb_colors[0]=round(v*255);
      rgb_colors[1]=round(n*255);
      rgb_colors[2]=round(m*255);
    break;

    case 1:
      rgb_colors[0]=round(n*255);
      rgb_colors[1]=round(v*255);
      rgb_colors[2]=round(m*255);
    break;

    case 2:
      rgb_colors[0]=round(m*255);
      rgb_colors[1]=round(v*255);
      rgb_colors[2]=round(n*255);
    break;

    case 3:
      rgb_colors[0]=round(m*255);
      rgb_colors[1]=round(n*255);
      rgb_colors[2]=round(v*255);
    break;

    case 4:
      rgb_colors[0]=round(n*255);
      rgb_colors[1]=round(m*255);
      rgb_colors[2]=round(v*255);
    break;

    case 5:
      rgb_colors[0]=round(v*255);
      rgb_colors[1]=round(m*255);
      rgb_colors[2]=round(n*255);
    break;
  }
}
