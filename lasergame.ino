/******************************************************************
 Created with PROGRAMINO IDE for Arduino - 25.08.2017 18:46:39
 Project     : LaserSoftware
 Libraries   :
 Author      : Peter Mein
 Description : Control Software for multiple arduino
******************************************************************/

/**
Constants
**/

//Modi
char ROLE_LASER_CONTROLLER_1 = '1';
char ROLE_LASER_CONTROLLER_2 = '2';
char ROLE_GAME_CONTROLLER_1 = '3';
char ROLE_GAME_CONTROLLER_2 = '4';

char MODE_SINGLE_LANE = '1';
char MODE_HEAD_TO_HEAD = '2';

/**
Variables
**/
int ROLE = 0;
int MODE = 0;

int BAUD_RATE = 9600;


void setup()
{
    // Wait for serial instructions
    Serial.begin(BAUD_RATE);
    
    while(!waitForRoleSelect()){}

    if(ROLE == ROLE_LASER_CONTROLLER_1 || ROLE == ROLE_LASER_CONTROLLER_2){ setupLaserController(); }
    if(ROLE == ROLE_GAME_CONTROLLER_1 || ROLE == ROLE_GAME_CONTROLLER_2){ setupLaserController(); }

    while(!waitForGameModeSelect()){}
    
    
}

void loop()
{
  //Game scheduler

  //In start countdown 

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


  
  digitalWrite(LED_BUILTIN, HIGH);  
  delay(1000);                       
  digitalWrite(LED_BUILTIN, LOW);   
  delay(1000);                      
}



void setupLaserController(){
      pinMode(LED_BUILTIN, OUTPUT);
}

void setupGameController(){
      pinMode(LED_BUILTIN, OUTPUT);
}



bool waitForRoleSelect(){
  while (!Serial) {}

  Serial.print("Select Arduino role: ");
  while (Serial.available() <= 0){}

  char role = (Serial.read());

  Serial.println(role);

  if(role == ROLE_LASER_CONTROLLER_1){ ROLE = ROLE_LASER_CONTROLLER_1; return true; }
  if(role == ROLE_LASER_CONTROLLER_2){ ROLE = ROLE_LASER_CONTROLLER_2; return true; }
  if(role == ROLE_GAME_CONTROLLER_1){ ROLE = ROLE_GAME_CONTROLLER_1; return true; }
  if(role == ROLE_GAME_CONTROLLER_2){ ROLE = ROLE_GAME_CONTROLLER_2; return true; }
  
  return false;
}

bool waitForGameModeSelect(){
  while (!Serial) {}

  Serial.print("Select game mode: ");
  while (Serial.available() <= 0){}

  char mode = (Serial.read());

  Serial.println(mode);

  if(mode == MODE_SINGLE_LANE){ MODE = MODE_SINGLE_LANE; return true; }
  if(mode == MODE_HEAD_TO_HEAD){ MODE = MODE_HEAD_TO_HEAD; return true; }
  
  return false;
}
