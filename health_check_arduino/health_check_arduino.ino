
#include <LiquidCrystal.h>

// LCD Pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Button Pins
const int yesButton = 2;
const int noButton = 3;
const int resetButton = 4;
// LED Pins
const int blueLED = 5;     // Good: Everything is fine
const int greenLED = 6;    // 1-2 severity
const int yellowLED = 13;  // 3-4 severity
const int redLED = A0;     // 5-6 severity

int questionIndex = 0;
bool startCheckup = false;
int healthScore = 0;

// Structure for questions with customizable severity values for Yes and No
struct Question {
  String text;
  int yesWeight;  // Severity points added if Yes is pressed
  int noWeight;   // Severity points added if No is pressed
};

// Define your questions and the severity weights here
Question questions[] = {
  // Example: If you have a fever, add 3 severity points if Yes.
  { "Do you have a fever?", 3, 0 },
  // Example: Headache counts as 1 severity point.
  { "Do you have a headache?", 1, 0 },
  // You can add more questions as needed. For instance:
  { "Are you experiencing nausea?", 2, 0 },
  { "Do you feel unusually tired?", 1, 0 }
};

const int totalQuestions = sizeof(questions) / sizeof(questions[0]);

void setup() {
  // Initialize buttons with internal pullups
  pinMode(yesButton, INPUT_PULLUP);
  pinMode(noButton, INPUT_PULLUP);
  pinMode(resetButton, INPUT_PULLUP);
  // Initialize LED outputs
  pinMode(blueLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  // Turn off all LEDs at start
  digitalWrite(blueLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, LOW);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("Start General");
  lcd.setCursor(0, 1);
  lcd.print("health check up?");
}

void loop() {
  // Before checkup starts, wait for Yes button to start
  if (!startCheckup) {
    if (digitalRead(yesButton) == LOW) {
      delay(200);  // Debounce delay
      startCheckup = true;
      lcd.clear();
      askQuestion();
    }
    return;
  }

  // During the checkup, check for answers
  if (questionIndex < totalQuestions) {
    if (digitalRead(yesButton) == LOW) {
      delay(200);  // Debounce delay
      healthScore += questions[questionIndex].yesWeight;
      nextQuestion();
    } else if (digitalRead(noButton) == LOW) {
      delay(200);  // Debounce delay
      healthScore += questions[questionIndex].noWeight;
      nextQuestion();
    }
  }

  // Allow a reset anytime during the process
  if (digitalRead(resetButton) == LOW) {
    delay(200);  // Debounce delay
    resetTest();
  }
}

void askQuestion() {
  lcd.clear();
  if (questionIndex < totalQuestions) {
    lcd.print(questions[questionIndex].text);
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

  // Turn off all LEDs first
  digitalWrite(blueLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, LOW);
  // Determine LED based on severity score
  if (healthScore == 0) {
    lcd.print("Good Health");
    digitalWrite(blueLED, HIGH);
  } else if (healthScore >= 1 && healthScore <= 2) {
    lcd.print("Take Rest");
    digitalWrite(greenLED, HIGH);
  } else if (healthScore >= 3 && healthScore <= 4) {
    lcd.print("Take More Rest");
    digitalWrite(yellowLED, HIGH);
  } else if (healthScore >= 5) {
    lcd.print("See a Doctor!");
    digitalWrite(redLED, HIGH);
  }
}

void resetTest() {
  healthScore = 0;
  questionIndex = 0;
  startCheckup = false;

  // Turn off all LEDs
  digitalWrite(blueLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, LOW);

  // Reset LCD message
  lcd.clear();
  lcd.print("Start General");
  lcd.setCursor(0, 1);
  lcd.print("health check up?");
}