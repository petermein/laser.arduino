#include <Arduino.h>

void disableLasers() {
  for (int i = 0; i < TOTAL_LASERS; i++) {
    digitalWrite(ALL_LASERS[i], LOW);
  }
}

void enableLasers() {
  for (int i = 0; i < TOTAL_LASERS; i++) {
    digitalWrite(ALL_LASERS[i], HIGH);
  }
}

void checkForLaserBreak() {
  for (int i = 0; i < TOTAL_USABLE_DETECTORS; i++) {
    if (digitalRead(USABLE_DETECTORS[i]) == LOW) {
      //Laser was broken!
      //Store the laser
      BROKEN_LASER = USABLE_DETECTORS[i];
      setGameState(GAME_FAILED);
      disableLasers();
    }
  }
}

bool determineCollectorState() {
  Serial.println((String)"Error correcting on detectors");
  for (int i = 0; i < TOTAL_DETECTORS; i++) {
    int state = digitalRead(ALL_DETECTORS[i]);

    if (state == HIGH) {
      //Detector is usable
      USABLE_DETECTORS[i] = ALL_DETECTORS[i];
      TOTAL_USABLE_DETECTORS++;
    } else {
      Serial.println((String)"Detector " + i + " is not working");
    }

  }
  Serial.println((String)"Found " + TOTAL_USABLE_DETECTORS + " usable detectors of " + TOTAL_DETECTORS);

  if (TOTAL_USABLE_DETECTORS < DETECTOR_THRESHOLD) {
    Serial.println("Game could not start, not enough detectors");
//        throwError(30);
//        return false;
  }

  return true;
}

void setupLaserPins() {
  //Set laser pins to output mode
  for (int i = 0; i < TOTAL_LASERS; i++) {
    pinMode(ALL_LASERS[i], OUTPUT);
  }

  for (int i = 0; i < TOTAL_DETECTORS; i++) {
    pinMode(ALL_DETECTORS[i], INPUT);
  }

  pinMode(WIN_BUTTON, INPUT);

  //Pinmodes for display
  //Pinmodes for serial rx/tx
}
