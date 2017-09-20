#include <SPI.h>
#include <SevenSegmentFun.h>
#include <SevenSegmentExtended.h>
#include <SevenSegmentTM1637.h>

/**
  Display
*/
const byte PIN_CLK = 7; // define CLK pin (any digital pin)
const byte PIN_DIO = 4; // define DIO pin (any digital pin)
SevenSegmentExtended display(PIN_CLK, PIN_DIO);
/**
  Lasers
*/
const int LASER_1 = 22;
const int LASER_2 = 24;
const int LASER_3 = 26;
const int LASER_4 = 28;
const int LASER_5 = 30;
const int LASER_6 = 32;
const int LASER_7 = 34;
const int LASER_8 = 36;
const int LASER_9 = 38;
const int LASER_10 = 40;
const int LASER_11 = 42;
const int LASER_12 = 44;
const int LASER_13 = 46;
const int LASER_14 = 48;
//const int LASER_15 = 50; // Disabled due to SPI
const int ALL_LASERS[] =
{
  LASER_1, LASER_2, LASER_3, LASER_4, LASER_5, LASER_6, LASER_7, LASER_8, LASER_9, LASER_10, LASER_11, LASER_12, LASER_13, LASER_14
}
;
const int TOTAL_LASERS = 14;
/**
  Detectors
*/
const int DETECTOR_1 = 23;
const int DETECTOR_2 = 25;
const int DETECTOR_3 = 27;
const int DETECTOR_4 = 29;
const int DETECTOR_5 = 31;
const int DETECTOR_6 = 33;
const int DETECTOR_7 = 35;
const int DETECTOR_8 = 37;
const int DETECTOR_9 = 39;
const int DETECTOR_10 = 41;
const int DETECTOR_11 = 43;
const int DETECTOR_12 = 45;
const int DETECTOR_13 = 47;
const int DETECTOR_14 = 49;
//const int DETECTOR_15 = 51; // Disabled due to SPI
const int ALL_DETECTORS[] =
{
  DETECTOR_1, DETECTOR_2, DETECTOR_3, DETECTOR_4, DETECTOR_5, DETECTOR_6, DETECTOR_7, DETECTOR_8, DETECTOR_9, DETECTOR_10, DETECTOR_11, DETECTOR_12, DETECTOR_13, DETECTOR_14
};

const int TOTAL_DETECTORS = 14;
int USABLE_DETECTORS[TOTAL_DETECTORS];
int TOTAL_USABLE_DETECTORS = 0;
int BROKEN_LASER = 0;
const int DETECTOR_THRESHOLD = 10;
/**
  Buttons
*/
const int WIN_BUTTON = 16;

/**
  Reset
*/
void(* resetFunc) (void) = 0; // declare reset function at address 0

/**
  Clock
*/
long startTime;
long elapsedTime;
int fractional;
long lastSendTime;

/**
  Host/Slave
*/
bool HOST = true;
const int HOST_JUMPER = 8;

/**
  Debug Jumper
*/
const int DEBUG_JUMPER = 2;
// Protocol
const byte SPI_SET_GAME_STATE = 's';
const byte SPI_ACK = 'a';
const byte SPI_GET_GAME_STATE = 'g';
const byte SPI_NOP = 'n';
const byte SPI_RESET = 'r';


/**
  Variables
*/
const int GAME_SETUP = 1;
const int GAME_WAITING = 2;
const int GAME_COUNTDOWN = 4;
const int GAME_STARTED = 8;
const int GAME_WON = 16;
const int GAME_FAILED = 32;
const int GAME_ERROR = 128; // Error state for faulty hardware
const int GAME_DEBUG = 256;
int GAME_STATE = GAME_SETUP;

int ERROR_CODE = 0;
// 2x game settings errors
// 3x hardware errors
const int DISCO_MODE_AMOUNT_LASERS = 5;
const int DISCO_MODE_DELAY = 500;

/**
  Serial
*/
const long BAUD_RATE = 115200;
const int PRINT_TIME_INTERVAL = 1000; // ~Every second

/**
  SPI Slave settings
*/
volatile byte command = 0;
void setup()
{
  // Pin's to recognize the type
  pinMode(LED_BUILTIN, OUTPUT);
  inDebugMode();
  determineRole();
  setupHostSlave();
  // Switch to waiting mode
  delay(1000);
}

// end of setup
/**
  Loops
*/
void loop()
{
  if (HOST)
  {
    hostLoop();
  }
  else
  {
    slaveLoop();
  }
}

void hostLoop()
{
  if (GAME_STATE == GAME_SETUP)
  {
    // Run validation of lasers
    // Transition to waiting
    setGameState(GAME_WAITING);
  }
  if (GAME_STATE == GAME_WAITING)
  {
    if(TOTAL_USABLE_DETECTORS == 0){
       display.print("TEST");
       determineCollectorState();
    } else {
       display.print("JE MOEDER");
       laserDiscoMode();
       checkForButtonPressInDisco();
    }
  }
  if (GAME_STATE == GAME_COUNTDOWN)
  {
    startCountdown();
    setGameState(GAME_STARTED);
    startTime = millis(); //Reset start time
  }
  if (GAME_STATE == GAME_STARTED)
  {
    clockLoop();
    checkForLaserBreak();
    checkForWinButtonPressed();
    askSlaveForCurrentGameState();
  }
  if (GAME_STATE == GAME_WON)
  {
    checkForResetButtonPressed();
  }
  if (GAME_STATE == GAME_FAILED)
  {
    display.print("FAIL");
    digitalWrite(BROKEN_LASER, HIGH);
    checkForResetButtonPressed();
  }
  if (GAME_STATE == GAME_DEBUG)
  {
    checkForResetButtonPressed();
  }
    if (GAME_STATE == GAME_ERROR)
  {
    checkForResetButtonPressed();
  }

  delay(1000); // 1 second delay
}

// end of loop
void slaveLoop()
{
  // if SPI not active, clear current command
  if (digitalRead(SS) == HIGH)
  {
    command = 0;
  }

  if (GAME_STATE == GAME_SETUP)
  {
    //Nothing
  }
  if (GAME_STATE == GAME_WAITING)
  {
    // Stay in laser mode
    laserDiscoMode();
  }
  if (GAME_STATE == GAME_COUNTDOWN)
  {
    //Nothing
  }
  if (GAME_STATE == GAME_STARTED)
  {
    checkForLaserBreak();
  }
  if (GAME_STATE == GAME_WON)
  {
    //all lasers on
  }
  if (GAME_STATE == GAME_FAILED)
  {
    // display broken laser
  }
  if (GAME_STATE == GAME_DEBUG)
  {
    // Something in debug mode
  }
}

// end of loop

// end of loops


/**
  Master/Slave Setup functions
*/
// Determine role for this arduino board
void determineRole()
{
  pinMode(HOST_JUMPER, INPUT);
  HOST = digitalRead(HOST_JUMPER) == HIGH;
}

// End determineRole
// Switch setup based on role
void setupHostSlave()
{
  // Set all internal leds to low
  digitalWrite(LED_BUILTIN, LOW);
  if (HOST)
  {
    setupHost();
  }
  else
  {
    setupSlave();
  }
}

// End setupHostSlave
// Setup host pins
void setupHost()
{
  Serial.begin(BAUD_RATE);
  Serial.println("Setup host");
  // Set internal led to high to recognize
  digitalWrite(LED_BUILTIN, HIGH);
  // Set SPI to HIGH before to disable slave when not sending
  digitalWrite(SS, HIGH); // ensure SS stays high for now
  // Put SCK, MOSI, SS pins into output mode
  // also put SCK, MOSI into LOW state, and SS into HIGH state.
  // Then put SPI hardware into Master mode and turn SPI on
  SPI.begin();
  // Slow down the master a bit
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  // Init start time
  startTime = millis();
  // Display
  display.begin(); // initializes the display
  display.setBacklight(100); // set the brightness to 100 %
  setupLaserPins();
}

// End host setup


// Slave setup
void setupSlave()
{
  // Setup serial for checking
  Serial.begin(BAUD_RATE);
  Serial.println("Setup slave");
  // Set internal led to low to recognize
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(MISO, OUTPUT); // have to send on master in, *slave out*
  SPCR |= _BV(SPE); // turn on SPI in slave modes
  SPCR |= _BV(SPIE); // turn on interrupts
  setupLaserPins();

}
// end master/slave setup functions

void throwError(int errorCode) {
  ERROR_CODE = errorCode;
  GAME_STATE = GAME_ERROR;
  display.clear();
  display.print((String)"E"+errorCode);
  Serial.println("Error: " + errorCode);
}
