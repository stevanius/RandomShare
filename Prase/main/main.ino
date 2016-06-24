

#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//Pin connected to ST_CP of 74HC595
int latchPin = A2;
//Pin connected to SH_CP of 74HC595
int clockPin = A3;
////Pin connected to DS of 74HC595
int dataPin = A1;

int stepEnPin = 0x08;
int stepStepPin = 0x04;
int stepDirPin = 0x02;

int ledRPin = 0x80;
int ledGPin = 0x20;
int ledBPin = 0x10;

byte state;

void shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
  // This shifts 8 bits out MSB first, 
  //on the rising edge of the clock,
  //clock idles low

  //internal function setup
  int i=0;
  int pinState;
  //pinMode(myClockPin, OUTPUT);
  //pinMode(myDataPin, OUTPUT);

  //clear everything out just in case to
  //prepare shift register for bit shifting
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);

  //for each bit in the byte myDataOut
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights. 
  for (i=7; i>=0; i--)  {
    digitalWrite(myClockPin, 0);

    //if the value passed to myDataOut and a bitmask result 
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000 
    // and proceeds to set pinState to 1.
    if ( myDataOut & (1<<i) ) {
      pinState= 1;
    }
    else {  
      pinState= 0;
    }

    //Sets the pin to HIGH or LOW depending on pinState
    digitalWrite(myDataPin, pinState);
    //register shifts bits on upstroke of clock pin  
    digitalWrite(myClockPin, 1);
    //zero the data pin after shift to prevent bleed through
    digitalWrite(myDataPin, 0);
  }

  //stop shifting
  digitalWrite(myClockPin, 0);
}

void setup() {
  // put your setup code here, to run once:

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
  
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  state = ledRPin | ledGPin | ledBPin;
}

void loop() {
  // put your main code here, to run repeatedly:
  int x;
  x = analogRead (0);
  lcd.setCursor(10,1);
  if (x < 60) {
    lcd.print ("Right ");
  }
  else if (x < 200) {
    lcd.print ("Up    ");
    //digitalWrite(latchPin, 0);
    //shiftOut(dataPin, clockPin, dataPin, 0x7F);
    //digitalWrite(latchPin, 1);
    state &= ~stepEnPin;
    state |= stepDirPin;
    state &= ~ledGPin;
    state &= ~ledRPin;

    for (int i=0; i<10; i++)
    {
      state ^= stepStepPin;
      digitalWrite(latchPin, 0);
      delay(1);
      shiftOut(dataPin, clockPin, dataPin, state);
      delay(1);
      digitalWrite(latchPin, 1);
      delay(1);
    }
  }
  else if (x < 400){
    lcd.print ("Down  ");
    //digitalWrite(latchPin, 0);
    //shiftOut(dataPin, clockPin, dataPin, 0xDF);
    //digitalWrite(latchPin, 1);

    state &= ~stepEnPin;
    state &= ~stepDirPin;
    state &= ~ledGPin;
    state &= ~ledRPin;

    for (int i=0; i<10; i++)
    {
      state ^= stepStepPin;
      digitalWrite(latchPin, 0);
      delay(1);
      shiftOut(dataPin, clockPin, dataPin, state);
      delay(1);
      digitalWrite(latchPin, 1);
      delay(1);
    }
  }
  else if (x < 600){
    lcd.print ("Left  ");
    //digitalWrite(latchPin, 0);  
    //shiftOut(dataPin, clockPin, dataPin, 0xEF);
    //digitalWrite(latchPin, 1);
  }
  else if (x < 800){
    lcd.print ("Select");
  }
  else
  {
    lcd.print ("      ");
    state |= stepEnPin;
    state |= ledGPin;
    state |= ledRPin;
  }

  digitalWrite(latchPin, 0);
  delay(1);
  shiftOut(dataPin, clockPin, dataPin, state);
  delay(1);
  digitalWrite(latchPin, 1);
  delay(1);
}
