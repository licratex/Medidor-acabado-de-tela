#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

void setup()  
{
  lcd.begin(20,4); 
  lcd.setCursor(3,0);
  lcd.print("Hello YouTube!");
  lcd.setCursor(8,1);
  lcd.print("****");
  lcd.setCursor(0,2);
  lcd.print("This is a demo text");
  lcd.setCursor(8,3);
  lcd.print("****");
}

void loop() 
{ 
}
