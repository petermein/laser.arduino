#include <Arduino.h>

void disableLasers() {
  for (int i = 0; i < TOTAL_LASERS; i++) {
    digitalWrite(ALL_LASERS[i], HIGH);
  }
}

void enableLasers() {
  for (int i = 0; i < TOTAL_LASERS; i++) {
    digitalWrite(ALL_LASERS[i], LOW);
  }
}

void checkForLaserBreak() {
  for (int i = 0; i < TOTAL_USABLE_DETECTORS; i++) {
    if (digitalRead(USABLE_DETECTORS[i]) == HIGH) {
      Serial.println((String)"Detector " + i + " was broken");
      //Store the laser
      BROKEN_LASER = USABLE_DETECTORS[i];
      setGameState(GAME_FAILED);
      disableLasers();
    }
  }
}

bool determineCollectorState() {
  Serial.println((String)"Reading detectors");
  for (int i = 0; i < TOTAL_DETECTORS; i++) {
    int state = digitalRead(ALL_DETECTORS[i]);

    if (state == LOW) {
      //Detector is usable
      USABLE_DETECTORS[] = ALL_DETECTORS[i];
      TOTAL_USABLE_DETECTORS++;
    } else {
      Serial.println((String)"Detector " + i + " is not working");
    }

  }
  Serial.println((String)"Found " + TOTAL_USABLE_DETECTORS + " usable detectors of " + TOTAL_DETECTORS);

  if (TOTAL_USABLE_DETECTORS < DETECTOR_THRESHOLD) {
    Serial.println("Game could not start, not enough detectors");
  }else{
    matchLasers();
  }

  return true;
}

void matchLasers() {
  Serial.println((String)"Matching detectors");
  disableLasers();
  for (int i = 0; i < TOTAL_USABLE_DETECTORS; i++) {
    for (int y = 0; y < TOTAL_LASERS; y++) {
       digitalWrite(ALL_LASERS[y], LOW);
       delay(1000);
       int state = digitalRead(USABLE_DETECTORS[i]);
       if (state == LOW) {
         DETECTOR_MATCHING[USABLE_DETECTORS[i]] = ALL_LASERS[y];
         Serial.println((String)"Detector " + USABLE_DETECTORS[i] + " is matched with laser "+ ALL_LASERS[y]);
       }
       digitalWrite(ALL_LASERS[y], HIGH);
    }
  }
}

void laserDiscoMode() {
  Serial.println((String)"Disco Disco");
  int setLasers[DISCO_MODE_AMOUNT_LASERS];
  for (int i = 0; i < DISCO_MODE_AMOUNT_LASERS; i++) {
    int rand = random(1, TOTAL_LASERS + 1) - 1;
    digitalWrite(ALL_LASERS[rand], LOW);
    Serial.println((String)ALL_LASERS[rand]);
    setLasers[i] = rand;
  }
  delay(DISCO_MODE_DELAY);
  disableLasers();
}

void setupLaserPins() {
  //Set laser pins to output mode
  for (int i = 0; i < TOTAL_LASERS; i++) {
    pinMode(ALL_LASERS[i], OUTPUT);
  }
  for (int i = 0; i < TOTAL_DETECTORS; i++) {
    pinMode(ALL_DETECTORS[i], INPUT);
  }
}
