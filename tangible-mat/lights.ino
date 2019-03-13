#include <FastLED.h>

// fastled led strip controls go here
// 1 channel for everything

#define NUM_LEDS 28 // 0 to 27
#define LED_STRIP_PIN D8 // better use a PWM enabled pin
//note for the D1 mini: "All of the IO pins have interrupt/pwm/I2C/one-wire support except D0."
//#define LED_STRIP_CLOCK_PIN 8 // clock pin for SPI based chipsets

#define LED_BASE_HUE CRGB::Gold //base colour being used
#define LED_BRIGHTNESS 64 // goes up to 256 i think?
// note that neopixels can get pretty bright, and high brightness
// takes more power. or do we want this to be a var instead?

// define indices like this, then plug into functions.
// /!!!\ ACTUALLY we need to declare these in main file /!!!\
// for meter, define first index and length (no of pixels)
// (meters assumed to be something that "fills up" from bottom, like volume)
// negative length goes backwards in the array (still starting from index)
#define LED_BUTTON_1 0
#define LED_BUTTON_2 1
#define LED_BUTTON_3 26
#define LED_BUTTON_4 27
#define LED_METER_1 2
#define LED_METER_1_LEN 12
#define LED_METER_2 25
#define LED_METER_2_LEN -12



// THIS IS THE ARRAY WHERE THE LEDS GO
CRGB leds[NUM_LEDS];

void initLedStrips() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(500);

  FastLED.addLeds<NEOPIXEL, LED_STRIP_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(LED_BRIGHTNESS);
  FastLED.clear(); //sanity check clear

}

//animates the led in a cool way, maybe for turning on or something
void ledCoolAnimate() {
  // FastLED's built-in rainbow generator
  Serial.println("Animate leds");
  fill_rainbow( leds, NUM_LEDS, LED_BASE_HUE, 7);
}

void ledSwitchButton(int index, bool turning_on) {
  CRGB colour = turning_on ? LED_BASE_HUE : CRGB::Black;
  leds[index] = colour;
  FastLED.show();
}

void setledMeters(int value) {
  ledSetMeter(25, -12, value / 100);
  ledSetMeter(2, 12, value / 100);
}

//set the meter turned on length as specified
// it takes a float range from 0 to 1 (0.5 is 50%)
void ledSetMeter(int first_index, int meter_len, float value) {
  // do float arithmetics, but cast to int so it's floored
  // should be fine even for negative values right???
  int max_index = first_index + (int)(value * (float) meter_len);

  // set the leds
  if (meter_len < 0) { //negative length = backwards
    for (int i = first_index; i < (first_index + meter_len); i--) {
      if (i < max_index) {
        leds[i] = LED_BASE_HUE;
      }
      else { //turn off if oob
        leds[i] = CRGB::Black;
      }
    }
  }
  else { //normal forwards way
    for (int i = first_index; i < (first_index + meter_len); i++) {
      if (i < max_index) {
        leds[i] = LED_BASE_HUE;
      }
      else { //turn off if oob
        leds[i] = CRGB::Black;
      }
    }
  }
  FastLED.show();
}

// wrapper for clearing all leds back to black
void ledClear() {
  FastLED.clear();
}
