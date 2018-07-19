#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"

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
const int LASER_15 = 50; 
const int ALL_LASERS[] =
{
  LASER_1, LASER_2, LASER_3//, LASER_4, LASER_5, LASER_6, LASER_7, LASER_8, LASER_9, LASER_10, LASER_11, LASER_12, LASER_13, LASER_14
}
;
const int TOTAL_LASERS = 3;
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
const int DETECTOR_15 = 51;
const int ALL_DETECTORS[] =
{
  DETECTOR_1, DETECTOR_2, DETECTOR_3//, DETECTOR_4, DETECTOR_5, DETECTOR_6, DETECTOR_7, DETECTOR_8, DETECTOR_9, DETECTOR_10, DETECTOR_11, DETECTOR_12, DETECTOR_13, DETECTOR_14
};

const int TOTAL_DETECTORS = 3;
int USABLE_DETECTORS[TOTAL_DETECTORS];
int TOTAL_USABLE_DETECTORS = 0;
int BROKEN_LASER = 0;
const int DETECTOR_THRESHOLD = 1;

/**
  Buttons
*/
const int WIN_BUTTON = 16;
const int CTRL_BUTTON = 17;

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
  Debug Jumper
*/
const int DEBUG_JUMPER = 2;

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
  digitalWrite(LED_BUILTIN, LOW);
  inDebugMode();
  
  Serial.begin(BAUD_RATE);
  Serial.println("Setup host");

  // Init start time
  startTime = millis();
  
  // Display
  display.begin(); // initializes the display
  display.setBacklight(100); // set the brightness to 100 %
  
  setupLaserPins();
  
  pinMode(WIN_BUTTON, INPUT);
  pinMode(CTRL_BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(CTRL_BUTTON), blink, RISING);
  
  // Switch to waiting mode
  delay(1000);
}

void loop()
{
  if (GAME_STATE == GAME_SETUP)
  {
    setGameState(GAME_WAITING);
  }
  if (GAME_STATE == GAME_WAITING)
  {
    if(TOTAL_USABLE_DETECTORS < DETECTOR_THRESHOLD){
       determineCollectorState();
    } else {
       laserDiscoMode();
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
  }
  if (GAME_STATE == GAME_WON)
  {
    
  }
  if (GAME_STATE == GAME_FAILED)
  {
    display.print("FAIL");
    digitalWrite(BROKEN_LASER, HIGH);
  }
  if (GAME_STATE == GAME_DEBUG)
  {
 
  }
    if (GAME_STATE == GAME_ERROR)
  {

  }
}

void handleCtrlButton() {
  if (GAME_STATE == GAME_WAITING)
  {
    setGameState(GAME_COUNTDOWN);
  }
  if (GAME_STATE == GAME_COUNTDOWN)
  {
    setGameState(GAME_STARTED);
  }
  if (GAME_STATE == GAME_STARTED)
  {
    setGameState(GAME_FAILED);
  }
  if (GAME_STATE == GAME_WON)
  {
    setGameState(GAME_WAITING);
  }
  if (GAME_STATE == GAME_FAILED)
  {
    setGameState(GAME_WAITING);
  }
  if (GAME_STATE == GAME_DEBUG)
  {
    setGameState(GAME_WAITING);
  }
}

void handleFinishButton() {
  if (GAME_STATE == GAME_STARTED)
  {
    setGameState(GAME_WON);
  }
}

/**
  Debug mode
*/
void inDebugMode()
{
  if (digitalRead(DEBUG_JUMPER) == HIGH)
  {
    GAME_STATE = GAME_DEBUG;
    digitalWrite(LED_BUILTIN, HIGH);
  }
}
