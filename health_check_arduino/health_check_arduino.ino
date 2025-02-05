#include <LiquidCrystal_I2C.h>

// Initialize LCD (I2C Address: 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// LED Pins
const int whiteLED = 5;    // Good: Everything is fine
const int greenLED = 6;    // 1-2 severity
const int orangeLED = 7;   // 3-4 severity
const int redLED = 8;      // 5-6 severity

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
  { "Do you have a fever?", 3, 0 },
  { "Do you have a headache?", 1, 0 },
  { "Do you have nausea?", 2, 0 },
  { "Do you feel unusually tired?", 1, 0 }
};

const int totalQuestions = sizeof(questions) / sizeof(questions[0]);

void setup() {
  // Initialize buttons with internal pull-ups
  pinMode(yesButton, INPUT_PULLUP);
  pinMode(noButton, INPUT_PULLUP);
  pinMode(resetButton, INPUT_PULLUP);

  // Initialize LED outputs
  pinMode(whiteLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(orangeLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  // Turn off all LEDs at start
  digitalWrite(whiteLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(orangeLED, LOW);
  digitalWrite(redLED, LOW);

Serial.println("before LCD begin");
  // Initialize LCD
  lcd.init();
  lcd.backlight();  // Ensure LCD is visible
  lcd.print("Start General");
  lcd.setCursor(0, 1);
  lcd.print("Health Checkup?");
}
void loop() {
  // Before checkup starts, wait for Yes button to start
  if (!startCheckup) {
    if (digitalRead(yesButton) == LOW) {  // Button active LOW
      delay(200);  // Debounce delay
      startCheckup = true;
      lcd.clear();
      askQuestion();
    }
    return;
  }

  // During checkup, check for answers
  if (questionIndex < totalQuestions) {
    if (digitalRead(yesButton) == LOW) {
      delay(200);
      healthScore += questions[questionIndex].yesWeight;
      nextQuestion();
    } 
    else if (digitalRead(noButton) == LOW) {
      delay(200);
      healthScore += questions[questionIndex].noWeight;
      nextQuestion();
    }
  }

  // Allow a reset anytime during the process
  if (digitalRead(resetButton) == LOW) {
    delay(200);
    resetTest();
  }
}

void splitString(String input, int maxLength) {
    int start = 0;
      lcd.setCursor(0, 0);

    while (start < input.length()) {
        int end = start + maxLength;

        if (end >= input.length()) {  // Last chunk
            lcd.setCursor(0, 1);
            lcd.print(input.substring(start));
            break;
        }

        int lastSpace = input.lastIndexOf(' ', end);
        if (lastSpace < start) {  // No space found in the current chunk
            lastSpace = end;  // Force split at maxLength
        }

        lcd.print(input.substring(start, lastSpace));
        start = lastSpace + 1;
    }
}

void askQuestion() {
  lcd.clear();
  if (questionIndex < totalQuestions) {
    splitString(questions[questionIndex].text, 16);
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
  digitalWrite(whiteLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(orangeLED, LOW);
  digitalWrite(redLED, LOW);

  // Determine LED based on severity score
  if (healthScore == 0) {
    lcd.print("Good Health");
    digitalWrite(whiteLED, HIGH);
  } else if (healthScore >= 1 && healthScore <= 2) {
    lcd.print("Take Rest");
    digitalWrite(greenLED, HIGH);
  } else if (healthScore >= 3 && healthScore <= 4) {
    lcd.print("Take More Rest");
    digitalWrite(orangeLED, HIGH);
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
  digitalWrite(whiteLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(orangeLED, LOW);
  digitalWrite(redLED, LOW);

  // Reset LCD message
  lcd.clear();
  lcd.print("Start General");
  lcd.setCursor(0, 1);
  lcd.print("Health Checkup?");
}