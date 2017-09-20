/******************************************************************
  Created with PROGRAMINO IDE for Arduino - 25.08.2017 18:46:39
  Project     : LaserSoftware
  Libraries   :
  Author      : Peter Mein
  Description : Control Software for multiple arduino
  Device      : Arduino Mega 2560 R3
******************************************************************/

/**
   Host or Slave
*/
int TYPE = 0;
int HOST = 1;
int SLAVE = 2;


/**
   Constants
*/
char MODE_NO_MODE = '0';
char MODE_DEBUG = '3';
char MODE_SINGLE_LANE = '1';
char MODE_DISCO = '2';

char MODE = MODE_NO_MODE;

/**

*/

/**
   Variables
*/


int LANE = 0;
int LANE_1 = 1;
int LANE_2 = 2;


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


int BAUD_RATE = 9600;

int DISCO_MODE_AMOUNT_LASERS = 5;
int DISCO_MODE_DELAY = 500;


//Host slave communication
 const char SERIAL_ACK = 'A';
 const char SERIAL_START = 'S';
 const char SERIAL_FINISHED = 'F';
 const char SERIAL_DEBUG = 'D';
 const char SERIAL_DISCO = 'E';
 const char SERIAL_GAME_MODE = 'L';

int CONNECTION_TIMEOUT = 10000;

/**
   Jumpers
*/
int JUMPER_MASTER = 12;

/**
   Lasers
*/
int LASER_1 = 22;
int LASER_2 = 24;
int LASER_3 = 26;
int LASER_4 = 28;
int LASER_5 = 30;
int LASER_6 = 32;
int LASER_7 = 34;
int LASER_8 = 36;
int LASER_9 = 38;
int LASER_10 = 40;
int LASER_11 = 42;
int LASER_12 = 44;
int LASER_13 = 46;
int LASER_14 = 48;
int LASER_15 = 50;

int ALL_LASERS[] = {LASER_1, LASER_2, LASER_3, LASER_4, LASER_5, LASER_6, LASER_7, LASER_8, LASER_9, LASER_10, LASER_11, LASER_12, LASER_13, LASER_14, LASER_15};
const int TOTAL_LASERS = 15;

/**
   Detectors
*/
int DETECTOR_1 = 23;
int DETECTOR_2 = 25;
int DETECTOR_3 = 27;
int DETECTOR_4 = 29;
int DETECTOR_5 = 31;
int DETECTOR_6 = 33;
int DETECTOR_7 = 35;
int DETECTOR_8 = 37;
int DETECTOR_9 = 39;
int DETECTOR_10 = 41;
int DETECTOR_11 = 43;
int DETECTOR_12 = 45;
int DETECTOR_13 = 47;
int DETECTOR_14 = 49;
int DETECTOR_15 = 51;

int ALL_DETECTORS[] = {DETECTOR_1, DETECTOR_2, DETECTOR_3, DETECTOR_4, DETECTOR_5, DETECTOR_6, DETECTOR_7, DETECTOR_8, DETECTOR_9, DETECTOR_10, DETECTOR_11, DETECTOR_12, DETECTOR_13, DETECTOR_14, DETECTOR_15};
const int TOTAL_DETECTORS = 15;

int USABLE_DETECTORS[TOTAL_DETECTORS];
int TOTAL_USABLE_DETECTORS = 0;
int BROKEN_LASER = 0;

int DETECTOR_THRESHOLD = 10;

/**
   Buttons
*/
int WIN_BUTTON = 16;


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
  Serial1.begin(BAUD_RATE);

  setupPins();
  //Determine type of arduino
  determineType();
  //Wait for connection from host/slave
  while (!waitForConnection()) {
    return;
  }

  if(TYPE == SLAVE){
    return; //Break out of setup and start waiting for host commands
  }

  while (!waitForGameModeSelect()) {}

  // //RUN SELF TEST FOR BROKEN COLLECTORS

  if (MODE == MODE_SINGLE_LANE) {
    Serial.println("Valid game mode selected");
    if (!determineCollectorState()) {
      return;
    };
    if (!waitForGameToStart()) {
      return;
    }
    GAME_STATE = GAME_COUNTDOWN;
    startCountdown();
    startTimer();
    GAME_STATE = GAME_STARTED;
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
    finishedGame();
  }


  if(TYPE == SLAVE){
    checkForSerialCommand();
  }




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

  //Check for command via serial
  //Anounce winner with time

}


void determineType(){
  TYPE = digitalRead(JUMPER_MASTER) == HIGH ? HOST : SLAVE;
  dd((String)"I am a " + TYPE);
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
  pinMode(JUMPER_MASTER, INPUT);

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
    while (Serial1.available() > 0) {
      char incomming = Serial1.read();
      Serial.println((String)"From host: " + incomming);
      parseIncommingCommand(incomming);
    }
}

void parseIncommingCommand(char command){
  switch(command){
    case SERIAL_GAME_MODE:
      setSingleLaneMode();
      break;
      case SERIAL_DISCO:
      setDiscoMode();
      break;
      case SERIAL_DEBUG:
      setModeDebug();
      runDebug();
      break;
  }
}

void checkForLaserBreak() {
  for (int i = 0; i < TOTAL_USABLE_DETECTORS; i++) {
    if (digitalRead(USABLE_DETECTORS[i]) == LOW) {
      //Laser was broken!
      //Store the laser
      BROKEN_LASER = USABLE_DETECTORS[i];
      GAME_STATE = GAME_FINISHED;
      disableLasers();
    }
  }
}

void checkForButtonPress() {
  if (digitalRead(WIN_BUTTON) == HIGH) {
    GAME_STATE = GAME_FINISHED;
    setWinState();
  }
}

void setWinState() {
  // WIN_STATE = true;
  // if (MODE == MODE_HEAD_TO_HEAD) {
  //   HEAD_TO_HEAD_WINNER = HEAD_TO_HEAD_WINNER == 0 ? 2 : 1;
  //   Serial.write(SERIAL_FINISHED);
  // }
}

void disableLasers() {
  for (int i = 0; i < TOTAL_LASERS; i++) {
    digitalWrite(ALL_LASERS[i], LOW);
  }
}

void runDebug() {
  //Print vars

  Serial.println("Testing lasers");
  for (int i = 0; i < TOTAL_LASERS; i++) {
    Serial.println((String)"Laser: " + ALL_LASERS[i]);
    digitalWrite(ALL_LASERS[i], HIGH);
    delay(200);
    digitalWrite(ALL_LASERS[i], LOW);

  }

  //Print photocell states
  Serial.println("Printing detector states");
  for (int i = 0; i < TOTAL_DETECTORS; i++) {
    int state = digitalRead(ALL_DETECTORS[i]);
    delay(100);
    Serial.println((String)"Detector: " + ALL_DETECTORS[i]);
    Serial.println(state);
  }

  delay(5000);

  //Blink screen


}

void sendSerialCommand(char command){
  Serial1.write(command);
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
       // throwError(30);
       // return false;
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
  dd("Disco mode!");
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

bool waitForGameToStart() {
  while (!Serial) {}
  if (MODE == MODE_SINGLE_LANE) {
    return waitForGameSingleLane();
  }

  throwError(24);
  return false;
}

bool waitForConnection() {
  if (TYPE == HOST) {
    return waitForAckFromSlave();
  } else if (TYPE == SLAVE) {
    return waitForHostToStart();
  }

  throwError(21);
  return false;
}

bool waitForAckFromSlave() {
  unsigned long timeout = millis();
  dd((String)"Started sending connection to slave");
  while ((millis() - timeout) < CONNECTION_TIMEOUT) {
    delay(500);
    Serial1.write(SERIAL_START);
    while (Serial1.available() > 0) {
      char incomming = Serial1.read();
      Serial.write(incomming);
      if (incomming == SERIAL_ACK) {
        dd((String)"Connection with slave!");
        return true;
      }
    }
  }

  throwError(22); //Connection timeout, no response from other lane
}

bool waitForHostToStart() {
  unsigned long timeout = millis();
  dd((String)"Started waiting connection from host");
  while ((millis() - timeout) < CONNECTION_TIMEOUT) {
    delay(500);
    while (Serial1.available() > 0) {
      char incomming = Serial1.read();
      Serial.write(incomming);
      if (incomming == SERIAL_START) {
        Serial1.write(SERIAL_ACK);
        dd((String)"Connection with host!");
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
  Serial.println("2: Disco");
  Serial.println("3: Debug");

  Serial.print("Select game mode: ");
  while (Serial.available() <= 0) {}

  char mode = (Serial.read());

  Serial.println(mode);

  if (mode == MODE_SINGLE_LANE) {
    setSingleLaneMode();
    sendSerialCommand(SERIAL_GAME_MODE);
    return true;
  }
  if (mode == MODE_DISCO) {
    setDiscoMode();
    sendSerialCommand(SERIAL_DISCO);
    return true;
  }
  if (mode == MODE_DEBUG) {
    setModeDebug();
    sendSerialCommand(SERIAL_DEBUG);
    return true;
  }

  Serial.println("No mode selected");
  return false;
}

void setSingleLaneMode(){
  MODE = MODE_SINGLE_LANE;
  GAME_STATE = GAME_WAITING;
}

void setDiscoMode(){
  MODE = MODE_DISCO;
  GAME_STATE = GAME_NO_GAME;
}

void setModeDebug(){
  MODE = MODE_DEBUG;
  GAME_STATE = GAME_NO_GAME;
}

void dd(String msg){
  Serial.println(msg);
}
