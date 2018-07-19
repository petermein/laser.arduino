#include <Arduino.h>

/**
  Clock functions
*/
void clockLoop()
{
  elapsedTime = millis() - startTime;
  if ((elapsedTime - lastSendTime) > PRINT_TIME_INTERVAL)
  {
    sendElapsedTimeToSerial();
  }
  updateDisplay();
}

// end of clockLoop
void sendElapsedTimeToSerial()
{
  int seconds = (int) (elapsedTime / 1000L);
  lastSendTime = elapsedTime;
  Serial.print(seconds);
  Serial.print(".");
  fractional = (int) (elapsedTime % 1000L);
  if (fractional == 0)
  {
    Serial.print("000");
  }
  else if (fractional < 10)
  {
    Serial.print("00");
  }
  else if (fractional < 100)
  {
    Serial.print("0");
  }
  Serial.println(fractional);
}

// end sendElapasedTimeToSerial
void updateDisplay()
{
  int seconds = (int) (elapsedTime / 1000L);
  int minutes = seconds / 60;
  display.printTime(minutes, seconds % 60, true);
}
