#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup()
{
  lcd.begin(16, 2);
  lcd.print("hello, world! ?");
  Serial.begin(115200);
}

char buffer[16];
int pos = 0;

void loop()
{
  while (Serial.available() > 0)
  {
    buffer[pos] = Serial.read();
    pos = (pos + 1) % 16;
  }

  lcd.setCursor(0,1);
  lcd.print(buffer);
}
