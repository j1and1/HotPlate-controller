// Display includes -> Adafruit PCD8544 lib
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#include "Configuration.h"
#include "TemperatureControl.h"

// Software SPI (slower updates, more flexible pin options):
// pin 13 - Serial clock out (SCLK)
// pin 11 - Serial data out (DIN)
// pin 9 - Data/Command select (D/C)
// pin 10 - LCD chip select (CS)
// pin 8 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(13, 11, 9, 10, 8);

// menu related variables
int menuitem = 0;
int page = 0;

volatile boolean up = false;
volatile boolean down = false;
volatile boolean middle = false;

int downButtonState = 0;
int upButtonState = 0;  
int selectButtonState = 0;          
int lastDownButtonState = 0;
int lastSelectButtonState = 0;
int lastUpButtonState = 0;
// menu related stuff end

void setup() {
  // put your setup code here, to run once:
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);

  pinMode(OUT1, OUTPUT);
  digitalWrite(OUT1, LOW);

  Serial.begin(9600);

  // start display related activities
  display.begin();
  display.setContrast(50);
  display.display(); // show splashscreen
  delay(2000);
  display.clearDisplay(); // clears the screen and buffer
  display.setRotation(2);
}

void loop() {
  displayMenu();
  doMenuAction();
}

void displayMenu()
{
  if (page == 0) 
  {    
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setCursor(15, 0);
    display.print("MAIN MENU");
    display.drawFastHLine(0,10,83,BLACK);
    display.setCursor(0, 15);
   
    if (menuitem==0) 
    { 
      display.setTextColor(WHITE, BLACK);
    }
    else 
    {
      display.setTextColor(BLACK, WHITE);
    }
    display.print(">Fixed Temp");
    display.setCursor(0, 25);

    if (menuitem==1) 
    { 
      display.setTextColor(WHITE, BLACK);
    }
    else 
    {
      display.setTextColor(BLACK, WHITE);
    }
    display.print(">Reflow curve");

    display.setTextColor(BLACK, WHITE);
    display.setCursor(0, 35);
    display.print("Temp C: ");
    display.display();
  }
  else if (page == 1) 
  {
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setCursor(15, 0);
    display.print("Target temp:");
    display.drawFastHLine(0,10,83,BLACK);
    display.setCursor(5, 15);
    display.print("Value");
    display.setTextSize(2);
    display.setCursor(5, 25);
    //display.print(targetTemp);
 
    display.setTextSize(2);
    display.display();
  }
  else if (page == 2) // thermal runaway
  {
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setCursor(15, 0);
    display.print("Error");
    display.drawFastHLine(0,10,83,BLACK);
    display.setCursor(5, 15);
    display.print("Thermal");
    display.setTextSize(2);
    display.setCursor(5, 25);
    display.print("runaway");
    display.display();
  }
  else
  {
    
  }
}

void doMenuAction()
{
  downButtonState = digitalRead(BTN3);
  selectButtonState = digitalRead(BTN2);
  upButtonState =   digitalRead(BTN1);

  checkIfDownButtonIsPressed();
  checkIfUpButtonIsPressed();
  checkIfSelectButtonIsPressed();

  if (up && page == 0 ) {
    up = false;
    menuitem--;
    if (menuitem== -1)
    {
      menuitem = 1;
    }      
  }
  else if (up && page == 1 ) 
  {
    up = false;
  }


  if (down && page == 0) {
    down = false;
    menuitem++;
    if (menuitem == 2) 
    {
      menuitem=0;
    }      
  }
  else if (down && page == 1) 
  {
    down = false;
  }

  if (middle) {
    middle = false;
    
    if (page == 0 && menuitem==0) {
      page = 1;
    }
    else if (page == 1) {
      page = 0;
    }
  }
}

void checkIfDownButtonIsPressed()
{
  if (downButtonState != lastDownButtonState) 
  {
    if (downButtonState == 0) 
    {
      down = true;
    }
    delay(50);
  }
  lastDownButtonState = downButtonState;
}

void checkIfUpButtonIsPressed()
{
  if (upButtonState != lastUpButtonState) 
  {
    if (upButtonState == 0) {
      up = true;
    }
    delay(50);
  }
  lastUpButtonState = upButtonState;
}

void checkIfSelectButtonIsPressed()
{
  if (selectButtonState != lastSelectButtonState) 
  {
    if (selectButtonState == 0) {
      middle = true;
    }
    delay(50);
  }
  lastSelectButtonState = selectButtonState;
}
