#include "GameState.h"

//the setup of the OLED library and random function
#include <Wire.h>           
#include <Adafruit_GFX.h>  
#include <Adafruit_SSD1306.h> 
#include <stdlib.h> 

// what is start with oled
#define SCREEN_WIDTH 128  
#define SCREEN_HEIGHT 64    
#define OLED_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//connected with the struct in header(like class in python)
GameState game;

// constant with skill information(mastery and energy)
const int INITIAL_AP = 3;
const int INITIAL_ENERGY = 5;
const int INITIAL_MASTERY = 0;
const int DAYS_TO_EXAM = 5;
const int PASS_MASTERY = 30;
const int PASS_ENERGY = 5;
const int MIN_ENERGY = 0; 
const int MAX_ENERGY = 10;

const int STUDY_MASTERY_GAIN = 3;
const int STUDY_ENERGY_COST = 2;
const int REST_ENERGY_GAIN = 2;

int MASTERY_CHANGE = 0;
int ENERGY_CHANGE = 0; 


// set up game value
void setupGame() {
  game.day = 1;
  game.ap = 3;
  game.energy = 5;
  game.mastery = 0;
  game.location = DORM;
  game.selectedLocation = DORM;
}

//functions with daily routine
void selectNextLocation() {
  int current = (int)game.selectedLocation;
  current = (current + 1) % 4; 
  game.selectedLocation = (Location)current;
}

void selectPrevLocation() {
  int current = (int)game.selectedLocation;
  current = (current + 3) % 4; 
  game.selectedLocation = (Location)current;
}

//the trigger of the random event
StudyEventType getRandomStudyEvent() {
  int randNum = random(100); 
  
  if (randNum < 65) {      
      return STUDY_EVENT_NONE;
  }
  else if (randNum < 73) {       
      return STUDY_EVENT_PHONE;
  }
  else if (randNum < 81) {     
      return STUDY_EVENT_HARDWORK;
  }
  else if (randNum < 87) {       
      return STUDY_EVENT_STRUGGLE;
  }
  else {                      
      return STUDY_EVENT_INSPIRATION;
  }
}

RestEventType getRandomRestEvent() {
  int randNum = random(100);
  
  if (randNum < 80) {
      return REST_EVENT_NONE;
  }
  
  if (randNum < 92) {
      return REST_EVENT_SLEEPLESS;
  }

  return REST_EVENT_DEEP_REST;
}

//the result of the random event using switcher
void applyStudyEvent(StudyEventType event, int& MASTERY_CHANGE, int& ENERGY_CHANGE) {
  switch(event) {
    case STUDY_EVENT_PHONE:  
      MASTERY_CHANGE -= 2;
      break;
        
    case STUDY_EVENT_HARDWORK: 
      MASTERY_CHANGE += 2; 
      ENERGY_CHANGE -= 1;  
      break;
        
    case STUDY_EVENT_STRUGGLE:  
      MASTERY_CHANGE -= 1;  
      ENERGY_CHANGE -= 1;  
      break;
      
    case STUDY_EVENT_INSPIRATION:  
      MASTERY_CHANGE += 1;  
      break;
        
    case STUDY_EVENT_NONE:
      default:
        break;
  }  
}

void applyRestEvent(RestEventType event, int& ENERGY_CHANGE) {
  switch(event) {
    case REST_EVENT_SLEEPLESS:  
      ENERGY_CHANGE -= 1;  
        
    case REST_EVENT_DEEP_REST: 
      ENERGY_CHANGE += 1; 
      break;
        
    case REST_EVENT_NONE:
      default:
        break;
  }
}

//count everyday's ap cost
void ap_cost(){
  Location loc = game.selectedLocation; 
  int apCost = 1;
  if (loc == DORM){
    apCost = 2;
  }
  game.ap -= apCost;
}


//determine if the random event
void performEventAction(){
  Location loc = game.selectedLocation; 
  if (loc == ACADEMIC_BUILDING || loc == LIBRARY) {
    StudyEventType studyEvent = getRandomStudyEvent();
    applyStudyEvent(studyEvent, MASTERY_CHANGE, ENERGY_CHANGE);
    }
  else if (loc == DORM || loc == CCTW) {
    RestEventType restEvent = getRandomRestEvent();
    applyRestEvent(restEvent, ENERGY_CHANGE);

  }
  if (MASTERY_CHANGE != 0 || ENERGY_CHANGE != 0){
    updateEventScreen();
  }
}

//control the result of the location event
//the two var. ENERGY_CHANGE and MASTERY_CHANGE are shared by these two function
void performLocationAction() {
  Location loc = game.selectedLocation; 

  switch(loc) {
    case ACADEMIC_BUILDING:
      MASTERY_CHANGE += STUDY_MASTERY_GAIN;
      ENERGY_CHANGE -= STUDY_ENERGY_COST; 
      break;
      
    case DORM:
      ENERGY_CHANGE += REST_ENERGY_GAIN + 3;
      break;
      
    case CCTW:
      ENERGY_CHANGE += REST_ENERGY_GAIN;
      break;
      
    case LIBRARY:
      MASTERY_CHANGE += STUDY_MASTERY_GAIN + 1; 
      ENERGY_CHANGE -= (STUDY_ENERGY_COST + 1); 
      break;
  }

    if (game.energy >= MAX_ENERGY && ENERGY_CHANGE > 0) {
      ENERGY_CHANGE = 0; 
    }

    if (game.energy + ENERGY_CHANGE > MAX_ENERGY) {
      ENERGY_CHANGE = MAX_ENERGY - game.energy;  
  }

      game.mastery += MASTERY_CHANGE;
      game.energy += ENERGY_CHANGE;
  
  game.location = loc;
}

//change the var. into char.
const char* getLocationName(Location loc) {
  switch(loc) {
    case DORM:
        return "Dorm";
    case CCTW:
        return "CCTW";
    case ACADEMIC_BUILDING:
        return "Academic Building";
    case LIBRARY:
        return "Library";
    default:
        return "???";
  }
}


//the function use when day ends
//with showcase and the initialization of daily var.
void endDay() {
  display.clearDisplay();

  game.day++;
  game.ap = INITIAL_AP;
  game.location = DORM;
  game.selectedLocation = DORM;
  
  int sleep_energy = 0;
  sleep_energy = REST_ENERGY_GAIN;
  if (game.energy >= MAX_ENERGY && sleep_energy > 0) {
      sleep_energy = 0; 
    }

  if (game.energy + sleep_energy > MAX_ENERGY) {
    sleep_energy = MAX_ENERGY - game.energy;  
  }
  game.energy += sleep_energy;

  display.setTextSize(1);
  display.setCursor(10, 8);
  display.print(F("Have a good sleep!"));
  display.setCursor(0, 25); 
  display.print(F("Energy:"));
  if (sleep_energy > 0){
    display.print("+");
  }
  display.print(sleep_energy);
  display.display();
}

//the showcase if player dies in game
void gameOver1(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 10);
  display.print(F("DIE from"));
  display.setCursor(10, 35);
  display.print(F("OVERWORK"));
  display.display();
  delay(5000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15, 25);
  display.print(F("Goodbye!"));
  display.display();
  delay(10000);

}

//the showcase of the exam day
//also with how the game ends 
void gameOver2(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10, 25);
  display.print(F("Here comes EXAM day"));
  display.display();
  delay(5000);

  examLoop();
  display.setCursor(30, 0);
  display.print(F("RESULT"));
  display.setTextSize(1);
  display.setCursor(0, 25); 
  display.print(F("Energy:"));
  display.print(game.energy);  
  display.print(F("/"));
  display.print(PASS_ENERGY);
  display.setCursor(0, 50);
  display.print(F("Mastery:"));
  display.print(game.mastery); 
  display.print(F("/"));
  display.print(PASS_MASTERY);
  display.display();
  delay(3000);

  if(hasPassedExam()){
    display.clearDisplay();
    display.setCursor(10, 8);
    display.print(F("Congratulation!"));
    display.setCursor(10, 25);
    display.print(F("You pass the exam!"));
    display.display();
  }
  else{
    display.clearDisplay();
    display.setCursor(32, 8);
    display.print(F("Sorry..."));
    display.setCursor(32, 25);
    display.print(F("You fail.."));
    display.display();
  }
  delay(5000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15, 25);
  display.print(F("Goodbye!"));
  display.display();

}

//the detailed showcase of the exam!
int examProgress = 0;

const int PROGRESS_INCREASE = 3;   
const int PROGRESS_DECREASE = 4; 

//initialization with exam interface and progress bar
void displayExamScreen() {
  examProgress = 0;
  display.clearDisplay();
  
  display.setTextSize(2);
  display.setCursor(42, 0);
  display.print(F("EXAM"));
  
  display.drawRect(5, 25, 118, 20, WHITE);
  int barWidth = map(examProgress, 0, 100, 0, 114);
  display.fillRect(7, 27, barWidth, 16, WHITE);
  
  display.setTextSize(1);
  display.setCursor(100, 30);
  display.print(examProgress);
  display.print(F("%"));
  
  display.setTextSize(1);
  display.setCursor(10, 55);
  if (digitalRead(5) == LOW) {
    display.print(F("WRITING..."));
  } 
  else {
    display.print(F("STOP DAYDREAMING!"));
  }
  
  display.display();
}

//how the progress bar works with the comfirm button
//use millis to control the increase/decrease(turn 100 ticks into 1)
void updateExamProgress() {
    static unsigned long lastUpdate = 0;
    unsigned long currentTime = millis();
    
    if (currentTime - lastUpdate < 100) {
        return;
    }
    lastUpdate = currentTime;
    
    bool isButtonPressed = (digitalRead(CONFIRMATION) == LOW);
    
    if (isButtonPressed) {
        examProgress += PROGRESS_INCREASE;
    } 
    else {
        examProgress -= PROGRESS_DECREASE;
        if (examProgress < 0) {
            examProgress = 0;
        }
    }
    
    if (examProgress > 100) {
        examProgress = 100;
    }
}

//the main loop of th exam logic
//also with the blinking showcase when the exam finish
void examLoop() {
  while (true) {
    examLEDs();
    updateExamProgress();
    displayExamScreen();
    
    if (examProgress >= 100) {
      break;
    }
  delay(10);
  }
  constantLEDs();
  for (int i = 0; i < 3; i++) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(30, 20);
    display.print(F("DONE!"));
    display.display();
    delay(300);
    
    display.clearDisplay();
    display.display();
    delay(300);
  }
}

//part with OLED

//the initial/cover of the game
void initOLED() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    return;
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(20, 5);  
  display.print(F("Designed by P.F."));

  display.setTextSize(2);
  display.setCursor(40, 20);  
  display.print(F("Exam"));

  display.setCursor(10, 40);
  display.print(F("Explosion"));

  display.display();
  
  delay(5000);  
}

//the showcse of the skill screen
void updateSkillScreen(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15,0);
  display.print("Striving");
  display.setTextSize(1);
  if (ENERGY_CHANGE > 0 || ENERGY_CHANGE < 0){
    display.setCursor(0, 25); 
    display.print(F("Energy:"));

    if (ENERGY_CHANGE > 0){
      display.print("+");
    }
    display.print(ENERGY_CHANGE);  
  }
  
  if (MASTERY_CHANGE > 0 || MASTERY_CHANGE < 0){
    display.setCursor(64, 25); 
    display.print(F("Mastery:"));
    if (MASTERY_CHANGE > 0){
      display.print("+");
    }
    display.print(MASTERY_CHANGE);  
  }
  display.setCursor(20, 45);
  display.print(F("*Action Result*"));

  display.display();

  MASTERY_CHANGE = 0;
  ENERGY_CHANGE = 0;
  delay(3000);
}

//the showcse of the event screen
void updateEventScreen(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 0);
  display.print(F("Watch Out"));
  display.setTextSize(1);

  if (ENERGY_CHANGE > 0 || ENERGY_CHANGE < 0){
    display.setCursor(0, 25); 
    display.print(F("Energy:"));
    if (ENERGY_CHANGE > 0){
      display.print("+");
    }
    display.print(ENERGY_CHANGE);  
  }
  
  if (MASTERY_CHANGE > 0 || MASTERY_CHANGE < 0){
    display.setCursor(64, 25); 
    display.print(F("Mastery:"));
    if (MASTERY_CHANGE > 0){
      display.print("+");
    }
    display.print(MASTERY_CHANGE);  
  }

  display.setCursor(0, 45);
  display.print(F("Event: "));
  if (MASTERY_CHANGE == -2 && ENERGY_CHANGE == 0)  {
    display.print(F("[Phone]"));
  } 
  else if (MASTERY_CHANGE == 2 && ENERGY_CHANGE == -1) {
    display.print(F("[Hard Work]"));
  } 
  else if (MASTERY_CHANGE == -1 && ENERGY_CHANGE == -1) {
    display.print(F("[Struggle]"));
  } 
  else if (MASTERY_CHANGE == 1 && ENERGY_CHANGE == 0) {
    display.print(F("[Inspiration]"));
  } 
  else if (ENERGY_CHANGE == -1 && MASTERY_CHANGE == 0) {  
    display.print(F("[Sleepless]"));
  } 
  else if (ENERGY_CHANGE == 1 && MASTERY_CHANGE == 0){ 
    display.print(F("[Deep Rest]"));
  } 
  else {
    display.print(F(""));
  }
  display.display();
  delay(3000);
}

//the showcase of everyday routine screen
//with day, the value of energy/mastery, and the location selected
void updateScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(F("Day "));
  display.print(game.day); 
  display.print(F("/5"));

  display.setTextSize(1);
  display.setCursor(0, 25); 
  display.print(F("Energy:"));
  display.print(game.energy);  
  
  display.setCursor(64, 25);
  display.print(F("Mastery:"));
  display.print(game.mastery); 
  
  display.drawFastHLine(0, 40, 128, WHITE);

  const char* locName = getLocationName(game.selectedLocation);
  if (strcmp(locName, "Academic Building") == 0) {
    display.setTextSize(1);
    display.setCursor(10, 50);
  } 
  else {
    display.setTextSize(2);
    if (strcmp(locName, "Dorm") == 0) {
      display.setCursor(40, 45);
    } 
    else if (strcmp(locName, "CCTW") == 0) {
      display.setCursor(35, 45);
    } 
    else if (strcmp(locName, "Library") == 0) {
      display.setCursor(25, 45);
    }
  }
  display.print(locName);
  display.display();
}


//the showcase screen when ap is not enough(only happen when choosing dorm)
void screenShowWarning(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(20, 25); 
  display.print(F("Not Enough AP"));
  display.display();
  delay(3000);
}

//boolean with if can do sth or not 
bool canConfirmAction() {
  if(game.selectedLocation == DORM) {
    return game.ap >= 2; 
  } 
  else {
    return game.ap >= 1; 
  }
}

bool isDayEnd(){
    return game.ap == 0;
}

bool isDie(){
  return (game.energy < MIN_ENERGY);
}

bool isGameOver() {
  return (game.day > DAYS_TO_EXAM);
}

bool hasPassedExam() {
  return (game.mastery >= PASS_MASTERY) && (game.energy >= PASS_ENERGY);
}