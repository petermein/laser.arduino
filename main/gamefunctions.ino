#include <Arduino.h>

void setGameState(int state) {
  if(isValidGameState(state)){
      Serial.println((String)GAME_STATE+">"+state);
      GAME_STATE = state;
       if (GAME_STATE == GAME_WAITING)
        {
          display.print("DISC");
        }
        else if (GAME_STATE == GAME_COUNTDOWN)
        {
        }
        else if (GAME_STATE == GAME_STARTED)
        {
        }
        else if (GAME_STATE == GAME_WON)
        {
        }
        else if (GAME_STATE == GAME_FAILED)
        {
          display.print("FAIL");
        }
        else if (GAME_STATE == GAME_DEBUG)
        {
          display.print("DBUG");
        }
  }else{
      Serial.println((String)state + "is not a valid state");
  }
}

bool isValidGameState(int x){
  return x == GAME_SETUP || x == GAME_WAITING || x == GAME_COUNTDOWN || x == GAME_STARTED || x == GAME_WON || x == GAME_FAILED || x == 64 || x == GAME_ERROR || x == GAME_DEBUG;
}

void startCountdown() {
  display.clear();
  display.print(5);
  delay(1000);
  display.print(4);
  delay(1000);
  display.print(3);
  delay(1000);
  display.print(2);
  delay(1000);
  display.print(1);
  delay(1000);
}
