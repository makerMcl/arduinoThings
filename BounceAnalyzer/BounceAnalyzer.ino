/*
 * Copyright 2020 Matthias Clauß
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * This is a jitter analyzator for buttons or switches.
 * Attach a button to test to BUTTON_PIN, press and get its jitter behaviour analyzed.
 * 
 * Records bouncing of buttons/switches from the perspective of the microcontroller and prints the result to Serial (57600 Baud).
 * Button-down and button-up are measured independently of each other since press and release behaviour will be different in most cases.
 * There are also buttons with no bouncing at all, the worst was 95ms from a simple push buttons being pressed slow and soft.
 * 
 * For a more detailed analysis of jitter and debouncing see http://www.ganssle.com/debouncing.pdf
 * 
 * Libraries required:<ul>
 * <li>Streaming by Mikal Hart</li>
 * </ul>
 * 
 * @author <a href="makerMcl@clauss.pro">Matthias Clau&szlig</a>
 */
#define BUTTON_PIN    8
#define BUTTON_ACTIVESTATE LOW
#define INDICATOR_LED 13

#define SHOWJITTER    // activate debug output
#include <Streaming.h>

/** millis for jitter, should be greater than jitter but lower than minimal user button press */
#define JITTER_THRESHOLD 100

const unsigned long ITERATIONS_LIMIT=4294967293; // 2^32-3

void setup() {
  // put your setup code here, to run once:
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(INDICATOR_LED, OUTPUT);
  Serial.begin(57600);
  Serial << "jitter analyzer started" << endl;
  Serial << "ITERATIONS_LIMIT=" <<ITERATIONS_LIMIT << endl;
}

unsigned long startTime=0;                // when first button press detected
unsigned long lastEndTime;                // last detected input pin's state change
word jitterCount=0;              // detected state changes
word measureIterations=0;        // measurement iterations for last button state change
int lastButtonLevel=!BUTTON_ACTIVESTATE;
int lastJitterLevel=lastButtonLevel;

void loop() {
  // take a probe from input pin. Note: anything could have happened at the pin since last iteration
  const unsigned long now=millis(); // TODO try static and see mem difference? 4470 / 574 -> 4530 / 586; approx. 26kHz
  const int inputState=digitalRead(BUTTON_PIN);
  // something changed in input state?
  if (inputState!=lastButtonLevel && (0==startTime)) {
    // measure sequence starting
    if (startTime>0) {
      #ifdef SHOWJITTER
      Serial.println();
      #endif
      Serial << "WARN:  measure aborted, hold/release button for at least " << JITTER_THRESHOLD << " milliseconds" << endl;
    }
    // starting measure sequence
    startTime=now;
    lastEndTime=now;
    measureIterations=0; // to ignore wait time without any activity
    lastButtonLevel=inputState;
    jitterCount=0;
    lastJitterLevel=inputState;
    digitalWrite(INDICATOR_LED, HIGH);
    #ifdef SHOWJITTER
    Serial << "debug: jitter=";
    #endif
  } else if (inputState!=lastJitterLevel) {
    // detected input state change in measure period -> jitter
    lastEndTime=now;
    ++jitterCount;
    #ifdef SHOWJITTER
    Serial << inputState; // for DEBUG: show every detected change at input pin
    #endif
    lastJitterLevel=inputState;
  }
  ++measureIterations;

  const unsigned long elapsedTime=(now-lastEndTime); // sell memory for time
  if (((startTime>0) && (elapsedTime>JITTER_THRESHOLD)) || (measureIterations>ITERATIONS_LIMIT)) {
    #ifdef SHOWJITTER
    Serial.println();
    #endif
    if (inputState!=lastButtonLevel) {
      //sprintf(data, "WARN: unexpected level at sequence end: inputLevel=%d, lastButtonLevel=%d, deltaTime=%d, measureIterations=%d   (%d)", inputState, lastButtonLevel, elapsedTime, measureIterations);
      Serial << "WARN:  unexpected level at sequence end: inputLevel=" << inputState << ", lastButtonLevel=" << lastButtonLevel << ", deltaTime=" << elapsedTime << ", measureIterations=" << measureIterations << endl;
    }
    // end measure sequence
    Serial << "INFO:  Button state change: newState=" << lastButtonLevel << ", jitterCount=" << jitterCount << ", sampleCount=" << measureIterations << 
          " (" << _FLOAT((float)measureIterations/((lastEndTime-startTime)), 3) << " kHz), jitterDuration[ms]=" << (lastEndTime-startTime) << endl;

    // now wait for the next sequence
    startTime=0; jitterCount=0; measureIterations=0;
    digitalWrite(INDICATOR_LED, LOW);
  }
}
