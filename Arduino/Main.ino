/* ############################### ~ cooper.bin @ makesmart.net ~ ############################### */
/* ---------------------------------------------------------------------------------------------- */
/*                                                                                                */
/*                                   General Arduino Libraries                                    */
/*                                                                                                */
/* ---------------------------------------------------------------------------------------------- */
/* ############################################################################################## */



#include <Arduino.h>
#include <FastLED.h>
#include <arduino_homekit_server.h>



/* ############################### ~ cooper.bin @ makesmart.net ~ ############################### */
/* ---------------------------------------------------------------------------------------------- */
/*                                                                                                */
/*                                      Local Project Files                                       */
/*                                                                                                */
/* ---------------------------------------------------------------------------------------------- */
/* ############################################################################################## */



#include "wifi_info.h"



/* ############################### ~ cooper.bin @ makesmart.net ~ ############################### */
/* ---------------------------------------------------------------------------------------------- */
/*                                                                                                */
/*                                        FastLED Settings                                        */
/*                                                                                                */
/* ---------------------------------------------------------------------------------------------- */
/* ############################################################################################## */



#define   NUM_LEDS      43
#define   DATA_PIN      D4

#define   LED_TYPE      WS2812B
#define   COLOR_ORDER   GRB       // look `void setup()` @ https://github.com/FastLED/FastLED/blob/master/examples/Blink/Blink.ino

CRGB      leds[NUM_LEDS];



/* ############################################################################################## */
/* ---------------------------------------------------------------------------------------------- */
/*                                                                                                */
/*                                   Global Lightbulb Variables                                   */
/*                                                                                                */
/* ---------------------------------------------------------------------------------------------- */
/* ############################################################################################## */



bool  lightbulbIsOn     = false;
int   currentHue        = 0;
int   currentSat        = 0;
int   currentBright     = 255;




void updateLightbulb();

/* ############################################################################################## */
/* ---------------------------------------------------------------------------------------------- */
/*                                                                                                */
/*                                        setup() Function                                        */
/*                                                                                                */
/* ---------------------------------------------------------------------------------------------- */
/* ############################################################################################## */


void setup() {
  
  Serial.begin(115200);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip  );
  
  updateLightbulb();

  wifi_connect();
 
  my_homekit_setup();
     
}


     
void loop() {

  EVERY_N_MILLISECONDS( 10 ) {
    homekit_loop();
  }  
  
}





/* ############################################################################################## */
/* ---------------------------------------------------------------------------------------------- */
/*                                                                                                */
/*                                           Functions                                            */
/*                                                                                                */
/* ---------------------------------------------------------------------------------------------- */
/* ############################################################################################## */

void updateLightbulb() {
  //Serial.println("() => updateLightbulb()");

  int updateHue     = currentHue;
  int updateSat     = currentSat;
  int updateBright  = lightbulbIsOn ? currentBright : 0;

 
  for( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(updateHue, updateSat, updateBright); 
    FastLED.show();
  }


}


int mapValue(int value, int maxValue = 100) {
  return map(value, 0, maxValue, 0, 255);
}





/* ############################################################################################## */
/* ---------------------------------------------------------------------------------------------- */
/*                                                                                                */
/*                       Homekit characteristics defined in my_accessory.c                        */
/*                                                                                                */
/* ---------------------------------------------------------------------------------------------- */
/* ############################################################################################## */


extern "C" homekit_server_config_t    accessory_config;

// LIGHTBULB
extern "C" homekit_characteristic_t   lightbulb_on;
extern "C" homekit_characteristic_t   lightbulb_bright;
extern "C" homekit_characteristic_t   lightbulb_sat;
extern "C" homekit_characteristic_t   lightbulb_hue;




/* ############################################################################################## */
/* ---------------------------------------------------------------------------------------------- */
/*                                                                                                */
/*                                      Lightbulb Functions                                       */
/*                                                                                                */
/* ---------------------------------------------------------------------------------------------- */
/* ############################################################################################## */


void function_lightbulb_on(const homekit_value_t v) {
    bool isOn                       = v.bool_value;
    lightbulb_on.value.bool_value   = isOn;

    lightbulbIsOn                   = isOn ? true : false;

    //Serial.println("LIGHTBULB ON / OFF " + String(lightbulbIsOn));
    updateLightbulb();

}


void function_lightbulb_bright(const homekit_value_t v) {
  
    // Value from 0 -100
    int bright                        = v.int_value;
    lightbulb_bright.value.int_value  = bright;

    // Value from 0 - 255
    int rgbBright                     = mapValue(bright);
    currentBright                     = rgbBright;

    //Serial.println("LIGHTBULB BRIGHT " + String(bright) + " " + String(rgbBright));
    updateLightbulb();
    
}


void function_lightbulb_sat(const homekit_value_t v) {
    
    // Value from 0 - 100
    int sat                         = v.float_value;
    lightbulb_sat.value.float_value = sat;

    // Value from 0 - 255
    int rgbSat                      = mapValue(sat);
    currentSat                      = rgbSat; 
   
    //Serial.println("LIGHTBULB SAT " + String(sat));
    updateLightbulb();

}


void function_lightbulb_hue(const homekit_value_t v) {
    // Value from 0 - 360
    int hue                           = v.float_value;
    lightbulb_hue.value.float_value   = hue; 
    

    // Value from 0 - 255
    int rgbHue                      = mapValue(hue, 360);
    currentHue                      = rgbHue; 

    //Serial.println("LIGHTBULB HUE " + String(hue));
    updateLightbulb();

}




/* ############################################################################################## */
/*                            Attach Lightbulb functions to accessory                             */
/* ############################################################################################## */

void my_homekit_setup() {
  
  lightbulb_on.setter       = function_lightbulb_on;
  lightbulb_bright.setter   = function_lightbulb_bright;
  lightbulb_sat.setter      = function_lightbulb_sat;
  lightbulb_hue.setter      = function_lightbulb_hue;
  
  arduino_homekit_setup(&accessory_config);

}


void homekit_loop() {  
  arduino_homekit_loop();
}
