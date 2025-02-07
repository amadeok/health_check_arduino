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

struct Question {
  char* text;
  int yesWeight;
  int noWeight;
  char* solution;
};

Question questions[] = {
  { "Do you feel tired?", 1, 0, "Get more sleep, stay hydrated, and eat balanced meals." },
  { "Have you had a headache?", 2, 0, "Drink water, rest in a dark room, and reduce screen time." },
  { "Do you feel stressed?", 1, 0, "Take deep breaths, go for a walk, and try relaxation techniques." },
  { "Have you had a sore throat?", 2, 0, "Drink warm liquids, gargle salt water, and rest your voice." },
  { "Have you had muscle pain?", 2, 0, "Stretch, apply a warm compress, and rest your muscles." },
  { "Do you have a fever?", 3, 0, "Rest, drink fluids, and take fever-reducing medicine if needed." },
  { "Do you have a headache?", 2, 0, "Stay hydrated, rest in a quiet place, and reduce screen time." }
};

const int totalQuestions = sizeof(questions) / sizeof(questions[0]);

bool solutionsNeeded[totalQuestions];

void setup() {
  Serial.begin(9600);
  for (int n = 0; n < totalQuestions; n++) {
    solutionsNeeded[n] = false;
  }
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
      solutionsNeeded[questionIndex] = true;
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

void splitString(char* text) {
  lcd.clear();
  int index = 0;
  
  // Find the length of the text manually
  int textLength = 0;
  while (text[textLength] != '\0') {
    textLength++;
  }

  if (textLength <= 16) {
    lcd.setCursor(0, 0);
    lcd.print(text);
  } else {
    // Find the first space before index 16 to break the string
    int breakIndex1 = 15;
    while (breakIndex1 >= 0 && text[breakIndex1] != ' ') {
      breakIndex1--;
    }
    if (breakIndex1 == -1) breakIndex1 = 16;

    // Print the first line
    lcd.setCursor(0, 0);
    char line1[17]; // Make sure it's 17 to hold the null-terminator
    strncpy(line1, text, breakIndex1);
    line1[breakIndex1] = '\0'; // Null-terminate the string
    lcd.print(line1);

    if (textLength > breakIndex1) {
      // Find the space before the next break point
      int breakIndex2 = breakIndex1 + 16;
      while (breakIndex2 < textLength && text[breakIndex2] != ' ') {
        breakIndex2--;
      }
      if (breakIndex2 == breakIndex1 + 16) breakIndex2 = breakIndex1 + 16;

      // Print the second line
      lcd.setCursor(0, 1);
      char line2[17];
      strncpy(line2, text + breakIndex1 + 1, breakIndex2 - breakIndex1);
      line2[breakIndex2 - breakIndex1] = '\0';
      lcd.print(line2);

      if (breakIndex2 >= textLength) return;

      delay(2000);
      lcd.clear();

      if (textLength > breakIndex2) {
        // Find the space before the third break point
        int breakIndex3 = breakIndex2 + 16;
        while (breakIndex3 < textLength && text[breakIndex3] != ' ') {
          breakIndex3--;
        }
        if (breakIndex3 == breakIndex2 + 16) breakIndex3 = breakIndex2 + 16;

        // Print the third line
        lcd.setCursor(0, 0);
        char line3[17];
        strncpy(line3, text + breakIndex2 + 1, breakIndex3 - breakIndex2);
        line3[breakIndex3 - breakIndex2] = '\0';
        lcd.print(line3);

        if (textLength > breakIndex3) {
          // Print the fourth line
          lcd.setCursor(0, 1);
          char line4[17];
          strncpy(line4, text + breakIndex3 + 1, 16);
          line4[16] = '\0';
          lcd.print(line4);
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
  bool displayAdvice = false;

  for (int n = 0; n < totalQuestions; n++)
    if (solutionsNeeded[n]) {
      displayAdvice = true;
      break;
    }

  if (displayAdvice) {
    lcd.clear();
    lcd.print("Advice:");
    delay(2000);

    for (int n = 0; n < totalQuestions; n++)
      if (solutionsNeeded[n]) {
        lcd.clear();
        splitString(questions[n].solution);
        delay(3000);
      }
  }

  resetTest();
}

void resetTest() {
  for (int n = 0; n < totalQuestions; n++)
    solutionsNeeded[n] = false;

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
