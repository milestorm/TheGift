#include <Arduino.h>
#include <avdweb_VirtualDelay.h>
#include <OneButton.h>
#include <config.h>

// TODO
// add some piezo buzzer for annoying melody? :)
// what to do with the rocker switch (no fuction at the moment)

VirtualDelay whiteDelay;
VirtualDelay redBreatheDelay;

bool switch_state[2] = {false, false};
int switch_pin[2] = {PIN_SWITCH_1, PIN_SWITCH_2};

unsigned long white_on_time = 30 * 1000; // 30 seconds
unsigned long white_on_time_previous = 0;
bool turn_white_on = false;
bool want_turn_white_on = false;
bool white_turned_on = false;

bool ir_detected = true;

bool led_red_breathing_active = true;
bool led_red_breathing_up = true;
int led_red_breathing_value = 0;
int led_red_breathing_delay = 15;


// detects IR module obstacle
bool detect_ir(bool analog_mode = false) {
  bool ir_on = false;
  if (analog_mode == true) {
    if (analogRead(PIN_IR_ANALOG) <= 835) { // magic constant
      ir_on = false;
    } else {
      ir_on = true;
    }
  } else {
    if (digitalRead(PIN_IR) == HIGH) {
      ir_on = true;
    } else {
      ir_on = false;
    }
  }
  return ir_on;
}

// fades out white led (duh...)
void led_white_fadeout(int delay_time) {
  for (int i = 256 - 1; i >= 0; i--) {
    analogWrite(PIN_W, i);
    delay(delay_time);
  }
  digitalWrite(PIN_W, LOW);  
}

// fades in white led
void led_white_fadein(int delay_time) {
  for (int i = 0; i < 255; i++) {
    analogWrite(PIN_W, i);
    delay(delay_time);
  }
  digitalWrite(PIN_W, LOW);  
}

// white led turn on blinking effect
void led_white_turnon_effect() {
  digitalWrite(PIN_W, HIGH);
  delay(40);
  digitalWrite(PIN_W, LOW);
  delay(100);
  digitalWrite(PIN_W, HIGH);
  delay(60);
  digitalWrite(PIN_W, LOW);
  delay(40);
  digitalWrite(PIN_W, HIGH);
  delay(40);
  digitalWrite(PIN_W, LOW);
  delay(80);
  digitalWrite(PIN_W, HIGH);
  delay(180);
  digitalWrite(PIN_W, LOW);
  delay(40);
  led_white_fadein(2);
  digitalWrite(PIN_W, HIGH);
}

// determinates what side of switch is on
void switch_tick() {
  for (int i = 0; i < 2; i++) {
    switch_state[i] = digitalRead(switch_pin[i]);
  }
}

// delay tick function for white led
void white_led_tick() {
  if (want_turn_white_on == true) {
    if (white_turned_on == false) {
      white_turned_on = true;
      led_white_turnon_effect();
    } else {
      digitalWrite(PIN_W, HIGH);
    }
    // digitalWrite(PIN_W, HIGH);    
    whiteDelay.start(white_on_time);
    if (whiteDelay.elapsed()) {
        //digitalWrite(PIN_W, LOW);
        white_turned_on = false;
        want_turn_white_on = false;
        led_white_fadeout(5);
    }
  }
}

// red led breathing
void led_red_breathe() {
  if (led_red_breathing_active == true) {
    analogWrite(PIN_R, led_red_breathing_value);
    redBreatheDelay.start(led_red_breathing_delay);
    if (redBreatheDelay.elapsed()) {
      if (led_red_breathing_up == true) {
        led_red_breathing_value += 1;
        if (led_red_breathing_value >= 255) {
          led_red_breathing_up = false;
        }
        
      } else {
        led_red_breathing_value -= 1;
        if (led_red_breathing_value <= 0) {
          led_red_breathing_up = true;
        }
      }
      
    }
  } else {
    digitalWrite(PIN_R, LOW);
  }
   
}

void setup() {
  Serial.begin(9600);

  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_W, OUTPUT);

  pinMode(PIN_IR, INPUT);

  pinMode(PIN_SWITCH_1, INPUT);
  pinMode(PIN_SWITCH_2, INPUT);

  digitalWrite(PIN_G, LOW);
  digitalWrite(PIN_W, LOW);
}

void loop() {
  // detect obstacle (car key) insertion into the box
  if ((detect_ir(true) == true) && (ir_detected == false)) {
    // key NOT inserted
    ir_detected = true;
    digitalWrite(PIN_R, HIGH);
    digitalWrite(PIN_G, LOW);
    want_turn_white_on = true;
    led_red_breathing_active = true;
  }
  if ((detect_ir(true) == false) && (ir_detected == true)) {
    // key inserted
    digitalWrite(PIN_R, LOW);
    digitalWrite(PIN_G, HIGH);
    ir_detected = false;
    want_turn_white_on = true;
    led_red_breathing_active = false;
  }

  // toggle switch at the bottom of the case
  if (switch_state[1] == true) {
    // do something
  }
  if (switch_state[0] == true) {
    // do something else
  } 

  Serial.println(analogRead(PIN_IR_ANALOG));
  
  // delay ticks
  white_led_tick();
  led_red_breathe();
  switch_tick();
}
