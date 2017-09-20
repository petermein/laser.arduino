#include <Arduino.h>

void laserDiscoMode() {
  int setLasers[DISCO_MODE_AMOUNT_LASERS];
  for (int i = 0; i < DISCO_MODE_AMOUNT_LASERS; i++) {
    int rand = random(1, TOTAL_LASERS);
    digitalWrite(ALL_LASERS[rand], HIGH);
    setLasers[i] = rand;
  }

  delay(DISCO_MODE_DELAY);

  //Unset all
  for (int i = 0; i < DISCO_MODE_AMOUNT_LASERS; i++) {
    digitalWrite(setLasers[i], LOW);
  }
}

void checkForButtonPressInDisco(){
  if(checkForButtonPress()){
     setGameState(GAME_COUNTDOWN);
  }
}

