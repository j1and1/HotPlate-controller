// Display includes -> Adafruit PCD8544 lib
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
// thermal resistor library -> https://github.com/miguel5612/ThermistorLibrary
#include <thermistor.h>

#define BTN1 4
#define BTN2 5
#define BTN3 6
#define SENS1 A0
#define OUT1 12

// thermal runaway timeout - the time after the heater disables if there are no temperature updates
#define THERMAL_RUNAWAY_TIMEOUT 6000 // milliseconds
#define THERMAL_RUNAWAY_DEGREE_DIFF 3
// temperature PID controller values (just stole these from my 3D printer configuration)
#define P 29.12
#define I 3.22
#define D 65.83
// maximum temperature the heatplate can reach (according to spec sheet)
#define MAX_TEMP 260

// Software SPI (slower updates, more flexible pin options):
// pin 13 - Serial clock out (SCLK)
// pin 11 - Serial data out (DIN)
// pin 9 - Data/Command select (D/C)
// pin 10 - LCD chip select (CS)
// pin 8 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(13, 11, 9, 10, 8);
thermistor therm1(SENS1, 1);

// thermal runaway related variables
unsigned long lastTempIncrease = 0;
double lastTemp = 0.0;
// heating related variables
unsigned long heatingStart = 0;
double targetTemp = 0.0;
bool heating = false;
bool useCurve = false;

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
  keepTemp();
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
    display.print(readTemp());
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
    display.print(targetTemp);
 
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

double readTemp()
{
  return therm1.analog2temp();
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
    targetTemp--;
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
    targetTemp++;
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

void keepTemp()
{
  if (targetTemp > 0.0 && heating)
  {
    double currentTemp = readTemp();
    // detect thermal runaway
    if (millis() - lastTempIncrease >= THERMAL_RUNAWAY_TIMEOUT)
    {
      heating = false;
      page = 2; // show thermal runaway screen now
    }

    // update time variable used for thermal runaway if the temperature is within spec
    const double highBound = lastTemp + THERMAL_RUNAWAY_DEGREE_DIFF;
    const double lowBound = lastTemp - THERMAL_RUNAWAY_DEGREE_DIFF;
    if (lowBound <= currentTemp && currentTemp <= lowBound)
    {
      lastTempIncrease = millis();
    }      
    lastTemp = currentTemp;
  }
  else
  {
    lastTempIncrease = millis(); // placing this here as we need this time to be fresh when we want to enable heater 
    digitalWrite(OUT1, LOW);
  }
}
