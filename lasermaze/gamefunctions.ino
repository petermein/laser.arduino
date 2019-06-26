#include <Arduino.h>

void setGameState(int state) {
  
  if (HOST) {
    byte result = sendCommandWithInteger(SPI_SET_GAME_STATE, state);
    if (result != SPI_ACK) {
        Serial.println("A");
        Serial.println(result);
    } else {
        Serial.println("NA");
        Serial.println(result);
    }
  }

  if(isValidGameState(state)){
      Serial.println((String)GAME_STATE+">"+state);
      GAME_STATE = state;
  }
}

bool isValidGameState(int x){
  return x == GAME_SETUP || x == GAME_WAITING || x == GAME_COUNTDOWN || x == GAME_STARTED || x == GAME_WON || x == GAME_FAILED || x == 64 || x == GAME_ERROR || x == GAME_DEBUG;
}

void checkForWinButtonPressed(){
  if(checkForButtonPress()){
    setGameState(GAME_WON);
  }
}

void checkForResetButtonPressed(){
  if(checkForButtonPress()){
    sendCommandWithInteger(SPI_RESET, SPI_NOP);
    resetFunc();
  }
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

bool checkForButtonPress() {
  return digitalRead(WIN_BUTTON) == HIGH;
}




