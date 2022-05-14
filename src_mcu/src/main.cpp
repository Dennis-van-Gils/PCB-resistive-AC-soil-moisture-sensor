/*******************************************************************************
  Dennis van Gils
  14-05-2022
*******************************************************************************/

#include <Arduino.h>

#define Ser Serial
#define PIN_SENSOR 6
#define N_AVG 10
#define TIMEOUT 300 // [ms]

volatile uint8_t isr_loop_counter = 0;
volatile bool isr_rising_has_finished_averaging = false;
volatile uint32_t micros_start = 0;
volatile uint32_t micros_end = 0;

void isr_rising() {
  /* Interrupt service routine activated whenever an up-flank is detected on the
  digital input pin.
  This routine will be executed up to `N_AVG` times in direct succession
  (mechanism to be coded elsewhere).
  */
  if (!isr_rising_has_finished_averaging) {
    if (isr_loop_counter == 0) {
      micros_start = micros(); // Catch start of the first up-flank
    }
    isr_loop_counter++;
    if (isr_loop_counter > N_AVG) {
      micros_end = micros(); // Catch start of the last up-flank
      isr_rising_has_finished_averaging = true;
    }
  }
}

// -----------------------------------------------------------------------------
//    setup
// -----------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  pinMode(PIN_SENSOR, INPUT);

  // 32u4 interrupt pins: 0, 1, 2, 3, 7
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

    isr_loop_counter = 0;
    isr_rising_has_finished_averaging = false;
    while ((!isr_rising_has_finished_averaging) &&
           ((millis() - tick) < TIMEOUT)) {}

    if (isr_rising_has_finished_averaging) {
      Ser.println((uint32_t)1e6 * N_AVG / (micros_end - micros_start));
    } else {
      Ser.println("Timed out");
    }
  }
}