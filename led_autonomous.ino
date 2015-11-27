#include <avr/io.h>
#define F_CPU 16000000UL // we need to define cpu frequency, it's 16MHz for ATmega328
#include <util/delay.h>
#include <Arduino.h>
const int LED_PIN = 9; // Pin the transistor is connected to // pin 9 (PB1) with OCR1A
const int PIR_PIN = 2; // Pin the PIR motion sensor is connected to
const int LDR_PIN = 3; // Pin the LDR light sensor is connected to

const int LIGHT_DELAY_MS = 10000; // time for which the light shall stay on when a motion is detected
const int FADE_DELAY_MS = 10; // _delay_ms at each step of fading

const byte ON_VALUE = 255;
const byte OFF_VALUE = 0;

int current_brightness = 0;
unsigned long light_on_ms = 0L;

void setup() {
    Serial.begin(9600);

    // pin direction definitions
    pinMode(PIR_PIN, INPUT);
    pinMode(LDR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    // turn LEDs "off"
    fade(OFF_VALUE);
}

void loop(){
    // turn the light on if the PIR shows a motion
    if(digitalRead(PIR_PIN) == 1) {
        fade(ON_VALUE);
        light_on_ms = millis();
    } else if(millis() - light_on_ms < LIGHT_DELAY_MS) {
        Serial.println("waiting");
    }
    if(millis() - light_on_ms >= LIGHT_DELAY_MS) {
        fade(OFF_VALUE);
    }

    _delay_ms(100);
}


void fade(int target_brightness_level) {
    if(target_brightness_level == current_brightness) {
        return;
    } else if(target_brightness_level < current_brightness) { // fade down
        while(current_brightness > target_brightness_level) {
            analogWrite(LED_PIN, --current_brightness); // increase current_brightness and apply to register
            Serial.println(current_brightness);
            _delay_ms(FADE_DELAY_MS);
        }
    } else if(target_brightness_level > current_brightness) { // fade up
        while(current_brightness < target_brightness_level) {
            analogWrite(LED_PIN, ++current_brightness); // increase current_brightness and apply to register
            Serial.println(current_brightness);
            _delay_ms(FADE_DELAY_MS);
        }
    }
}
