#include <Arduino.h>
#include <avdweb_VirtualDelay.h>
#include <OneButton.h>
#include <config.h>

// TODO
// udelat dejchaci ledky do sluplicku
// pri vlozeni udelat efekt bilyho svetlaa jakoze zarivka zablikani
// pri vylozeni podobny ale kratsi

VirtualDelay whiteDelay;

OneButton switch_1(PIN_SWITCH_1, true);
OneButton switch_2(PIN_SWITCH_2, true);

unsigned long white_on_time = 30 * 1000; // 30 seconds
unsigned long white_on_time_previous = 0;
bool turn_white_on = false;
bool want_turn_white_on = false;

bool ir_detected = true;

bool detect_ir() {
  bool ir_on = false;
  if (digitalRead(PIN_IR) == HIGH) {
    ir_on = true;
  } else {
    ir_on = false;
  }

  return ir_on;
}

void white_led_tick() {
  if (want_turn_white_on == true) {
    digitalWrite(PIN_W, HIGH);
    whiteDelay.start(white_on_time);
    if (whiteDelay.elapsed()) {
        digitalWrite(PIN_W, LOW);
        want_turn_white_on = false;
    }
  }
}

void setup() {
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_W, OUTPUT);

  pinMode(PIN_IR, INPUT);

  // initial turn on
  digitalWrite(PIN_R, HIGH);
  digitalWrite(PIN_G, LOW);

  digitalWrite(PIN_W, LOW);
}

void loop() {
  if ((detect_ir() == true) && (ir_detected == false)) {
    ir_detected = true;
    digitalWrite(PIN_R, HIGH);
    digitalWrite(PIN_G, LOW);
    want_turn_white_on = true;
  }

  if ((detect_ir() == false) && (ir_detected == true)) {
    digitalWrite(PIN_R, LOW);
    digitalWrite(PIN_G, HIGH);
    ir_detected = false;
    want_turn_white_on = true;
  }

  white_led_tick();
}