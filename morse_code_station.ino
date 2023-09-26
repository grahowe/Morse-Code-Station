/**
 * A Morse code station for the Arduino. Uses any board from the Nano to the Uno.
 * 
 * The display is a 128*64 OLED display and it uses the 
 * AdaFruit OLED driver libraries: 
 * - Adafruit GFX
 * - Adafruit SSD1306
 * 
 * You can search for these libraries and install them from within the Arduino
 * IDE. From the IDE menu choose Sketch->Include Library->Library Manager
 * enter the names of the libraries above and click install for each of them.
 * 
 * Author: Mario Gianota July 2020
 * Revised by: Owen Graham, KE0SBX, September 2023
 */
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define SPEAKER_PIN 3 //PWM-enabled pin for tone generation
#define LED_PIN 13
#define CODE_BUTTON 2

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool codeButtonArmed;
bool codeButtonPressed;
unsigned long codeTime;
unsigned long startTime;
unsigned long lastButtonPressTime;
bool letterDecoded;
bool newWord;

// Time range of a dot in milliseconds
const unsigned int dotTimeMillisMin = 20;
const unsigned int dotTimeMillisMax = 150;

// Array to store the times of the code button presses
unsigned long buttonPressTimes[6];
int bptIndex;

int row;
int col;

void setup() {
  
  pinMode(CODE_BUTTON, INPUT);
  pinMode(SPEAKER_PIN, OUTPUT); //Our speaker is still an output
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(SPEAKER_PIN, LOW); //Initialize the speaker to LOW
  Serial.begin(9600);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println("SSD1306 allocation failed");
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.write("KE0SBX CW Station");
  display.drawLine(0, 10, display.width(), 10, SSD1306_WHITE);

  display.setCursor(0, 12);
  display.display();
  delay(100);
}

void loop() {
  scanButtons();
  if( millis() - lastButtonPressTime > 1600 && newWord == true ) {
    Serial.println("New word");
    display.write(' ');
    display.display();
    newWord = false;
  }else if( millis() - lastButtonPressTime > 600 && letterDecoded == false) {
    decodeButtonPresses();
    letterDecoded = true;
  }
}

void codeButtonDown() {
  tone(SPEAKER_PIN, 700); // You can adjust the "sidetone" frequency (e.g., 400 - 1000 Hz) as needed
  // It is set to 700Hz currently, but can be changed to match your preferred sidetone frequency
  digitalWrite(LED_PIN, HIGH);
  codeTime = millis() - startTime;
}

void codeButtonReleased() {
  noTone(SPEAKER_PIN); // Turn off the speaker
  digitalWrite(LED_PIN, LOW);

  // Most button bounces take less than 25 millis. If the code time
  // is greater than 25 millis then it was probably a legit button press
  if(codeTime > 25) {
    
    Serial.print("Code time: ");
    Serial.print(codeTime);
    Serial.println(" milliseconds");

    // Save codeTime
    buttonPressTimes[bptIndex] = codeTime;
    bptIndex++;
    if( bptIndex == 5 ) {
      
      for(int i=0; i<5; i++) {
        buttonPressTimes[bptIndex] = 0;
      }
    }
  }
}

void scanButtons() {
  if(! codeButtonArmed && digitalRead(CODE_BUTTON) == HIGH) {
    codeButtonArmed = true;
    // start timer
    startTime = millis();
    lastButtonPressTime = startTime;
    codeTime = 0;
    letterDecoded = false;
    newWord = true;
  }
  if( digitalRead(CODE_BUTTON) == HIGH ) {
    codeButtonPressed = true;
    codeButtonDown();
  }
  if( codeButtonPressed && digitalRead(CODE_BUTTON) == LOW ) {
    codeButtonPressed = false;
    codeButtonReleased();
    codeButtonArmed = false;
  }
}

void decodeButtonPresses() {
  Serial.print("DECODE LETTER: ");
  for(int i=0; i<bptIndex; i++) {
    if( isDot(buttonPressTimes[i]) ) 
      Serial.print(" DOT ");
    else if( isDash(buttonPressTimes[i]) )
      Serial.print(" DASH");
  }
  Serial.print("   ");
  char c = decodeMsg();
  Serial.print(c);
  Serial.println();
  display.write(c);
  display.display();
  bptIndex = 0;
  for(int i=0; i<5; i++) {
    buttonPressTimes[i] = 0;
  }
}

bool isDot(unsigned long t) {
  if(t >= dotTimeMillisMin && t <= dotTimeMillisMax)
    return true;
  return false;
}
bool isDash(unsigned long t) {
  if(t > dotTimeMillisMax)
    return true;
  return false;
}

void drawChar(char c) {
  display.write(c);
  display.display();
}
char decodeMsg() {
  char c = '>';
  if( isDot(buttonPressTimes[0]) && isDash(buttonPressTimes[1]) && buttonPressTimes[2] == 0 )
    c = 'A';
  else if( isDash(buttonPressTimes[0]) && isDot(buttonPressTimes[1]) && isDot(buttonPressTimes[2]) && isDot(buttonPressTimes[3]) && buttonPressTimes[4]== 0 )
    c = 'B';
  else if( isDash(buttonPressTimes[0]) && isDot(buttonPressTimes[1]) && isDash(buttonPressTimes[2]) && isDot(buttonPressTimes[3]) && buttonPressTimes[4]== 0 )
    c = 'C';
  else if( isDash(buttonPressTimes[0]) && isDot(buttonPressTimes[1]) && isDot(buttonPressTimes[2]) && buttonPressTimes[3]== 0 )
    c = 'D';
  else if( isDot(buttonPressTimes[0]) && buttonPressTimes[1] == 0 )
    c = 'E';
  else if( isDot(buttonPressTimes[0]) && isDot(buttonPressTimes[1]) && isDash(buttonPressTimes[2]) && isDot(buttonPressTimes[3]) && buttonPressTimes[4]== 0 )
    c = 'F';
  else if( isDash(buttonPressTimes[0]) && isDash(buttonPressTimes[1]) && isDot(buttonPressTimes[2]) && buttonPressTimes[3]== 0 )
    c = 'G';
  else if( isDot(buttonPressTimes[0]) && isDot(buttonPressTimes[1]) && isDot(buttonPressTimes[2]) && isDot(buttonPressTimes[3]) && buttonPressTimes[4]== 0 )
    c = 'H';
  else if( isDot(buttonPressTimes[0]) && isDot(buttonPressTimes[1]) && buttonPressTimes[2]== 0 )
    c = 'I';
  else if( isDot(buttonPressTimes[0]) && isDash(buttonPressTimes[1]) && isDash(buttonPressTimes[2]) && isDash(buttonPressTimes[3]) && buttonPressTimes[4]== 0 )
    c = 'J';
  else if( isDash(buttonPressTimes[0]) && isDot(buttonPressTimes[1]) && isDash(buttonPressTimes[2]) && buttonPressTimes[3]== 0 )
    c = 'K';
  else if( isDot(buttonPressTimes[0]) && isDash(buttonPressTimes[1]) && isDot(buttonPressTimes[2]) && isDot(buttonPressTimes[3]) && buttonPressTimes[4]== 0 )
    c = 'L';
  else if( isDash(buttonPressTimes[0]) && isDash(buttonPressTimes[1]) && buttonPressTimes[2]== 0 )
    c = 'M';
  else if( isDash(buttonPressTimes[0]) && isDot(buttonPressTimes[1]) && buttonPressTimes[2]== 0 )
    c = 'N';
  else if( isDash(buttonPressTimes[0]) && isDash(buttonPressTimes[1]) && isDash(buttonPressTimes[2]) && buttonPressTimes[3]== 0 )
    c = 'O';
  else if( isDot(buttonPressTimes[0]) && isDash(buttonPressTimes[1]) && isDash(buttonPressTimes[2]) && isDot(buttonPressTimes[3]) && buttonPressTimes[4]== 0 )
    c = 'P';
  else if( isDash(buttonPressTimes[0]) && isDash(buttonPressTimes[1]) && isDot(buttonPressTimes[2]) && isDash(buttonPressTimes[3]) && buttonPressTimes[4]== 0 )
    c = 'Q';
  else if( isDot(buttonPressTimes[0]) && isDash(buttonPressTimes[1]) && isDot(buttonPressTimes[2]) && buttonPressTimes[3]== 0 )
    c = 'R';
  else if( isDot(buttonPressTimes[0]) && isDot(buttonPressTimes[1]) && isDot(buttonPressTimes[2]) && buttonPressTimes[3]== 0 )
    c = 'S';
  else if( isDash(buttonPressTimes[0]) && buttonPressTimes[1]== 0 )
    c = 'T';
  else if( isDot(buttonPressTimes[0]) && isDot(buttonPressTimes[1]) && isDash(buttonPressTimes[2]) && buttonPressTimes[3]== 0 )
    c = 'U';
  else if( isDot(buttonPressTimes[0]) && isDot(buttonPressTimes[1]) && isDot(buttonPressTimes[2]) && isDash(buttonPressTimes[3]) && buttonPressTimes[4]== 0 )
    c = 'V';
  else if( isDot(buttonPressTimes[0]) && isDash(buttonPressTimes[1]) && isDash(buttonPressTimes[2]) && buttonPressTimes[3]== 0 )
    c = 'W';
  else if( isDash(buttonPressTimes[0]) && isDot(buttonPressTimes[1]) && isDot(buttonPressTimes[2]) && isDash(buttonPressTimes[3]) && buttonPressTimes[4]== 0 )
    c = 'X';
  else if( isDash(buttonPressTimes[0]) && isDot(buttonPressTimes[1]) && isDash(buttonPressTimes[2]) && isDash(buttonPressTimes[3]) && buttonPressTimes[4]== 0 )
    c = 'Y';
  else if( isDash(buttonPressTimes[0]) && isDash(buttonPressTimes[1]) && isDot(buttonPressTimes[2]) && isDot(buttonPressTimes[3]) && buttonPressTimes[4]== 0 )
    c = 'Z';
  else if( isDot(buttonPressTimes[0]) && isDash(buttonPressTimes[1]) && isDash(buttonPressTimes[2]) && isDash(buttonPressTimes[3]) && isDash(buttonPressTimes[4]))
    c = '1';
  else if( isDot(buttonPressTimes[0]) && isDot(buttonPressTimes[1]) && isDash(buttonPressTimes[2]) && isDash(buttonPressTimes[3]) && isDash(buttonPressTimes[4]))
    c = '2';
  else if( isDot(buttonPressTimes[0]) && isDot(buttonPressTimes[1]) && isDot(buttonPressTimes[2]) && isDash(buttonPressTimes[3]) && isDash(buttonPressTimes[4]))
    c = '3';
  else if( isDot(buttonPressTimes[0]) && isDot(buttonPressTimes[1]) && isDot(buttonPressTimes[2]) && isDot(buttonPressTimes[3]) && isDash(buttonPressTimes[4]))
    c = '4';
  else if( isDot(buttonPressTimes[0]) && isDot(buttonPressTimes[1]) && isDot(buttonPressTimes[2]) && isDot(buttonPressTimes[3]) && isDot(buttonPressTimes[4]))
    c = '5';
  else if( isDash(buttonPressTimes[0]) && isDot(buttonPressTimes[1]) && isDot(buttonPressTimes[2]) && isDot(buttonPressTimes[3]) && isDot(buttonPressTimes[4]))
    c = '6';
  else if( isDash(buttonPressTimes[0]) && isDash(buttonPressTimes[1]) && isDot(buttonPressTimes[2]) && isDot(buttonPressTimes[3]) && isDot(buttonPressTimes[4]))
    c = '7';
  else if( isDash(buttonPressTimes[0]) && isDash(buttonPressTimes[1]) && isDash(buttonPressTimes[2]) && isDot(buttonPressTimes[3]) && isDot(buttonPressTimes[4]))
    c = '8';
  else if( isDash(buttonPressTimes[0]) && isDash(buttonPressTimes[1]) && isDash(buttonPressTimes[2]) && isDash(buttonPressTimes[3]) && isDot(buttonPressTimes[4]))
    c = '9';
  else if( isDash(buttonPressTimes[0]) && isDash(buttonPressTimes[1]) && isDash(buttonPressTimes[2]) && isDash(buttonPressTimes[3]) && isDash(buttonPressTimes[4]))
    c = '0';
  return c;
}
