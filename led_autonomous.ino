const int LED_PIN = 9; // Pin the transistor is connected to // pin 9 (PB1) with OCR1A
const int PIR_PIN = 2; // Pin the PIR motion sensor is connected to
const int LDR_PIN = A0; // Pin the LDR light sensor is connected to

const long LIGHT_DELAY_MS = 1000 /* 60000L*/; // time for which the light shall stay on when a motion is detected (millis)
const long FADE_DELAY_MS = 5L; // delay at each step of fading
const int BRIGHTNESS_THRESHOLD = 100; // threshold for brightness (if sensor value read is <= threshold, light will be on)

const byte ON_VALUE = 31;
const byte OFF_VALUE = 0;

int current_brightness = 0;
unsigned long light_on_ms = 0L;

const uint8_t pwm_table[32] = {0, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 8, 9, 11, 13, 16, 19, 23, 27, 32, 38, 45, 54, 64, 76, 90, 107, 128, 152, 180, 214, 255};

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
  int brightness_ldr = analogRead(LDR_PIN);
  Serial.print("brightness:");
  Serial.println(brightness_ldr);
  if(brightness_ldr <= BRIGHTNESS_THRESHOLD || current_brightness == ON_VALUE) { // if ambient brightness low OR already on
    // turn the light on if the PIR shows a motion
    if(digitalRead(PIR_PIN) == 1) { // if motion detected
      Serial.println("Motion -> light on");
        fade(ON_VALUE); // turn on
      light_on_ms = millis(); // save time the lights were turned on
    }
  }
  // check if wait time is over
  if(millis() - light_on_ms >= LIGHT_DELAY_MS) {
     Serial.println("light off");
     fade(OFF_VALUE);// turn off
  }
  delay(100);
}


void fade(int target_brightness_level) {
  if(target_brightness_level == current_brightness) {
      return;
  } else if(target_brightness_level < current_brightness) { // fade down
      while(current_brightness > target_brightness_level) {
          analogWrite(LED_PIN, pwm_table[--current_brightness]); // increase current_brightness and apply to register
          Serial.println(pwm_table[current_brightness]);
          delay(FADE_DELAY_MS);
      }
  } else if(target_brightness_level > current_brightness) { // fade up
      while(current_brightness < target_brightness_level) {
          analogWrite(LED_PIN, pwm_table[++current_brightness]); // increase current_brightness and apply to register
          Serial.println(pwm_table[current_brightness]);
          delay(FADE_DELAY_MS);
      }
  }
}
