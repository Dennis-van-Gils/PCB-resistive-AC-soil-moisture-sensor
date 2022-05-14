/*******************************************************************************
  Dennis van Gils
  14-05-2022
*******************************************************************************/

#include <Arduino.h>

#define Ser Serial

#define PIN_SENSOR 6
#define N_AVG 10
#define TIMEOUT 300 // [ms]

volatile bool isr_done = false;
volatile uint8_t isr_counter = 0;
volatile uint32_t micros_period = 0;

void isr_rising() {
  /* Interrupt service routine activated whenever an up-flank is detected on the
  digital input pin.
  This routine will be executed up to `N_AVG` times in direct succession
  (mechanism to be coded elsewhere).
  */
  static uint32_t micros_start = 0;

  if (!isr_done) {
    if (isr_counter == 0) {
      micros_start = micros();
    }
    isr_counter++;
    if (isr_counter > N_AVG) {
      micros_period = micros() - micros_start;
      isr_done = true;
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

    isr_counter = 0;
    isr_done = false;
    while ((!isr_done) && ((millis() - tick) < TIMEOUT)) {}

    if (isr_done) {
      Ser.println((uint32_t)1e6 * N_AVG / micros_period);
    } else {
      Ser.println("Timed out");
    }
  }
}