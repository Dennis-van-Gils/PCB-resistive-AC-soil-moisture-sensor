/*******************************************************************************
  Dennis van Gils
  13-05-2022
*******************************************************************************/

#include <Arduino.h>

#define Ser Serial
#define PIN_SENSOR 6
#define PIN_SENSOR_2 5
#define N_AVG 10

volatile int pwm_value = 0;
volatile int prev_time = 0;

void rising() {
  Ser.println(micros() - prev_time);
  prev_time = micros();
}

// -----------------------------------------------------------------------------
//    setup
// -----------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  pinMode(PIN_SENSOR, INPUT);

  // attachInterrupt(PIN_SENSOR_2, rising, RISING);
}

// -----------------------------------------------------------------------------
//    loop
// -----------------------------------------------------------------------------

void loop() {
  char char_cmd; // Incoming serial command
  uint32_t now = millis();
  static uint32_t tick = now;
  double avg_pulse_hi;
  double avg_pulse_lo;

  if (now - tick > 500) {
    tick = now;

    avg_pulse_hi = 0;
    avg_pulse_lo = 0;
    for (uint8_t i = 0; i < N_AVG; i++) {
      avg_pulse_hi += pulseIn(PIN_SENSOR, HIGH);
      // avg_pulse_lo += pulseIn(PIN_SENSOR, LOW);
    }
    for (uint8_t i = 0; i < N_AVG; i++) {
      // avg_pulse_hi += pulseIn(PIN_SENSOR, HIGH);
      avg_pulse_lo += pulseIn(PIN_SENSOR, LOW);
    }
    avg_pulse_hi /= N_AVG;
    avg_pulse_lo /= N_AVG;

    Ser.print(millis() - tick);
    Ser.print("\t");
    Ser.print(avg_pulse_hi, 1);
    Ser.print("\t");
    Ser.print(avg_pulse_lo, 1);
    Ser.print("\t");
    Ser.print(5e5 / avg_pulse_hi, 0);
    Ser.print("\t");
    Ser.print(5e5 / avg_pulse_lo, 0);
    Ser.print("\t");
    Ser.println(1e6 / (avg_pulse_hi + avg_pulse_lo), 0);
  }

  // Check for incoming serial commands
  if (Ser.available() > 0) {
    char_cmd = Ser.read();

    if (char_cmd == '?') {
      Ser.println("Resistive AC soil moisture sensor demo");
    }
  }
}