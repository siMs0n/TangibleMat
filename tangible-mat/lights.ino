#include <FastLED.h>

// fastled led strip controls go here
// 1 channel for everything

#define NUM_LEDS 32
#define LED_STRIP_PIN 3 // better use a PWM enabled pin
//note for the D1 mini: "All of the IO pins have interrupt/pwm/I2C/one-wire support except D0."
//#define LED_STRIP_CLOCK_PIN 8 // clock pin for SPI based chipsets

#define LED_BASE_HUE CRGB::Gold //base colour being used
#define LED_BRIGHTNESS 16 // goes up to 256 i think?
// note that neopixels can get pretty bright, and high brightness
// takes more power. or do we want this to be a var instead?

// define indices here maybe? for buttons etc. or in main file
// for meter, define first index and length (no of pixels)
// (meters assumed to be something that "fills up" from bottom)
#define LED_BUTTON_1 0
#define LED_BUTTON_2 1
#define LED_BUTTON_3 2
#define LED_BUTTON_4 3
#define LED_BUTTON_5 4
#define LED_METER_1 5
#define LED_METER_1_LEN 10


// THIS IS THE ARRAY WHERE THE LEDS GO
CRGB leds[NUM_LEDS];

void initLedStrips() {
    // sanity check delay - allows reprogramming if accidently blowing power w/leds
    // delay(2000);

    FastLED.addLeds<NEOPIXEL, LED_STRIP_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(LED_BRIGHTNESS);
    
}

//animates the led in a cool way, maybe for turning on or something
void ledCoolAnimate() {
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, LED_BASE_HUE, 7);
}

void ledSwitchButton(int index, bool turning_on) {
  CRGB colour = turning_on ? LED_BASE_HUE : CRGB::Black;
  leds[index] = colour;
  FastLED.show();
}

//set the meter turned on length as specified
// it takes a float range from 0 to 1 (0.5 is 50%)
void ledSetMeter(int first_index, int meter_len, float value) {
  // do float arithmetics, but cast to int so it's floored
  int max_index = first_index + (int)(value * (float) meter_len);
  
  // set the leds
  for (int i = first_index; i < (first_index + meter_len); i++) {
    if (i < max_index) {
      leds[i] = LED_BASE_HUE;
    }
    else { //turn off if oob
      leds[i] = CRGB::Black;
    }
  }
  FastLED.show();
}
