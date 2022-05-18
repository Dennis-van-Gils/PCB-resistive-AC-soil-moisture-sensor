/*******************************************************************************
  Minimum working example to measure a frequency on a digital input pin using
  an interrupt service routine and with averaging applied to the frequency
  measurement.

  Dennis van Gils
  18-05-2022
*******************************************************************************/

#include <Arduino.h>
#include <math.h>

#define Ser Serial

// 32u4 microcontroller has interrupts on digital pins: 0, 1, 2, 3, 7
// SAMD microcontroller has interrupts on all digital pins, except 4
#define PIN_SENSOR 5
#define N_AVG 10    // Number of up-flanks to average over
#define TIMEOUT 300 // [ms] Timeout to stop detecting the frequency

volatile bool isr_done = false;
volatile uint8_t isr_counter = 0;
volatile uint16_t frequency = 0; // [Hz] Measured average frequency

float resistance = 0; // [Ohm] Frequency to resistance best-fit transformation
const float k = -0.55295297;
const float x0 = 1.6795268;
const float b = 0.40662999;
const float a = 3.98202594;

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
  uint32_t t_0 = millis();
  isr_counter = 0;
  isr_done = false;
  while ((!isr_done) && ((millis() - t_0) < TIMEOUT)) {}

  if (isr_done) {
    // Frequency to resistance best-fit transformation (is monotonic, yeay :))
    // Resistance fit +/-20 % (at worst) wrt as measured during calibration,
    // good enough for order-of-10s estimation
    float p = (log10(frequency) - x0) * b;
    float R_log = (k * log(p / (1 - p)) + a);
    resistance = pow(10, R_log);

  } else {
    resistance = NAN;
  }

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
      Ser.print(frequency);
      Ser.print("\t");
      Ser.println(resistance);
    } else {
      Ser.println("Timed out");
    }
  }
}