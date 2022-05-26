/*******************************************************************************
  Dennis van Gils
  26-05-2022
*******************************************************************************/

#include <Arduino.h>

#include "pins_arduino.h"
#include "wiring_private.h"

// Alternatively: Try compiling new custom assembly code based on 'pulseIn()`
// myself. See:
// https://garretlab.web.fc2.com/en/arduino/inside/hardware/arduino/avr/cores/arduino/wiring_pulse.S/countPulseASM.html
// C:\Users\vangi\.platformio\packages\framework-arduino-avr\cores\arduino\wiring_pulse.S
// C:\Users\vangi\.platformio\packages\framework-arduino-avr\cores\arduino\wiring_pulse.c

uint32_t dvg_countPulseASM(volatile uint8_t *port, uint8_t bit,
                           uint8_t stateMask, unsigned long maxloops) {
  unsigned long width = 0;

  // wait for any previous pulse to end
  while ((*port & bit) == stateMask)
    if (--maxloops == 0)
      return 0;

  // wait for the pulse to start
  while ((*port & bit) != stateMask)
    if (--maxloops == 0)
      return 0;

  for (uint8_t i = 10; i > 0; i--) {
    // wait for the pulse to stop
    while ((*port & bit) == stateMask) {
      if (++width == maxloops)
        return 0;
    }

    // wait for the pulse to start once more
    while ((*port & bit) != stateMask)
      if (++width == maxloops)
        return 0;
  }

  return width;
}

/* Measures the length (in microseconds) of a pulse on the pin; state is HIGH
 * or LOW, the type of pulse to measure.  Works on pulses from 2-3
 * microseconds to 3 minutes in length, but must be called at least a few
 * dozen microseconds before the start of the pulse.
 *
 * This function performs better with short pulses in noInterrupt() context
 */
unsigned long dvg_pulseIn(uint8_t pin, uint8_t state, unsigned long timeout) {
  // cache the port and bit of the pin in order to speed up the
  // pulse width measuring loop and achieve finer resolution.  calling
  // digitalRead() instead yields much coarser resolution.
  uint8_t bit = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  uint8_t stateMask = (state ? bit : 0);

  // convert the timeout from microseconds to a number of times through
  // the initial loop; it takes approximately 16 clock cycles per iteration
  unsigned long maxloops = microsecondsToClockCycles(timeout) / 16;

  unsigned long width =
      dvg_countPulseASM(portInputRegister(port), bit, stateMask, maxloops);

  // prevent clockCyclesToMicroseconds to return bogus values if countPulseASM
  // timed out
  if (width)
    return clockCyclesToMicroseconds(width * 16 + 16);
  else
    return 0;
}
