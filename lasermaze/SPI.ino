#include <Arduino.h>

byte sendCommandWithInteger(byte command, int value) {

  byte result;

  // enable Slave Select
  digitalWrite(SS, LOW);

  transferAndWait (command);
  transferAndWait(value);
  result = transferAndWait(SPI_NOP);
  // disable Slave Select
  digitalWrite(SS, HIGH);

  return result;
}

byte transferAndWait(const byte b)
{
  byte a = SPI.transfer(b);
  delayMicroseconds(100); //Instellen om timing issues te voorkomen
  return a;
}

// end of transferAndWait
// SPI interrupt routine

ISR(SPI_STC_vect)
{
  byte c = SPDR;

  switch (command)
  {
    // no command? then this is the command
    case 0:
      command = c;
      SPDR = 0;
      break;
    // add to incoming byte, return result
    case SPI_SET_GAME_STATE:
      setGameState(SPDR);
      SPDR = SPI_ACK; // Set ACK to register
      break;
    case SPI_GET_GAME_STATE:
      SPDR = GAME_STATE; // Set game_state to register
      break;
    case SPI_RESET:
      resetFunc();
      break;
  }
  // end of switch
} // end of interrupt service routine (ISR) SPI_STC_vect

