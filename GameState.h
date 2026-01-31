#ifndef GAMESTATE_H
#define GAMESTATE_H
//statement: the heart of moderation
//I understand it as a label collection of warehouse


enum Location {
  DORM,
  CCTW,
  LIBRARY,
  ACADEMIC_BUILDING,
};

enum StudyEventType {
  STUDY_EVENT_NONE, 
  STUDY_EVENT_PHONE,   
  STUDY_EVENT_HARDWORK,     
  STUDY_EVENT_STRUGGLE,     
  STUDY_EVENT_INSPIRATION  
};

enum RestEventType {
  REST_EVENT_NONE,         
  REST_EVENT_SLEEPLESS,   
  REST_EVENT_DEEP_REST     
};


struct GameState {
  int day;
  int ap;
  int energy;
  int mastery;
  Location location;
  Location selectedLocation;
};

extern GameState game;  // there is a var. game, it's class is GameState

extern const int CONFIRMATION;
extern const int INITIAL_AP;
extern const int INITIAL_ENERGY;
extern const int INITIAL_MASTERY;
extern const int DAYS_TO_EXAM;
extern const int PASS_MASTERY;
extern const int PASS_ENERGY;
extern const int MIN_ENERGY;
extern const int MAX_ENERGY;

extern const int STUDY_MASTERY_GAIN;
extern const int STUDY_ENERGY_COST;
extern const int REST_ENERGY_GAIN;

extern int MASTERY_CHANGE;
extern int ENERGY_CHANGE; 

const char* getLocationName(Location loc);  

StudyEventType getRandomStudyEvent();
RestEventType getRandomRestEvent();

bool isDayEnd();
bool isGameOver();
bool hasPassedExam();
bool isDie();
bool canConfirmAction();    
static bool lastConfirm;

void setupGame();

void applyStudyEvent(StudyEventType event, int& MASTERY_CHANGE, int& ENERGY_CHANGE);
void applyRestEvent(RestEventType event, int& ENERGY_CHANGE);

void performEventAction();
void performLocationAction(); 
void endDay();
void selectNextLocation();     
void selectPrevLocation(); 
void ap_cost();

void initOLED();              
void updateScreen();  
void updateEventScreen();
void updateSkillScreen();
void screenShowWarning();

void examLoop();
void examCompleted();
void gameOver1();
void gameOver2();

void updateLocationLEDs(bool showingAction = false);
void updateAPLEDs();
void examLEDs();
void constantLEDs();
void chooseSwitcher();
void confirmSwitcher();
void quickReset();


#endif