#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include <MemoryFree.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
#define nbrAccount 15
enum states {
  Men,
  select
};
struct accAttribute {  // structure of an employee account
  long ID;
  int jobGrade;
  float Salary;
  String jobTitle;
  String penStatus;
} account;

accAttribute allAccounts[nbrAccount];  // array with all employee accounts
bool displayChange = false;
static int indexAccDisplayed = 0;
int indexMaxAcc = 0;

//1st we implement a function to print R every 2 seconds

void printR() {  // function that is in charge of printing R every 2 seconds

  static unsigned long timePassed = millis();
  if (millis() >= timePassed + 2000) {
    timePassed = millis();
    Serial.print(F("R"));
  }
}
//this function is the one to start the Program (synch phase)

void startFunction() {  // function that starts the programm by asking for BEGIN
  bool R = true;
  while (R = true) {
    printR();
    if (Serial.available() > 0) {
      String input = Serial.readString();
      input.trim();
      if (input == "BEGIN") {
        Serial.println(F("UDCHARS,FREERAM,SCROLL"));
        lcd.setBacklight(7);
        R = false;
        break;
      }
    }
  }
}

// The command string is for reading inputs until the end of the readstring


int startWord = 0;      // the start of each word in the command
int indexWord = 0;      // the index of each word in the command ex ADD index is 0
String typeCommand[4];  // array of separated input



void inputSeparation() {  // Separates the input in arrays based on "-"
  for (int j = 0; j < 4; j++) {
    typeCommand[j] = "";  //clears array
  }
  String command = Serial.readString();
  command.trim();
  // FOLLOWING code will populate the array above with the input separated by "-"
  int startWord = 0;  // the start of each word in the command
  int indexWord = 0;  // the index of each word in the command ex ADD index is 0
  for (int i = 0; i < 4; i++) {
    int endWord = command.indexOf('-', startWord);
    if (endWord == -1) {
      typeCommand[i] = command.substring(startWord);
      break;
    }
    typeCommand[i] = command.substring(startWord, endWord);
    startWord = endWord + 1;
  }
}

bool numbersOnly(String str) {  // function to check that all charachters are integers
  bool hasDecimal = false;
  for (unsigned int i = 0; i < str.length(); i++) {
    char c = str[i];
    if (c == '.') {
      if (hasDecimal) {
        return false;
      }
      hasDecimal = true;
    } else if (!isDigit(c)) {
      return false;
    }
  }
  return true;
}

bool validJobTitle(int index) {
  int isIntegerOrAlpha = 0;
  if (typeCommand[index].length() > 2 && typeCommand[index].length() < 18) {
    for (int c = 0; c < typeCommand[index].length(); c++) {

      if (isDigit(typeCommand[index][c]) || isAlpha(typeCommand[index][c]) || typeCommand[index][c] == '_' || typeCommand[index][c] == '.') {

        isIntegerOrAlpha += 1;
      } else {
      }
    }
    if (isIntegerOrAlpha == typeCommand[index].length()) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

bool inputValidity() {            // Function that checks wether the inputs are valid
  if (typeCommand[0] == "ADD") {  //do the following if this line is true
    if (typeCommand[1].length() == 7 && numbersOnly(typeCommand[1])) {
      if (typeCommand[2].length() == 1 && typeCommand[2].toInt() > 0) {
        if (validJobTitle(3)) {
          return true;
        } else {
          return false;
        }
      } else {  // else for if typecommand1 is not 7 figures
        return false;
      }
    } else {
      return false;
    }
  } else if (typeCommand[0] == "PST") {  //do the following if this l.istrue.
    if (typeCommand[1].length() == 7 && numbersOnly(typeCommand[1])) {
      if (typeCommand[2] == "PEN" || typeCommand[2] == "NPEN") {
        if ((typeCommand[3].trim(), typeCommand[3].length() == 0)) {
          return true;
        } else {
          return false;
        }
      } else {
        return false;
      }
    } else {
      return false;
    }
  } else if (typeCommand[0] == "SAL") {
    if (typeCommand[1].length() == 7 && numbersOnly(typeCommand[1])) {
      if (typeCommand[2].toFloat() > 0 && typeCommand[2].toFloat() < 100000.00 && numbersOnly(typeCommand[2])) {
        if ((typeCommand[3].trim(), typeCommand[3].length() == 0)) {
          return true;
        } else {
          return false;
        }
      } else {
        return false;
      }
    } else {
      return false;
    }
  } else if (typeCommand[0] == "GRD") {
    if (typeCommand[1].length() == 7 && numbersOnly(typeCommand[1])) {
      if (typeCommand[2].length() == 1 && typeCommand[2].toInt() > 0) {
        if ((typeCommand[3].trim(), typeCommand[3].length() == 0)) {
          return true;
        } else {
          return false;
        }
      } else {
        return false;
      }
    } else {
      return false;
    }
  } else if (typeCommand[0] == "CJT") {
    if (typeCommand[1].length() == 7 && numbersOnly(typeCommand[1])) {
      if (validJobTitle(2)) {
        if ((typeCommand[3].trim(), typeCommand[3].length() == 0)) {
          return true;
        } else {
          return false;
        }
      } else {
        return false;
      }
    } else {
      return false;
    }

  } else if (typeCommand[0] == "DEL") {
    if (typeCommand[1].length() == 7 && numbersOnly(typeCommand[1])) {
      if ((typeCommand[2].trim(), typeCommand[2].length() == 0)) {
        if ((typeCommand[3].trim(), typeCommand[3].length() == 0)) {
          return true;
        } else {
          return false;
        }
      } else {
        return false;
      }
    } else {
      return false;
    }

  } else {  // else for if typecommand1 is not 7 figures
    return false;
  }
}





void sortAccounts() {
  for (int i = 0; i < indexMaxAcc; i++) {
    for (int j = 0; j < indexMaxAcc - i; j++) {
      if (allAccounts[j].ID > allAccounts[j + 1].ID) {
        // Swap allAccounts[j] and allAccounts[j + 1]
        accAttribute temp = allAccounts[j];
        allAccounts[j] = allAccounts[j + 1];
        allAccounts[j + 1] = temp;
      } else {
      }
    }
  }
}



static int p = 0;  // the index of the new account to be added
void add() {
  char canAdd = '\0';
  if (p > 14) {

    Serial.println(F("ERROR: you have reached the maximum number of account"));

  } else {

    for (int i = 0; i < 15; i++) {

      if (typeCommand[1].toInt() == allAccounts[i].ID) {
        canAdd = 'n';
        break;
      }
    }
    if (canAdd == '\0') {
      account.ID = typeCommand[1].toInt();
      account.jobGrade = typeCommand[2].toInt();
      typeCommand[3].trim();
      account.jobTitle = typeCommand[3];
      account.penStatus = " PEN";
      account.Salary = 0.00;
      allAccounts[p] = account;  // assign this account to the index p
      p += 1;                    // increase to next index for adding the other account

    }

    else {
      Serial.println(F("ERROR: You can't create an account with existing ID number"));
    }
  }
}


void sal() {
  //convert f=2.dp
  float typeCommandFloat = (typeCommand[2].toFloat() * 100) / 100.0;
  char matchID = '\0';  // char to check that the salary input match any acc ID
  for (int i = 0; i < 15; i++) {
    if (typeCommand[1].toInt() == allAccounts[i].ID) {
      allAccounts[i].Salary = typeCommandFloat;
      matchID = 'y';
      break;
    } else {
    }
  }
  if (matchID == '\0') {
    Serial.println(F("ERROR: No account has a matching ID"));
  } else {
  }
}

void pst() {
  bool validityChangePen = false;  // this is a variable that says wether you can change pension (true) or you can not (false)
  char reasonOfInvalidity = '\0';  // this is a variable that will tell you why you can or can not then based on the reason, print the appropriate ERROR message
  for (int i = 0; i < 15; i++) {   //Go through each account and check if conditions are met
    if (typeCommand[1].toInt() == allAccounts[i].ID) {
      if (allAccounts[i].Salary > 0) {
        if (typeCommand[2] == "PEN") {
          typeCommand[2] = " " + typeCommand[2];
        }
        if (typeCommand[2] != allAccounts[i].penStatus) {
          allAccounts[i].penStatus = typeCommand[2];  // This allows me to keep the account whish satisfy all the values for changing pension status
          validityChangePen = true;
          reasonOfInvalidity = 'x';  // no reason -> (valid)
          break;
        } else {
          reasonOfInvalidity = 'a';  // the pension status is the same
          break;
        }
      } else {
        reasonOfInvalidity = 'b';  // the salary is not bigger than 0
        break;
      }
    } else {
      reasonOfInvalidity = 'i';
    }
  }
  if (validityChangePen == false) {
    if (reasonOfInvalidity == 'a') {
      Serial.println(F("ERROR: Pension Status not changed"));
    } else if (reasonOfInvalidity == 'b') {
      Serial.println(F("ERROR: This employee's salary is still £0.00"));
    } else if (reasonOfInvalidity == 'c') {
      Serial.println(F("ERROR: the account details are incomplete"));
    } else if (reasonOfInvalidity == 'i') {
      Serial.println(F("ERROR: no matching accound ID"));
    } else {
      Serial.println(F("ERROR: he account details are incomplete"));
    }
  }
}


void grd() {
  bool validityChangeGrd = false;
  char reasonOfInvalidity = '\0';  // char to check that the input ID match any acc ID

  for (int i = 0; i < 15; i++) {
    if (typeCommand[1].toInt() == allAccounts[i].ID) {
      if (allAccounts[i].jobGrade < typeCommand[2].toInt()) {
        allAccounts[i].jobGrade = typeCommand[2].toInt();
        reasonOfInvalidity = 'n';  // n for no reason. i.e. it's working fine
        validityChangeGrd = true;
        break;
      } else if (allAccounts[i].jobGrade > typeCommand[2].toInt()) {
        reasonOfInvalidity = 'l';  // l for the new jobGrade is lower than old
      } else if (allAccounts[i].jobGrade == typeCommand[2].toInt()) {
        reasonOfInvalidity = 's';  // s for the new jobGrade is the same as old
      } else {
        reasonOfInvalidity = 'c';  // c for incompleteness of the input
      }
    }
  }
  if (validityChangeGrd == false) {
    if (reasonOfInvalidity == 's') {
      Serial.println(F("ERROR: The job grade can't be modified to the same value"));
    } else if (reasonOfInvalidity == 'l') {
      Serial.println(F("ERROR: The job grade can't be modified to a lower value"));
    } else if (reasonOfInvalidity == 'c') {
      Serial.println(F("ERROR: the account details are incomplete"));
    } else {
      Serial.println(F("ERROR: No account has a matching ID"));
    }
  }
}

void cjt() {
  char matchID = '\0';
  for (int i = 0; i < 15; i++) {
    if (typeCommand[1].toInt() == allAccounts[i].ID) {
      allAccounts[i].jobTitle = typeCommand[2];
      matchID = 'y';
      break;
    } else {
      matchID = 'n';
    }
  }
  if (matchID == 'n') {
    Serial.println(F("ERROR: No account has a matching ID"));
  } else {
  }
}



byte arrowUp[8] = {
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100
};

byte arrowDown[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b10101,
  0b01110,
  0b00100
};



void screenDisplay() {
  sortAccounts();
  if (displayChange == true) {
    lcd.clear();
    displayChange = false;
  }
  int numberOfValidAccounts = 0;
  for (int k = 0; k < 15; k++) {
    if (allAccounts[k].jobGrade != 0) {
      numberOfValidAccounts++;
    } else {
    }
  }
  indexMaxAcc = numberOfValidAccounts - 1;
  if (numberOfValidAccounts > 0) {
    if (allAccounts[indexAccDisplayed].jobGrade > 0) {
      if (allAccounts[indexAccDisplayed].penStatus == "NPEN") {
        lcd.setBacklight(1);
      } else {
        lcd.setBacklight(2);
      }
      lcd.setCursor(1, 0);
      lcd.print(allAccounts[indexAccDisplayed].jobGrade);
      lcd.setCursor(8, 0);
      lcd.print(allAccounts[indexAccDisplayed].Salary);
      lcd.setCursor(3, 0);
      lcd.print(allAccounts[indexAccDisplayed].penStatus);
      lcd.setCursor(1, 1);
      lcd.print(allAccounts[indexAccDisplayed].ID);
      lcd.setCursor(9, 1);
      if (allAccounts[indexAccDisplayed].jobTitle.length() > 7) {
        scroll();
      } else {
        lcd.print(allAccounts[indexAccDisplayed].jobTitle);
      }
      if (indexAccDisplayed == 0) {
        if (indexMaxAcc > 0) {
          lcd.createChar(1, arrowDown);
          lcd.setCursor(0, 1);
          lcd.write((uint8_t)1);
          lcd.setCursor(0, 0);
          lcd.print(" ");
        } else {
        }

      } else if (indexAccDisplayed == indexMaxAcc) {
        lcd.createChar(0, arrowUp);
        lcd.setCursor(0, 0);
        lcd.write((uint8_t)0);
        lcd.setCursor(0, 1);
        lcd.print(" ");

      } else {
        lcd.createChar(0, arrowUp);
        lcd.createChar(1, arrowDown);
        lcd.setCursor(0, 0);
        lcd.write((uint8_t)0);
        lcd.setCursor(0, 1);
        lcd.write((uint8_t)1);
      }
    } else {
    }
  } else {
    lcd.setBacklight(7);
  }
}

void scroll() {
  static int i = 0;
  static int j = 8;
  static unsigned long timePassed = millis();
  if (millis() >= timePassed + 1000) {
    timePassed = millis();
    static int k = 0;
    //allAccounts[indexAccDisplayed][k] = allAccounts[indexAccDisplayed][k + 1];
    lcd.setCursor(9, 1);
    lcd.print(allAccounts[indexAccDisplayed].jobTitle.substring(i, j));
    if (j < allAccounts[indexAccDisplayed].jobTitle.length()) {
      i++;
      j++;
    } else {
      i = 0;
      j = 8;
    }
  }
}



void del() {
  char matchID = '\0';
  for (int a = 0; a < 15; a++) {

    if (typeCommand[1].toInt() == allAccounts[a].ID) {
      allAccounts[a].ID = 10000000;
      allAccounts[a].jobGrade = 0;
      allAccounts[a].Salary = 0.0;
      allAccounts[a].jobTitle = "JobTitle";
      allAccounts[a].penStatus = " PEN";
      allAccounts[14] = allAccounts[a];

      for (int j = a; j < 14; j++) {
        allAccounts[j] = allAccounts[j + 1];
      }

      p -= 1;  // setting the index back by 1 for for future new account

      if (indexMaxAcc == a && indexMaxAcc != 0) {
        indexAccDisplayed -= 1;
      } else {
      }
      matchID = 'y';
      break;

    } else {
      matchID = 'n';
    }
  }

  if (matchID == 'n') {
    Serial.println(F("ERROR: No account has a matching ID"));
  } else {
  }
}

//extern int __heap_start, *__brkval;
//int freeRam() {
//int availableFR;
//if ((int)__brkval == 0) {
//return (int)&availableFR - (int)&__heap_start;
//} else {
//return (int)&availableFR - (int)__brkval;
//}
//return availableFR
//}













void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setBacklight(3);
  startFunction();
  for (int l = 0; l < 15; l++) {
    allAccounts[l].ID = 10000000;
    allAccounts[l].jobGrade = 0;
    allAccounts[l].Salary = 0.0;
    allAccounts[l].jobTitle = "JobTitle";
    allAccounts[l].penStatus = " PEN";
  }
  displayChange = true;
}



void loop() {
  uint8_t buttons = lcd.readButtons();
  static bool selectPressFirstTime = true;
  static unsigned long selectTimer = millis();
  static states state = Men;



  switch (state) {

    case Men:
      {
        if (Serial.available() > 0) {
          inputSeparation();
          if (inputValidity()) {
            if (typeCommand[0] == "ADD") {
              displayChange = true;
              add();
            } else if (typeCommand[0] == "PST") {
              displayChange = true;
              pst();
            } else if (typeCommand[0] == "GRD") {
              grd();
            } else if (typeCommand[0] == "SAL") {
              displayChange = true;
              sal();
            } else if (typeCommand[0] == "CJT") {
              displayChange = true;
              cjt();

            } else {
              displayChange = true;
              del();
            }
          }

          else {
            Serial.println(F("ERROR: Input is invalid"));
          }
        }

        screenDisplay();
        if (buttons & BUTTON_LEFT) {
          lcd.scrollDisplayRight();
        }
        if (buttons & BUTTON_DOWN) {
          if (indexAccDisplayed < indexMaxAcc) {
            displayChange = true;
            indexAccDisplayed = indexAccDisplayed + 1;
            screenDisplay();
          } else {
            Serial.println(F("ERROR: you are at the last account"));
            screenDisplay();
          }
        }
        if (buttons & BUTTON_RIGHT) {
          lcd.scrollDisplayLeft();  // scroll the job title
        }
        if (buttons & BUTTON_UP) {
          if (indexAccDisplayed > 0) {
            displayChange = true;
            indexAccDisplayed = indexAccDisplayed - 1;
            screenDisplay();
          } else {
            Serial.println(F("ERROR: you are at the first account"));
            screenDisplay();
          }
        }
        if (buttons & BUTTON_SELECT) {
          if (selectPressFirstTime) {
            selectPressFirstTime = false;
            selectTimer = millis();
          } else {
            if (millis() >= selectTimer + 1000) {
              state = select;
              lcd.setBacklight(5);
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print(F("F432252"));
              lcd.setCursor(0, 1);
              lcd.print(freeMemory());
            }
          }
        }
      }
      break;

    case select:
      if (!(buttons & BUTTON_SELECT)) {
        selectPressFirstTime = true;
        displayChange = true;
        state = Men;
      }
      break;
  }
}


//if (isDigit(typeCommand[3][c]) || isAlpha(typeCommand[3][c]) || typeCommand[3][c] == "_" || typeCommand[3][c] == ".") PREVIOUS CHECK FOR VALIDITY OF JT
