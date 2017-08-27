/******************************************************************
  Created with PROGRAMINO IDE for Arduino - 25.08.2017 18:46:39
  Project     : LaserSoftware
  Libraries   :
  Author      : Peter Mein
  Description : Control Software for multiple arduino
  Device      : Arduino Mega 2560 R3
******************************************************************/

/**
   Constants
*/
char MODE_NO_MODE = '0';
char MODE_DEBUG = '4';
char MODE_SINGLE_LANE = '1';
char MODE_HEAD_TO_HEAD = '2';
char MODE_DISCO = '3';

char MODE = MODE_NO_MODE;

/**

*/

/**
   Variables
*/


int LANE = 0;
char LANE_1 = '1';
char LANE_2 = '2';


int GAME_STATE = 0;
int GAME_NO_GAME = 1;
int GAME_WAITING = 2;
int GAME_COUNTDOWN = 4;
int GAME_STARTED = 8;
int GAME_FINISHED = 16;
int GAME_ERROR = 128; //Error state for faulty hardware

bool WIN_STATE = false;
int HEAD_TO_HEAD_WINNER = 0; //1 no, 2 yes

int ERROR_CODE = 0;
//2x game settings errors
//3x hardware errors

long startTime;
long elapsedTime;
int fractional;
long WINNING_TIME = 0;

int BAUD_RATE = 9600;

int DISCO_MODE_AMOUNT_LASERS = 1;
int DISCO_MODE_DELAY = 50;


//Host slave communication
char SERIAL_ACK = 'A';
char SERIAL_START = 'S';
char SERIAL_FINISHED = 'F';
char SERIAL_RESET = 'R';

int CONNECTION_TIMEOUT = 10000;

/**
   Lasers
*/
int LASER_1 = 9;
int LASER_2 = 10;
int LASER_3 = 11;
int LASER_4 = 4;
int LASER_5 = 5;
int LASER_6 = 6;
int LASER_7 = 7;
int LASER_8 = 8;
int LASER_9 = 9;
int LASER_10 = 10;
int LASER_11 = 11;
int LASER_12 = 12;
int LASER_13 = 13;
int LASER_14 = 14;
int LASER_15 = 15;

int ALL_LASERS[] = {LASER_1, LASER_2, LASER_3}; // LASER_4, LASER_5, LASER_6, LASER_7, LASER_8, LASER_9, LASER_10, LASER_11, LASER_12, LASER_13, LASER_14, LASER_15};
const int TOTAL_LASERS = 3;

/**
   Detectors
*/
int DETECTOR_1 = 6;
int DETECTOR_2 = 5;
int DETECTOR_3 = 4;
int DETECTOR_4 = 24;
int DETECTOR_5 = 25;
int DETECTOR_6 = 26;
int DETECTOR_7 = 27;
int DETECTOR_8 = 28;
int DETECTOR_9 = 29;
int DETECTOR_10 = 30;
int DETECTOR_11 = 31;
int DETECTOR_12 = 32;
int DETECTOR_13 = 33;
int DETECTOR_14 = 34;
int DETECTOR_15 = 35;

int ALL_DETECTORS[] = {DETECTOR_1, DETECTOR_2, DETECTOR_3 };//DETECTOR_4, DETECTOR_5, DETECTOR_6, DETECTOR_7, DETECTOR_8, DETECTOR_9, DETECTOR_10, DETECTOR_11, DETECTOR_12, DETECTOR_13, DETECTOR_14, DETECTOR_15};
const int TOTAL_DETECTORS = 3;

int USABLE_DETECTORS[TOTAL_DETECTORS];
int LASER_USABLE_DETECTOR_TUPLE[TOTAL_DETECTORS];
int TOTAL_USABLE_DETECTORS = 0;
int BROKEN_LASER = 0;

int DETECTOR_THRESHOLD = 1;

/**
   Buttons
*/
int WIN_BUTTON = 12;


/**
   Reset
*/
void(* resetFunc) (void) = 0;//declare reset function at address 0

/**
   Game code
*/
void setup()
{
  // Wait for serial instructions
  Serial.begin(BAUD_RATE);

  setupPins();

  while (!waitForGameModeSelect()) {}

  //RUN SELF TEST FOR BROKEN COLLECTORS

  if (MODE == MODE_SINGLE_LANE || MODE == MODE_HEAD_TO_HEAD) {
    Serial.println("Valid game mode selected");
    if (!determineCollectorState()) {
      return;
    };

    if (!waitForGameToStart()) {
      return;
    }
    GAME_STATE = GAME_COUNTDOWN;
    startCountdown();
    enableLasers();
    startTimer();
    GAME_STATE = GAME_STARTED;
  }

  if(MODE == MODE_DISCO){
      Serial.println("Disco mode engagend!");
  }

  if (MODE == MODE_DEBUG) {
    runDebug();
    resetFunc();
  }
}

void loop()
{
  //Game scheduler
  if (GAME_STATE == GAME_ERROR) {
    Serial.println((String) "Game in error mode " + ERROR_CODE);
    delay(5000);
  }

  //In start countdown
  if (GAME_STATE == GAME_STARTED) {
    clockLoop();
    checkForLaserBreak();
    checkForButtonPress();
  }

  if (GAME_STATE == GAME_NO_GAME) {
    if (MODE == MODE_DISCO) {
      laserDiscoMode();
    }

    //Debug can't get into the loop
  }

  if (GAME_STATE == GAME_FINISHED) {
    //Display winning time
  }

  //Alway check for reset command
  checkForResetCommand();



  //Start clock cycle

  //Increment clock

  //Read all pins

  //If there is a low pin -> game failed
  //Display fail
  //Send serial command to php game finished

  //If button is pressed -> game won
  //Stop time
  //Display win
  //Send serial command to php game finished with time

  //If game mode is head to head
  //Check for command via serial
  //Anounce winner with time

}

void startTimer() {
  startTime = millis();
}

void startCountdown() {
  Serial.println("Get ready!");
  delay(1000);
  Serial.println(5);
  delay(1000);
  Serial.println(4);
  delay(1000);
  Serial.println(3);
  delay(1000);
  Serial.println(2);
  delay(1000);
  Serial.println(1);
  delay(1000);
  Serial.println("Go!");

}

void clockLoop() {
  if (!GAME_STARTED) {
    return; //Clock loop shoud not run on other than game started
  }

  printElapsedTime();
}

void printElapsedTime() {
  elapsedTime =   millis() - startTime;
  printTime(elapsedTime);
}

void printTime(long time) {
  Serial.print( (int)(elapsedTime / 1000L));
  Serial.print(".");

  fractional = (int)(elapsedTime % 1000L);

  if (fractional == 0) {
    Serial.print("000");
  } else if (fractional < 10) {
    Serial.print("00");
  } else if (fractional < 100) {
    Serial.print("0");
  }

  Serial.println(fractional);
}

void setupPins() {
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


bool finishedGame() {
  if (WIN_STATE) {
    //Display time
    Serial.print((String) "Winning time: ");
    printElapsedTime();
  } else {
    //TODO Display fail
    digitalWrite(BROKEN_LASER, HIGH);
  }
}

void checkForSerialCommand() {
  if (MODE == MODE_HEAD_TO_HEAD) {
    while (Serial.available() > 0) {
      char incomming = Serial.read();
      if (incomming == SERIAL_FINISHED) {
        //
        return true;
      }
    }
  }
}

void checkForResetCommand(){
  while (Serial.available() > 0) {
      char incomming = Serial.read();
      if (incomming == SERIAL_RESET) {
        resetFunc();
      }
    }
}

void checkForLaserBreak() {
  for (int i = 0; i < TOTAL_USABLE_DETECTORS; i++) {
    if (digitalRead(USABLE_DETECTORS[i]) == LOW) {
      //Laser was broken!
      //Store the laser
      BROKEN_LASER = LASER_USABLE_DETECTOR_TUPLE[i];
      GAME_STATE = GAME_FINISHED;
      disableLasers();
      digitalWrite(BROKEN_LASER, HIGH);
      Serial.println("Laser broken!");
      return;
    }
  }
}

void checkForButtonPress() {
  if (digitalRead(WIN_BUTTON) == HIGH) {
    GAME_STATE = GAME_FINISHED;
    setWinState();
    Serial.println("Button pressed!");
  }
}

void setWinState() {
  WIN_STATE = true;
  WINNING_TIME =   millis() - startTime;
  if (MODE == MODE_HEAD_TO_HEAD) {
    HEAD_TO_HEAD_WINNER = HEAD_TO_HEAD_WINNER == 0 ? 2 : 1;
  }
  Serial.print((String)"Winning time: ");
  printTime(WINNING_TIME);
}

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

void runDebug() {
  //Print vars

  Serial.println("Testing lasers");
  for (int i = 0; i < TOTAL_LASERS; i++) {
    Serial.println("Laser: " + i);
    digitalWrite(ALL_LASERS[i], HIGH);
    delay(2000);
    digitalWrite(ALL_LASERS[i], LOW);

  }

  //Print photocell states
  Serial.println("Printing detector states");
  for (int i = 0; i < TOTAL_DETECTORS; i++) {
    int state = digitalRead(ALL_DETECTORS[i]);
    Serial.println((String)"Detector: " + i + " is " + state);
  }

  //Blink screen


}

bool determineCollectorState() {
  enableLasers();
  delay(200);
  Serial.println("Error correcting on detectors");
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

  disableLasers();
  for (int i = 0; i < TOTAL_USABLE_DETECTORS; i++) {
      for(int j = 0; j < TOTAL_LASERS; j++){
          digitalWrite(ALL_LASERS[j], HIGH);
          delay(500);
          if(digitalRead(USABLE_DETECTORS[i]) == HIGH){
             //Found mapping
             //add to list
             LASER_USABLE_DETECTOR_TUPLE[i] = ALL_LASERS[j];
          }
          digitalWrite(ALL_LASERS[j], LOW);
      }
  }



  //Start mapping lasers to detectors
  
  
  Serial.println((String)"Found " + TOTAL_USABLE_DETECTORS + " usable detectors of " + TOTAL_DETECTORS);
  disableLasers();
  if (TOTAL_USABLE_DETECTORS < DETECTOR_THRESHOLD) {
    Serial.println("Game could not start, not enough detectors");
    throwError(30);
    return false;
  }

  return true;
}

void throwError(int errorCode) {
  ERROR_CODE = errorCode;
  GAME_STATE = GAME_ERROR;
  //Display on screen?
  Serial.println("Error: " + errorCode);
}

void laserDiscoMode() {
  int setLasers[DISCO_MODE_AMOUNT_LASERS];
  for (int i = 0; i < DISCO_MODE_AMOUNT_LASERS; i++) {
    int rand = random(0, TOTAL_LASERS);
    digitalWrite(ALL_LASERS[rand], HIGH);
    setLasers[i] = ALL_LASERS[rand];
  }

  delay(DISCO_MODE_DELAY);

  //Unset all
  for (int i = 0; i < DISCO_MODE_AMOUNT_LASERS; i++) {
    digitalWrite(setLasers[i], LOW);
  }
}

bool waitForGameToStart() {
  while (!Serial) {}
  if (MODE == MODE_HEAD_TO_HEAD) {
    return waitForGameHeadToHead();
  } else if (MODE == MODE_SINGLE_LANE) {
    return waitForGameSingleLane();
  }

  throwError(24);
  return false;
}

bool waitForGameHeadToHead() {
  Serial.print("Starting head to head game! Waiting for lane select: ");
  while (Serial.available() <= 0) {}
  char lane =  (Serial.read());
  Serial.println(lane);
  if (lane == LANE_1) {
    lane = LANE_1; //HOST
    return startGameAsHost();
  } else if (lane == LANE_2) {
    lane = LANE_2; //SLAVE
    return startGameAsSlave();
  }

  throwError(21);
  return false;
}

bool startGameAsHost() {
  unsigned long timeout = millis();
  Serial.println("Started sending start to slave, waiting for ACK");
  while ((millis() - timeout) < CONNECTION_TIMEOUT) {
    while (Serial.available() > 0) {
      char incomming = Serial.read();
      if (incomming == SERIAL_ACK) {
        return true;
      }
    }
  }

  throwError(22); //Connection timeout, no response from other lane
}

bool startGameAsSlave() {
  unsigned long timeout = millis();
  while ((millis() - timeout) < CONNECTION_TIMEOUT) {
    while (Serial.available() > 0) {
      char incomming = Serial.read();
      if (incomming == SERIAL_START) {
        Serial.write(SERIAL_ACK);
        return true;
      }
    }
  }
  throwError(23); //Connection timeout, no response from other lane
}


bool waitForGameSingleLane() {
  Serial.print("Starting single lane game! Press any key to start: ");
  while (Serial.available() <= 0) {}
  char lane =  (Serial.read());
  LANE = LANE_1; //Always lane 1 in single mode
  return true;
}

bool waitForGameModeSelect() {
  while (!Serial) {}

  Serial.println("1: Single lane");
  Serial.println("2: Head to head");
  Serial.println("3: Disco");
  Serial.println("4: Debug");

  Serial.print("Select game mode: ");
  while (Serial.available() <= 0) {}

  char mode = (Serial.read());

  Serial.println(mode);

  if (mode == MODE_SINGLE_LANE) {
    MODE = MODE_SINGLE_LANE;
    GAME_STATE = GAME_WAITING;
    return true;
  }
  if (mode == MODE_HEAD_TO_HEAD) {
    MODE = MODE_HEAD_TO_HEAD;
    GAME_STATE = GAME_WAITING;
    return true;
  }
  if (mode == MODE_DISCO) {
    MODE = MODE_DISCO;
    GAME_STATE = GAME_NO_GAME;
    return true;
  }
  if (mode == MODE_DEBUG) {
    MODE = MODE_DEBUG;
    GAME_STATE = GAME_NO_GAME;
    return true;
  }

  Serial.println("No mode selected");
  return false;
}
