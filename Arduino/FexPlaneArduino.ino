#include <Encoder.h>

#include <TimerOne.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/**********************************************************/
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
/*********************************************************/
const int PIN_BUTTON_C1_MODE = 2;
const int PIN_BUTTON_C2_MODE = 3;
const int PIN_BUTTON_SPD_MODE = 4;
const int PIN_BUTTON_HDG_MODE = 5;
const int PIN_BUTTON_ALT_MODE = 6;
const int PIN_BUTTON_VS_MODE = 7;
int buttonStateC1 = HIGH;
int buttonStateC2 = HIGH;
int buttonStateSPD = HIGH;
int buttonStateHDG = HIGH;
int buttonStateALT = HIGH;
int buttonStateVS = HIGH;
int lastButtonStateSPD = HIGH;   // the previous reading from the input pin
int lastButtonStateHDG = HIGH; 
int lastButtonStateALT = HIGH; 
int lastButtonStateVS = HIGH; 
int lastButtonStateC1 = HIGH; 
int lastButtonStateC2 = HIGH; 
unsigned long lastDebounceTimeSPD = 0;  // the last time the input pin was toggled
unsigned long lastDebounceTimeHDG = 0;
unsigned long lastDebounceTimeALT = 0;
unsigned long lastDebounceTimeVS = 0;
unsigned long lastDebounceTimeC1 = 0;
unsigned long lastDebounceTimeC2 = 0;

unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
String inputString = "";
/*********************************************************/
const int PIN_ENC_0 = 8;
const int PIN_ENC_1 = 9;
Encoder myEnc(PIN_ENC_0, PIN_ENC_1); 
long encoderValue = 0;
/*********************************************************/

void setup()
{
  lcd.init();  //initialize the lcd
  lcd.backlight();  //open the backlight
  pinMode(PIN_BUTTON_C1_MODE, INPUT_PULLUP);
  pinMode(PIN_BUTTON_C2_MODE, INPUT_PULLUP);
  pinMode(PIN_BUTTON_SPD_MODE, INPUT_PULLUP);
  pinMode(PIN_BUTTON_HDG_MODE, INPUT_PULLUP);
  pinMode(PIN_BUTTON_ALT_MODE, INPUT_PULLUP);
  pinMode(PIN_BUTTON_VS_MODE, INPUT_PULLUP);
  Timer1.initialize(100);
  Timer1.attachInterrupt(timerIsr); 
  
  lcd.setCursor(0, 0);  lcd.print("CONNECT PLANE");
  Serial.begin(9600);
}
/*********************************************************/
void loop()
{
  buttonCheckSPD();
  buttonCheckHDG();
  buttonCheckVS();
  buttonCheckALT();
  buttonCheckC1();
  buttonCheckC2();
  encoderCheck();
}
/*********************************************************/


void buttonCheckSPD()
{
  int newButtonStateSPD = digitalRead(PIN_BUTTON_SPD_MODE);
  if (lastButtonStateSPD != newButtonStateSPD)
  {
    lastDebounceTimeSPD = millis();
  }
  if ((millis() - lastDebounceTimeSPD) > debounceDelay)
  {
    if (newButtonStateSPD != buttonStateSPD)
    {
      if(newButtonStateSPD == LOW)
      {
        Serial.println("MS");
      }
      buttonStateSPD = newButtonStateSPD;
    }
  }
  lastButtonStateSPD = newButtonStateSPD;
}

void buttonCheckHDG()
{
  int newButtonStateHDG = digitalRead(PIN_BUTTON_HDG_MODE);
  if (lastButtonStateHDG != newButtonStateHDG)
  {
    lastDebounceTimeHDG = millis();
  }
  if ((millis() - lastDebounceTimeHDG) > debounceDelay)
  {
    if (newButtonStateHDG != buttonStateHDG)
    {
      if(newButtonStateHDG == LOW)
      {
        Serial.println("MH");
      }
      buttonStateHDG = newButtonStateHDG;
    }
  }
  lastButtonStateHDG = newButtonStateHDG;
}

void buttonCheckALT()
{
  int newButtonStateALT = digitalRead(PIN_BUTTON_ALT_MODE);
  if (lastButtonStateALT != newButtonStateALT)
  {
    lastButtonStateALT = millis();
  }
  if ((millis() - lastDebounceTimeALT) > debounceDelay)
  {
    if (newButtonStateALT != buttonStateALT)
    {
      if(newButtonStateALT == LOW)
      {
        Serial.println("MA");
      }
      buttonStateALT = newButtonStateALT;
    }
  }
  lastButtonStateALT = newButtonStateALT;
}

void buttonCheckVS()
{
  int newButtonStateVS = digitalRead(PIN_BUTTON_VS_MODE);
  if (lastButtonStateVS != newButtonStateVS)
  {
    lastDebounceTimeVS = millis();
  }
  if ((millis() - lastDebounceTimeVS) > debounceDelay)
  {
    if (newButtonStateVS != buttonStateVS)
    {
      if(newButtonStateVS == LOW)
      {
        Serial.println("MV");
      }
      buttonStateVS = newButtonStateVS;
    }
  }
  lastButtonStateVS = newButtonStateVS;
}

void buttonCheckC1()
{
  int newButtonStateC1 = digitalRead(PIN_BUTTON_C1_MODE);
  if (lastButtonStateC1 != newButtonStateC1)
  {
    lastDebounceTimeC1 = millis();
  }
  if ((millis() - lastDebounceTimeC1) > debounceDelay)
  {
    if (newButtonStateC1 != buttonStateC1)
    {
      if(newButtonStateC1 == LOW)
      {
        Serial.println("MB");
      }
      buttonStateC1 = newButtonStateC1;
    }
  }
  lastButtonStateC1 = newButtonStateC1;
}

void buttonCheckC2()
{
  int newButtonStateC2 = digitalRead(PIN_BUTTON_C2_MODE);
  if (lastButtonStateC2 != newButtonStateC2)
  {
    lastDebounceTimeC2 = millis();
  }
  if ((millis() - lastDebounceTimeC2) > debounceDelay)
  {
    if (newButtonStateC2 != buttonStateC2)
    {
      if(newButtonStateC2 == LOW)
      {
        Serial.println("MC");
      }
      buttonStateC2 = newButtonStateC2;
    }
  }
  lastButtonStateC2 = newButtonStateC2;
}

void encoderCheck()
{
  int val = myEnc.read();
  noInterrupts();
  encoderValue += val;
  interrupts();
}

void timerIsr()
{
  if(encoderValue != 0)
  {
    Serial.print("E");
    Serial.println(encoderValue);
    encoderValue = 0;
    myEnc.write(0);
  }
}

void serialEvent()
{
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    
    if (inChar == '\n')
    {
      lcd.clear();
      int offset = 0;
      switch(inputString[0])
      {
        case 'A':
        lcd.print("    ALTITUDE");
        offset = 10;
        break;
        case 'S':
        lcd.print("    AIRSPEED");
        break;
        case 'H':
        lcd.print("    HEADING");
        offset = 13;
        break;
        case 'V':
        lcd.print(" VERTICAL SPEED");
        offset = 11;
        break;
        default:
        lcd.print("UNKNOWN");
        break;
      }
      inputString.remove(0,1);
      
      lcd.setCursor(offset, 1);  lcd.print(inputString);
      // clear the string:
      inputString = "";
    }
    else
    {
      inputString += inChar;
    }
  }
}
