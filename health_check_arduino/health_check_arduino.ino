#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int whiteLED = 5;
const int greenLED = 6;
const int orangeLED = 7;
const int redLED = 8;
const int buzzer = 9;

const int yesButton = 4;
const int noButton = 3;
const int resetButton = 2;

int questionIndex = 0;
bool startCheckup = false;
int healthScore = 0;
String solutions = "";

struct Question {
  String text;
  int yesWeight;
  int noWeight;
  String solution;
};

Question questions[] = {
  { "Do you have a fever?", 1, 0, "Drink fluids, rest." },
  { "Do you have a headache?", 1, 0, "Take pain relievers." },
  { "Are you experiencing nausea?", 1, 0, "Stay hydrated." },
  { "Do you feel unusually tired?", 1, 0, "Get more sleep." },
  { "Do you feel tired?", 1, 0, "Eat nutritious food." },
  { "Have you had a headache?", 1, 0, "Avoid screen time." },
  { "Do you feel stressed?", 1, 0, "Practice meditation." },
  { "Have you had a sore throat?", 1, 0, "Gargle warm salt water." },
  { "Do you feel thirsty?", 1, 0, "Drink more water." },
  { "Have you had muscle pain?", 1, 0, "Try light stretching." },
  { "Do you struggle to sleep?", 1, 0, "Follow a sleep schedule." },
  { "Have you felt dizzy?", 1, 0, "Sit down, breathe deeply." },
  { "Do you feel bloated?", 1, 0, "Avoid heavy meals." },
  { "Have you felt fatigued?", 1, 0, "Exercise regularly." },
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
      solutions += "- " + questions[questionIndex].solution + "\n";
      nextQuestion();
    } else if (digitalRead(noButton) == LOW) {
      buttonBeep(100);
      delay(200);
      nextQuestion();
    }
  }

  if (digitalRead(resetButton) == LOW) {
    buttonBeep(500);
    delay(200);
    resetTest();
  }
}

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

  if (healthScore <= 1) {
    lcd.print("Good Health");
    digitalWrite(whiteLED, HIGH);
  } else if (healthScore <= 2) {
    lcd.print("Take Rest");
    digitalWrite(greenLED, HIGH);
  } else if (healthScore < 4) {
    lcd.print("More Rest Needed");
    digitalWrite(orangeLED, HIGH);
  } else {
    lcd.print("See a Doctor!");
    digitalWrite(redLED, HIGH);
  }
  delay(3000);
  displaySolutions();
}

void displaySolutions() {
  if (solutions.length() > 0) {
    lcd.clear();
    lcd.print("Advice:");
    delay(2000);

    int start = 0;
    while (start < solutions.length()) {
      lcd.clear();
      splitString(solutions.substring(start, min(start + 32, solutions.length())));
      start += 32;
      delay(2500);
    }
  }
  resetTest();
}

void resetTest() {
  healthScore = 0;
  questionIndex = 0;
  startCheckup = false;
  solutions = "";

  digitalWrite(whiteLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(orangeLED, LOW);
  digitalWrite(redLED, LOW);
  lcd.clear();
  lcd.print("Start General");
  lcd.setCursor(0, 1);
  lcd.print("Health Checkup?");
}