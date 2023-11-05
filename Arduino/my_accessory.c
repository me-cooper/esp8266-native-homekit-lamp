#include <homekit/homekit.h>
#include <homekit/characteristics.h>

void my_accessory_identify(homekit_value_t _value) {
  printf("accessory identify\n");
} 

/* NORMAL LIGHTBULD SETTINGS */
    homekit_characteristic_t cha_on                 = HOMEKIT_CHARACTERISTIC_(ON, false);
    homekit_characteristic_t cha_bright             = HOMEKIT_CHARACTERISTIC_(BRIGHTNESS, 0);
    homekit_characteristic_t cha_sat                = HOMEKIT_CHARACTERISTIC_(SATURATION, (float) 0);
    homekit_characteristic_t cha_hue                = HOMEKIT_CHARACTERISTIC_(HUE, (float) 180);


/* ANIMATION SWITCH SETTINGS */
  homekit_characteristic_t cha_switch_on            = HOMEKIT_CHARACTERISTIC_(ON, false);
  
  

homekit_accessory_t *accessories[] = {
  
    HOMEKIT_ACCESSORY(.id=1, .category=homekit_accessory_category_bridge, .services=(homekit_service_t*[]) {

        // IN DIESEM FALL IST DAS DIE BRIDGE, DIE ALLE ACCESSORIES ENTHHÄLT
    
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
            HOMEKIT_CHARACTERISTIC(NAME, "makesmart Animation Lamp"),
            HOMEKIT_CHARACTERISTIC(MANUFACTURER, "makesmart Community"),
            HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "0123456"),
            HOMEKIT_CHARACTERISTIC(MODEL, "ESP8266"),
            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "1.0"),
            HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
            NULL
        }),

        NULL
    }),

        // ACCESSPORIES

          // LAMP
          HOMEKIT_ACCESSORY(.id=2, .category=homekit_accessory_category_lightbulb, .services=(homekit_service_t*[]) {
  
            HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
              HOMEKIT_CHARACTERISTIC(NAME, "UFO Lampe"),
              HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
              NULL
            }),
  
            HOMEKIT_SERVICE(LIGHTBULB, .primary=true, .characteristics=(homekit_characteristic_t*[]) {
              &cha_on,
              &cha_bright,
              &cha_sat,
              &cha_hue,
              NULL
            }),  

            NULL
  
          }),




          // SWITCH
          HOMEKIT_ACCESSORY(.id=3, .category=homekit_accessory_category_switch, .services=(homekit_service_t*[]) {
  
            HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
              HOMEKIT_CHARACTERISTIC(NAME, "UFO Animation"),
              HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
              NULL
            }),
  
            HOMEKIT_SERVICE(SWITCH, .primary=true, .characteristics=(homekit_characteristic_t*[]){
              &cha_switch_on,
              NULL
            }),  

            NULL
  
          }),

       
  NULL
};


homekit_server_config_t accessory_config = {
    .accessories = accessories,
    .password = "123-45-678"
};
