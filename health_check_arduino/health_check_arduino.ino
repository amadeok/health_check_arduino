#include <LiquidCrystal_I2C.h>

// Initialize LCD (I2C Address: 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// LED Pins
const int whiteLED = 5;    // Good: Everything is fine
const int greenLED = 6;    // 1-2 severity
const int orangeLED = 7;   // 3-4 severity
const int redLED = 8;      // 5-6 severity

// Buzzer Pin
const int buzzer = 9;

int questionIndex = 0;
bool startCheckup = false;
int healthScore = 0;

// Defining the Buttons
const int yesButton = 4;
const int noButton = 3;
const int resetButton = 2;

// Structure for questions with severity values
struct Question {
  String text;
  int yesWeight;  // Severity points added if Yes is pressed
  int noWeight;   // Severity points added if No is pressed
};

// Define health questions
Question questions[] = {

  { "Do you have a fever?", 1, 0 },
  { "Do you have a headache?", 1, 0 },
  { "Are you experiencing nausea?", 1, 0 },
          { "Do you feel unusually tired?", 1, 0 },

          { "Do you feel tired?", 1, 0 },
    { "Have you had a headache?", 1, 0 },
    { "Do you feel stressed?", 1, 0 },
    { "Have you had a sore throat?", 1, 0 },
    { "Do you feel thirsty?", 1, 0 },
    { "Have you had muscle pain?", 1, 0 },
    { "Do you struggle to sleep?", 1, 0 },
    { "Have you felt dizzy?", 1, 0 },
    { "Do you feel bloated?", 1, 0 },
    { "Have you felt fatigued?", 1, 0 },
      
};

const int totalQuestions = sizeof(questions) / sizeof(questions[0]);

void setup() {
  pinMode(yesButton, INPUT_PULLUP);
  pinMode(noButton, INPUT_PULLUP);
  pinMode(resetButton, INPUT_PULLUP);

  pinMode(whiteLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(orangeLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  digitalWrite(whiteLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(orangeLED, LOW);
  digitalWrite(redLED, LOW);
  digitalWrite(buzzer, LOW);

  lcd.init();
  lcd.backlight();
  lcd.print("Start General");
  lcd.setCursor(0, 1);
  lcd.print("Health Checkup?");
}

void loop() {
  if (!startCheckup) {
    if (digitalRead(yesButton) == LOW) {  
      buttonBeep(100);
      delay(200);  
      startCheckup = true;
      lcd.clear();
      askQuestion();
    }
    return;
  }

  if (questionIndex < totalQuestions) {
    if (digitalRead(yesButton) == LOW) {
      buttonBeep(100);
      delay(200);
      healthScore += questions[questionIndex].yesWeight;
      nextQuestion();
    } 
    else if (digitalRead(noButton) == LOW) {
      buttonBeep(100);
      delay(200);
      healthScore += questions[questionIndex].noWeight;
      nextQuestion();
    }
  }

  if (digitalRead(resetButton) == LOW) {
    buttonBeep(500);
    delay(200);
    resetTest();
  }
}

// Function to play a beep sound when a button is pressed
void buttonBeep(int duration) {
  digitalWrite(buzzer, HIGH);
  delay(duration);  
  digitalWrite(buzzer, LOW);
}
void splitString(String text) {
  lcd.clear();
  int index = 0;
  
  if (text.length() <= 16) {
    lcd.setCursor(0, 0);
    lcd.print(text);
  } else {
    int breakIndex1 = text.lastIndexOf(' ', 15);
    if (breakIndex1 == -1) breakIndex1 = 16;
    
    lcd.setCursor(0, 0);
    lcd.print(text.substring(0, breakIndex1));
    
    if (text.length() > breakIndex1) {
      int breakIndex2 = text.lastIndexOf(' ', min(breakIndex1 + 17, text.length()));
      if (breakIndex2 == -1 || breakIndex2 <= breakIndex1) 
        breakIndex2 = breakIndex1 + 17;
      
      lcd.setCursor(0, 1);
      lcd.print(text.substring(breakIndex1 + 1, min(breakIndex2, text.length())));
      
      if (breakIndex2 >= text.length()) return;
      
      delay(1000);
      lcd.clear();
      
      if (text.length() > breakIndex2) {
        int breakIndex3 = text.lastIndexOf(' ', min(breakIndex2 + 17, text.length()));
        if (breakIndex3 == -1 || breakIndex3 <= breakIndex2) breakIndex3 = breakIndex2 + 17;
        
        lcd.setCursor(0, 0);
        lcd.print(text.substring(breakIndex2 + 1, min(breakIndex3, text.length())));
        
        if (text.length() > breakIndex3) {
          lcd.setCursor(0, 1);
          lcd.print(text.substring(breakIndex3 + 1, min(breakIndex3 + 16, text.length())));
        }
      }
    }
  }
}

void askQuestion() {
  lcd.clear();
  if (questionIndex < totalQuestions) {
    splitString(questions[questionIndex].text);
  } else {
    displayResult();
    healthScore = 0;
  }
}

void nextQuestion() {
  questionIndex++;
  if (questionIndex < totalQuestions) {
    askQuestion();
  } else {
    displayResult();
  }
}

void displayResult() {
  lcd.clear();
  lcd.print("Result:");
  lcd.setCursor(0, 1);

  digitalWrite(whiteLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(orangeLED, LOW);
  digitalWrite(redLED, LOW);

  if (healthScore <=1) {
    lcd.print("Good Health");
    digitalWrite(whiteLED, HIGH);
    delay(2000);
  } else if (healthScore > 1 && healthScore <= 2) {
    lcd.print("Take Rest");
    digitalWrite(greenLED, HIGH);
  } else if (healthScore >= 3 && healthScore < 4) {
    lcd.print("Take More Rest");
    digitalWrite(orangeLED, HIGH);
  } else if (healthScore >= 4) {
    lcd.print("See a Doctor!");
    digitalWrite(redLED, HIGH);
  }
}

void resetTest() {
  healthScore = 0;
  questionIndex = 0;
  startCheckup = false;

  digitalWrite(whiteLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(orangeLED, LOW);
  digitalWrite(redLED, LOW);

  lcd.clear();
  lcd.print("Start General");
  lcd.setCursor(0, 1);
  lcd.print("Health Checkup?");
}
