# esp8266-native-homekit-lamp
Eine ESP8266 RGB-LED Lampe mit Adressierbaren LEDs. Nativ für Apple's Homekit.

Hatte ursprünglich das Beispiel von: [Example05_WS2812_Neopixel]( https://github.com/Mixiaoxiao/Arduino-HomeKit-ESP8266/tree/master/examples/Example05_WS2812_Neopixel) am laufen. Nur ließen sich die LED's nicht richtig dimmen. Und die Farben waren auch etwas verschoben. Also habe ich das ganze auf FastLED umgebaut. Werde schauen, dass ich das Example mit meinem Code update. Denn der ursprüngliche Code ist etwas Crap aus genannten Gründen...

---

*Stand: 06.11.2023* 



### Was macht dieser Code? 

Er verwandelt euren ESP8266 in eine Native HomeKit RGB-Lampe. Ich Nutze dafür WS2812B LED-Ringe aus dem  (mein Shop aber geile Produkte). Keine Werbung, da ich da echt die Ringe verwendet habe. LOL [RGB makesmart.shop](https://makesmart.shop/search?search=RGB)

### Verdrahtung

DATA PIN LEDS: **D4**

Oder anpassen in `Main.ino`, je nachdem welchen Pin ihr verwendet:

```arduino
#define   NUM_LEDS      43
#define   DATA_PIN      D4

#define   LED_TYPE      WS2812B
#define   COLOR_ORDER   GRB       // look `void setup()` @ https://github.com/FastLED/FastLED/blob/master/examples/Blink/Blink.ino

CRGB      leds[NUM_LEDS];
```



### Anpassungen

Ihr müsst die Anzahl der LEDs in `Main.ino` ebenfalls anpassen:

```arduino
#define   NUM_LEDS      43
```



Die Wlan-Einstellungen werden in `wifi_info.h` angepasst:

```arudino
const char *ssid = "WLAN-SSID";
const char *password = "WLAN-PSK";
```



### Bibliotheken

- [Arduino-HomeKit-ESP8266](https://github.com/Mixiaoxiao/Arduino-HomeKit-ESP8266)
- [FastLED](https://github.com/FastLED/FastLED)



## Upload

alls ihr auch Probleme beim Pairing habt, ist es ratsam den Flash des ESP's komplett zu löschen und alles neu hochzuladen, das geht in den Werkzeugen:

`Erase Flash -> All Flash Content`

Des Weiteren benötigtESP8266 HomeKit sehr viel Leistung, da verschlüsselt. Übertaktet den ESP8266, um Ausfälle zu vermeiden. Das geht ebenfalls über die Werkzeuge:

`CPU Frequency -> 160 MHz`  



### Pairing



Der ESP8266 fungiert als Bridge und stellt 1 Accessoire bereit - eine Lampe.

Eigentlich kann man das auch direkt als Device ohne Bridge einbinden, aber die Bridge ermöglicht es neben der Lampe zusätzliche Komponenten unter einem ESP einbinden zu können. Ein Beispiel dazu findet ihr hier: [Example04_MultipleAccessories](https://github.com/Mixiaoxiao/Arduino-HomeKit-ESP8266/tree/master/examples/Example04_MultipleAccessories)

Namen können in `my_accessory.c` geändert werden. 

```arudino
HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
	HOMEKIT_CHARACTERISTIC(NAME, "UFO Lampe"),
	HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
	NULL
}),
```



Oder einfach beim Pairen mit der Home App.



---



Have fun.

 
