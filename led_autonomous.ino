const int LED_PIN = 9; // Pin the transistor is connected to // pin 9 (PB1) with OCR1A
const int PIR_PIN_1 = 2; // Pin the 1st PIR motion sensor is connected to
const int PIR_PIN_2 = 3; // Pin the 2nd PIR motion sensor is connected to
const int LDR_PIN = 3; // Pin the LDR light sensor is connected to

const long LIGHT_DELAY_MS = 5 * 60000L; // time for which the light shall stay on when a motion is detected (millis)
const long FADE_DELAY_MS = 5L; // delay at each step of fading

const byte ON_VALUE = 255;
const byte OFF_VALUE = 0;

int current_brightness = 0;
unsigned long light_on_ms = 0L;
//const uint8_t pwm_table[32] = {0, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 8, 9, 11, 13, 16, 19, 23, 27, 32, 38, 45, 54, 64, 76, 90, 107, 128, 152, 180, 214, };
const uint16_t pwm_table[256] =
{
  0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3,
  3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7,
  7, 7, 8, 8, 8, 9, 9, 10, 10, 10, 11, 11, 12, 12, 13, 13, 14, 15,
  15, 16, 17, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
  31, 32, 33, 35, 36, 38, 40, 41, 43, 45, 47, 49, 52, 54, 56, 59,
  61, 64, 67, 70, 73, 76, 79, 83, 87, 91, 95, 99, 103, 108, 112,
  117, 123, 128, 134, 140, 146, 152, 159, 166, 173, 181, 189, 197,
  206, 215, 225, 235, 245, 256, 267, 279, 292, 304, 318, 332, 347,
  362, 378, 395, 412, 431, 450, 470, 490, 512, 535, 558, 583, 609,
  636, 664, 693, 724, 756, 790, 825, 861, 899, 939, 981, 1024, 1069,
  1117, 1166, 1218, 1272, 1328, 1387, 1448, 1512, 1579, 1649, 1722,
  1798, 1878, 1961, 2048, 2139, 2233, 2332, 2435, 2543, 2656, 2773,
  2896, 3025, 3158, 3298, 3444, 3597, 3756, 3922, 4096, 4277, 4467,
  4664, 4871, 5087, 5312, 5547, 5793, 6049, 6317, 6596, 6889, 7194,
  7512, 7845, 8192, 8555, 8933, 9329, 9742, 10173, 10624, 11094,
  11585, 12098, 12634, 13193, 13777, 14387, 15024, 15689, 16384,
  17109, 17867, 18658, 19484, 20346, 21247, 22188, 23170, 24196,
  25267, 26386, 27554, 28774, 30048, 31378, 32768, 34218, 35733,
  37315, 38967, 40693, 42494, 44376, 46340, 48392, 50534, 52772,
  55108, 57548, 60096, 62757, 65535
};

void setup() {
    //Serial.begin(9600);

    //HWPWM INIT
    //for more info see atmega 328P datasheet around page 134

    //first we'll reset both time control registers because the arduino init() function
    //might have changed it
    TCCR1B = 0;
    TCCR1A = 0;
    //now we'll set to fast pwm mode with icr1 as TOP
    TCCR1A |= _BV(WGM11);
    TCCR1B |= _BV(WGM12);
    TCCR1B |= _BV(WGM13);
    //some other settings
    TCCR1B |= _BV(CS10); // no prescaler
    ICR1 |= 0xFFFF; // set timers to use 16bit (count to 0xFFFF)
    TCCR1A |= _BV(COM1A1); // connect pin 9 to timer 1 channel A
    TCCR1A |= _BV(COM1B1); // connect pin 10 to timer 1 channel B
    Serial.begin(9600);

    DDRB |= 0b000000110;  //set pin 9 and 10 as output

    // pin direction definitions
    pinMode(PIR_PIN_1, INPUT);
    pinMode(PIR_PIN_2, INPUT);
    pinMode(LDR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    // turn LEDs "off"
    fade(OFF_VALUE);
}

void loop(){
    // turn the light on if the PIR shows a motion
    if(digitalRead(PIR_PIN_1) == 1 || digitalRead(PIR_PIN_2)) {
        fade(ON_VALUE);
        light_on_ms = millis();
    } else if(millis() - light_on_ms < LIGHT_DELAY_MS) {
        Serial.print("waiting ");
        Serial.println(LIGHT_DELAY_MS - (millis() - light_on_ms));
    }
    if(millis() - light_on_ms >= LIGHT_DELAY_MS) {
        fade(OFF_VALUE);
    }

    delay(100);
}


void fade(int target_brightness_level) {
    if(target_brightness_level == current_brightness) {
        return;
    } else if(target_brightness_level < current_brightness) { // fade down
        while(current_brightness > target_brightness_level) {
            OCR1A = pwm_table[--current_brightness]; // increase current_brightness and apply to register
            Serial.println(OCR1A);
            delay(FADE_DELAY_MS);
        }
    } else if(target_brightness_level > current_brightness) { // fade up
        while(current_brightness < target_brightness_level) {
            OCR1A = pwm_table[++current_brightness]; // increase current_brightness and apply to register
            Serial.println(OCR1A);
            delay(FADE_DELAY_MS);
        }
    }
}
