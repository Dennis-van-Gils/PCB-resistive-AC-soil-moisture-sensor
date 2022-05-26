/*******************************************************************************
  Minimum working example to measure a frequency on a digital input pin using
  an interrupt service routine and with averaging applied to the frequency
  measurement.

  Dennis van Gils
  19-05-2022
*******************************************************************************/

#include <Arduino.h>
#include <math.h>

#define Ser Serial

// 32u4 microcontroller has interrupts on digital pins: 0, 1, 2, 3, 7
// SAMD microcontroller has interrupts on all digital pins, except 4
#ifdef _VARIANT_FEATHER_M4_
#  define PIN_SENSOR 5
#else
#  define PIN_SENSOR 7
#endif
#define N_AVG 10    // Number of up-flanks to average over
#define TIMEOUT 300 // [ms] Timeout to stop detecting the frequency

volatile bool isr_done = false;
volatile uint8_t isr_counter = 0;
volatile uint32_t T_upflanks = 0; // [us] Measured duration for N_AVG up-flanks

double frequency = 0;  // [Hz] Measured average frequency
double resistance = 0; // [Ohm] Frequency to resistance best-fit transformation
const double k = -0.61404261;
const double x0 = 1.65479988;
const double b = 0.40044114;
const double a = 3.97842502;

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
      T_upflanks = micros() - micros_start;
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
    frequency = 1000000. * N_AVG / T_upflanks;
    double p = (log10(frequency) - x0) * b;
    double R_log = (k * log(p / (1 - p)) + a);
    resistance = pow(10, R_log);
    if (resistance > 10e6) {
      resistance = INFINITY;
    }
  } else {
    resistance = NAN;
  }

  return isr_done;
}

// -----------------------------------------------------------------------------
//    setup
// -----------------------------------------------------------------------------

void setup() {
  Ser.begin(9600);

  // Set to INPUT_PULLUP even though the sensor itself already has a pull-up
  // resistor onboard. Doing so will prevent floating noise on the input pin
  // leading to false frequencies when the sensor is disconnected.
  pinMode(PIN_SENSOR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_SENSOR), isr_rising, RISING);

  // Light up onboard LED pin 13
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
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
      Ser.print(T_upflanks);
      Ser.print("\t");
      Ser.print(frequency);
      Ser.print("\t");
      Ser.println(resistance);
    } else {
      Ser.println("Timed out");
    }
  }
}