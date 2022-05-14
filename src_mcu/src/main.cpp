/*******************************************************************************
  Minimum working example to measure a frequency on a digital input pin using
  an interrupt service routine and with averaging applied to the frequency
  measurement.

  Dennis van Gils
  14-05-2022
*******************************************************************************/

#include <Arduino.h>

#define Ser Serial

// 32u4 microcontroller has interrupts on digital pins: 0, 1, 2, 3, 7
// SAMD microcontroller has interrupts on all digital pins, except 4
#define PIN_SENSOR 7
#define N_AVG 10    // Number of up-flanks to average over
#define TIMEOUT 300 // [ms] Timeout to stop detecting the frequency

volatile bool isr_done = false;
volatile uint8_t isr_counter = 0;
volatile uint16_t frequency = 0; // [Hz] Measured average frequency

void isr_rising() {
  /* Interrupt service routine for when an up-flank is detected on the input pin
   */
  static uint32_t micros_start = 0;

  if (!isr_done) {
    if (isr_counter == 0) {
      micros_start = micros();
    }
    isr_counter++;
    if (isr_counter > N_AVG) {
      frequency = 1000000 * N_AVG / (micros() - micros_start);
      isr_done = true;
    }
  }
}

bool measure_frequency() {
  static uint32_t t_0 = millis();
  isr_counter = 0;
  isr_done = false;
  while ((!isr_done) && ((millis() - t_0) < TIMEOUT)) {}
  return isr_done;
}

// -----------------------------------------------------------------------------
//    setup
// -----------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);

  // Set to INPUT_PULLUP even though the sensor itself already has a pull-up
  // resistor onboard. Doing so will prevent floating noise on the input pin
  // leading to false frequencies when the sensor is disconnected.
  pinMode(PIN_SENSOR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_SENSOR), isr_rising, RISING);
}

// -----------------------------------------------------------------------------
//    loop
// -----------------------------------------------------------------------------

void loop() {
  uint32_t now = millis();
  static uint32_t tick = now;

  if (now - tick > 500) {
    tick = now;

    if (measure_frequency()) {
      Ser.println(frequency);
    } else {
      Ser.println("Timed out");
    }
  }
}