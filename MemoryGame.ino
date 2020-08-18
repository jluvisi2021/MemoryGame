#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define RED 6
#define GREEN 9
#define BLUE 10
#define SWITCH 7
#define SUBMIT 8
#define BUZZER 13

/*
 * Pins on Arduino:
 * 2 = Occupied, LCD
 * 3 = Occupied, LCD
 * 4 = Occupied, LCD
 * 5 = Occupied, LCD
 * 6 = Occupied, RGB LED (RED)
 * 9 = Occupied, RGB LED (GREEN)
 * 10 = Occupied, RGB LED (Blue)
 * 7 = Button Color Switcher
 * 8 = Button Submit
 * 11 = Occupied, LCD
 * 12 = Occupied, LCD
 * 13 = Speaker
 */
/* Game
 * User clicks on button color switcher to change the color
 * of the led after the pattern has been shown.
 * Once they have finished selecting their color they click
 * button submit.
 * Once the user has submitted all of the values required for the pattern the system checks if the pattern is correct.
 * If the pattern is not correct a "You Lose" message is displayed and the values are reset.
 * This program always runs until the power is cut off.
 */
// Define pattern that will hold values RGB
String pattern = "";
// How large the pattern currently is.
int patternSize = 20;
char color;

void printLCD(String s1);
void printLCD(String s1, String s2);
void flashRGB(byte r, byte g, byte b, int duration);
void setupPattern();
void sout(String str); // System Out print function
void buzz(int d);
void flashLEDWithPattern();
void cycleLED();

void setup()
{
    Serial.begin(9600);
    pinMode(RED, OUTPUT);
    pinMode(BLUE, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(SWITCH, INPUT_PULLUP);
    pinMode(SUBMIT, INPUT_PULLUP);
    pinMode(BUZZER, OUTPUT);
    lcd.begin(16, 2);
    sout("Pins have been setup");
}

/**
 * Function to test all of the parts.
 */

void testFunctions()
{
    sout("Beginning testFunctions() in 10 seconds.");
    delay(10000);
    sout("Testing... printLCD(1)");
    printLCD("Hello");
    delay(4000);
    sout("Testing... printLCD(2)");
    printLCD("Line 1", "Line 2");
    delay(4000);
    sout("Testing... buzz()");
    buzz(1500);
    delay(4000);
    sout("Testing... flashRGB");
    sout("RED");
    flashRGB(255, 0, 0, 2000);
    delay(2000);
    sout("GREEN");
    flashRGB(0, 255, 0, 2000);
    delay(2000);
    sout("BLUE");
    flashRGB(0, 0, 255, 2000);
    delay(4000);
    sout("Test Buttons (15 Seconds)");
    boolean buttonSwitchPressed = false;
    boolean buttonSubmitPressed = false;
    int attempts = 500;
    printLCD("Press BTN 1");
    while (!buttonSwitchPressed && attempts > 0) {
        if (digitalRead(SWITCH) == LOW) {
            buttonSwitchPressed = true;
        }
        attempts--;
        delay(10);
    }
    attempts = 500;
    delay(500);
    if (buttonSwitchPressed) {
        printLCD("Button One", "Pressed.");
    }
    else {
        printLCD("Button One", "No Data");
    }
    delay(8500);
    printLCD("Press BTN 2");
    while (!buttonSubmitPressed && attempts > 0) {
        if (digitalRead(SUBMIT) == LOW) {
            buttonSubmitPressed = true;
        }
        attempts--;
        delay(10);
    }
    attempts = 500;
    delay(500);
    if (buttonSwitchPressed) {
        printLCD("Button Two", "Pressed.");
    }
    else {
        printLCD("Button Two", "No Data");
    }
    delay(8500);
    printLCD("Testing Patterns");
    delay(1000);
    printLCD("Starting...");
    delay(3000);
    setupPattern();
    printLCD(pattern);
    patternSize++;
    delay(3000);
    setupPattern();
    printLCD(pattern);
    patternSize++;
    delay(3000);
    setupPattern();
    printLCD(pattern);
    patternSize++;
    delay(3000);
    setupPattern();
    printLCD(pattern);
    patternSize++;
    delay(3000);
    setupPattern();
    patternSize = 2;
    printLCD("Restarting...");
    delay(3500);
}

String lastN(String input, int n)
{
     return input.substring(input.length() - n);
}

/**
 * Run this when the program starts.
 */
void mainMenuSequence(int refreshDelay)
{
    while (digitalRead(SWITCH) == HIGH && digitalRead(SUBMIT) == HIGH) {
        printLCD("Memory Game", "Press to Start");
        delay(refreshDelay);
    }
}

void loop()
{
    mainMenuSequence(250);
    printLCD("Starting...");
    delay(2500);
    while (true) {
        printLCD("Watch LED");
        setupPattern();
        delay(2500);
        flashLEDWithPattern();
        printLCD("Input Pattern", "Size: " + (String)patternSize);
        String userPattern = "";
        int presses = 0;
        color = 'R';
        // Set the LED to red by default.
        flashRGB(255, 0, 0);
        while (presses != pattern.length()) {
            // While the user hasnt pressed submit
            while (digitalRead(SUBMIT) == HIGH) {
                if (digitalRead(SWITCH) == LOW) {
                    cycleLED();
                }

                delay(100);
            }
            buzz(100);
            userPattern.concat((String)color);
            printLCD("Sequence", (String)userPattern);
            // If the user characters are beginning to go off the screen then make the screen also scroll.
            if(userPattern.length() > 15) {
              printLCD("Sequence", (String)lastN(userPattern, 15));
            }
            delay(200);
            ++presses;
        }
        delay(3000);
        printLCD("Checking...");
        // Check to see if the pattern is correct.
        delay(1150);
        if (userPattern == pattern) {
            printLCD("Sequence", "Correct!");
            delay(4000);
            printLCD("Streak: " + (String)patternSize);
            delay(2500);
        }
        else {
          // The player has input the wrong pattern.
            printLCD("Wrong Pattern!");
            buzz(1000);
            delay(2000);
            printLCD("You Lost!", "Streak: " + (String)patternSize);
            flashRGB(0, 0, 0);
            delay(5000);
            pattern = "";
            userPattern = "";
            patternSize = 2;
            break;
        }
        // Reset the LED.
        flashRGB(0, 0, 0);
        userPattern = "";
        pattern = "";
        ++patternSize;
    }
}

// Cycles through the LED based on the current color.
void cycleLED()
{
    if (color == 'R') {
        flashRGB(0, 255, 0);
        color = 'G';
    }
    else if (color == 'G') {
        flashRGB(0, 0, 255);
        color = 'B';
    }
    else if (color == 'B') {
        flashRGB(255, 0, 0);
        color = 'R';
    }
}
/**
 * Gets the current pattern and flashes the LED.
 * 0.5 Second Delay.
 */
void flashLEDWithPattern()
{
    for (int i = 0; i < pattern.length(); i++) {
        if (pattern[i] == 'R') {
            flashRGB(255, 0, 0, 1500);
        }
        else if (pattern[i] == 'G') {
            flashRGB(0, 255, 0, 1500);
        }
        else if (pattern[i] == 'B') {
            flashRGB(0, 0, 255, 1500);
        }
        delay(500);
    }
}
/**
 * Serial print out function.
 */
void sout(String str)
{
    Serial.println();
    Serial.println(str);
}
/**
 * Print a line out to the LCD.
 * Single line appears on Row 0 only.
 */
void printLCD(String s1)
{
    sout("Printing to LCD");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(s1);
}

/**
 * Print out 2 lines to the LCD.
 */
void printLCD(String s1, String s2)
{
    sout("Printing to LCD");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(s1);
    lcd.setCursor(0, 1);
    lcd.print(s2);
}

/**
 * 5V through Speaker.
 */
void buzz(int d)
{
    sout("Using Buzzer");
    delay(20);
    digitalWrite(BUZZER, HIGH);
    delay(d);
    digitalWrite(BUZZER, LOW);
    sout("Finished using buzzer");
}

/**
 * Flash the RGB LED with 3 byte values and a specified time.
 */
void flashRGB(byte r, byte g, byte b, int duration)
{
    sout("Flashing RGB");
    delay(100);
    analogWrite(RED, r);
    analogWrite(GREEN, g);
    analogWrite(BLUE, b);
    delay(duration);
    analogWrite(RED, 0);
    analogWrite(GREEN, 0);
    analogWrite(BLUE, 0);
    sout("Finished flashing RGB");
}
/**
 * Flash the RGB LED with 3 byte values until the values are overwritten.
 */
void flashRGB(byte r, byte g, byte b)
{
    sout("Flashing RGB");
    delay(100);
    analogWrite(RED, 0);
    analogWrite(GREEN, 0);
    analogWrite(BLUE, 0);
    analogWrite(RED, r);
    analogWrite(GREEN, g);
    analogWrite(BLUE, b);
    sout("Finished flashing RGB");
}

/** 
 *  Will append to the string pattern "R", "G", or "B" values.
 *  That will be the pattern the user has to enter and the order
 *  in which the rgb led flashes.
 */
void setupPattern()
{
    pattern = "";
    for (int i = 0; i < patternSize; i++) {
        int val = random(3);
        switch (val) {
        case 0:
            pattern.concat("R");
            break;
        case 1:
            pattern.concat("G");
            break;
        case 2:
            pattern.concat("B");
            break;
        }
    }
    sout("Finished setting up pattern");
}
