#include "GameState.h"

//LED of Location
const int LED_DORM = 11;
const int LED_CCTW = 10;
const int LED_LIBRARY = 9;
const int LED_ACADEMIC_BUILDING = 8;

//Switcher
const int GO_UP = 7;
const int GO_DOWN = 6;
const int CONFIRMATION = 5;

//LED of Action Point
const int LED_AP1 = 4;
const int LED_AP2 = 3;
const int LED_AP3 = 2;

void setup(){
    Serial.begin(9600);

    pinMode(LED_DORM, OUTPUT);
    pinMode(LED_CCTW, OUTPUT);
    pinMode(LED_LIBRARY, OUTPUT);
    pinMode(LED_ACADEMIC_BUILDING, OUTPUT);

    pinMode(LED_AP1, OUTPUT);
    pinMode(LED_AP2, OUTPUT);
    pinMode(LED_AP3, OUTPUT);

    pinMode(GO_UP, INPUT_PULLUP);
    pinMode(GO_DOWN, INPUT_PULLUP);
    pinMode(CONFIRMATION, INPUT_PULLUP);

    //showcase of the beginning of the game
    initOLED(); 
    setupGame(); 
    updateLocationLEDs();  
    updateAPLEDs();   
    updateScreen();
}

//the code controlling the location LEDs with state monitor
void updateLocationLEDs(bool showingAction) {
    digitalWrite(LED_DORM, LOW);
    digitalWrite(LED_CCTW, LOW);
    digitalWrite(LED_LIBRARY, LOW);
    digitalWrite(LED_ACADEMIC_BUILDING, LOW);

    if (game.day == 6){
        digitalWrite(LED_LIBRARY, HIGH);
        return;
    }

    int currentPin, selectedPin;

    //input the location and selected location
    switch(game.location) {
        case DORM: currentPin = LED_DORM; break;
        case CCTW: currentPin = LED_CCTW; break;
        case LIBRARY: currentPin = LED_LIBRARY; break;
        case ACADEMIC_BUILDING: currentPin = LED_ACADEMIC_BUILDING; break;
    }
    switch(game.selectedLocation) {
        case DORM: selectedPin = LED_DORM; break;
        case CCTW: selectedPin = LED_CCTW; break;
        case LIBRARY: selectedPin = LED_LIBRARY; break;
        case ACADEMIC_BUILDING: selectedPin = LED_ACADEMIC_BUILDING; break;
    }

    //Special not blinking mode
    if (showingAction){
        digitalWrite(currentPin, LOW);
        digitalWrite(selectedPin, HIGH);
        return;
    }
    digitalWrite(currentPin, HIGH);

    //blinking mode using millis to count the time and to renew itself
    static bool blink = false;
    static unsigned long lastTime = 0;
    if(millis() - lastTime > 500) {
        lastTime = millis();
        blink = !blink;
    }
    digitalWrite(selectedPin, blink ? HIGH : LOW);
}

//the code controlling the AP LEDs
void updateAPLEDs() {
    //special mode when player dies
    if (isDie()){
        digitalWrite(LED_AP1, LOW);
        digitalWrite(LED_AP2, LOW);
        digitalWrite(LED_AP3, LOW);
        return;
    }
    digitalWrite(LED_AP1, game.ap >= 1);
    digitalWrite(LED_AP2, game.ap >= 2);
    digitalWrite(LED_AP3, game.ap >= 3);
}

//the code mode when all lights needs to blink
//actually, this function is not only use in exam period, but since it is first written
//when doing the light control of exam time, so it gets this name
void examLEDs(){
    static bool blink = false;
    static unsigned long lastTime = 0;
    if(millis() - lastTime > 500) {
        lastTime = millis();
        blink = !blink;
    }
    digitalWrite(LED_ACADEMIC_BUILDING, blink ? HIGH : LOW);
    digitalWrite(LED_CCTW, blink ? HIGH : LOW);
    digitalWrite(LED_LIBRARY, blink ? HIGH : LOW);
    digitalWrite(LED_DORM, blink ? HIGH : LOW);
    digitalWrite(LED_AP1, blink ? HIGH : LOW);
    digitalWrite(LED_AP2, blink ? HIGH : LOW);
    digitalWrite(LED_AP3, blink ? HIGH : LOW);
}

//the code mode when all lights needs to light up
void constantLEDs(){
    digitalWrite(LED_ACADEMIC_BUILDING, HIGH);
    digitalWrite(LED_CCTW, HIGH);
    digitalWrite(LED_LIBRARY, HIGH);
    digitalWrite(LED_DORM, HIGH);
    digitalWrite(LED_AP1, HIGH);
    digitalWrite(LED_AP2, HIGH);
    digitalWrite(LED_AP3, HIGH);
}

//the logic with the chooseSwitcher to go up and down 
//these two functions all have the edge detection function that if you kick the switcher, the function will only trigger once
void chooseSwitcher() {
    static bool lastUp = HIGH;
    static bool lastDown = HIGH;

    bool upNow = digitalRead(GO_UP);
    bool downNow = digitalRead(GO_DOWN);

    if (lastUp == HIGH && upNow == LOW) {
        selectNextLocation();
        updateLocationLEDs(false);
        updateScreen();
    }

    if (lastDown == HIGH && downNow == LOW) {
        selectPrevLocation();
        updateLocationLEDs(false);
        updateScreen();
    }

    lastUp = upNow;
    lastDown = downNow;
}

//the logic with the confirmSwitcher to confirm the location player wants to go 
void confirmSwitcher(){
    static bool lastConfirm = HIGH;

    bool now = digitalRead(CONFIRMATION);

    if (lastConfirm == HIGH && now == LOW) {
        if (canConfirmAction()) {
            ap_cost();
            updateAPLEDs();
            updateLocationLEDs(true);
            performEventAction();
            performLocationAction();
            updateSkillScreen();
            updateScreen();
        }
        else{
            updateLocationLEDs(true);
            screenShowWarning();
            updateScreen();
        }
            
    }
    lastConfirm = now;
}

//the function which can quickly reset the game with the tick of the up and down button together for 5s
void quickReset(){
    static bool triggered = false;
    static unsigned long startTime = 0;

    bool upPressed   = digitalRead(GO_UP) == LOW;
    bool downPressed = digitalRead(GO_DOWN) == LOW;

    if (upPressed && downPressed) {
        if (startTime == 0) {
        startTime = millis(); 
        }

        if (!triggered && millis() - startTime >= 5000) {
            triggered = true;
            resetGame();       
        }
    }   else {
            startTime = 0;
            triggered = false;
    }
}

//the main loop of the game
void loop() {
    //1.test if the game need to reset or not
    quickReset();

    //2.test if the player dies
    if (isDie()){
        gameOver1();
        resetGame();
        return;
    }
    
    //3.test if the day ends
    if (isDayEnd()) {
        delay(1000);
        endDay();
        updateLocationLEDs(true);
        delay(4000); 
        //3.1 test if the game comes to the exam day
        if (isGameOver()) {
            constantLEDs();
            gameOver2();
            delay(10000);
            resetGame();
            return;  
        }
        updateAPLEDs();
        updateScreen();
        return;          
    }
    
    //4. daily function: the use of three switcher and update the location LEDs
    chooseSwitcher(); 
    confirmSwitcher();    
    updateLocationLEDs(false);

    delay(10);
}

//the function to reset the game with a set of function
void resetGame() {
    setupGame();
    examLEDs();
    initOLED();
    updateScreen();
    updateLocationLEDs(false);
    updateAPLEDs();
}