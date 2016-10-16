#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
}

void loop()
{
  int pot = max(min(analogRead(3), 1023), 1);
  
  lcd.setCursor(10,1);
  lcd.print(pot, DEC);

  analogWrite(9, pot / 4);
}
