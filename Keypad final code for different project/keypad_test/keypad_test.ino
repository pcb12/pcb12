#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);
String code;
const byte ROWS = 4; 
const byte COLS = 3;
String password = "0000";
char key;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
//------------------------------------------------------------
byte rowPins[ROWS] = {4,3 ,2 ,8}; 
byte colPins[COLS] = {5,6 ,7};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Keypad");
  lcd.setCursor(0, 1);
  lcd.print("Test");
  delay(1000);
  lcd.clear();
}
void loop() {
  lcd.setCursor(0,0);
  lcd.print("Enter your Ammount");
  lcd.setCursor(0,1);
  lcd.print(code);
  readKeypad();
  if (key == '#') 
  {
   //do something you want in your project
  } 
  if (key == '*') {
     code = "";
     lcd.clear();
    //clear display button
  }
  
}
void readKeypad() {
  key = keypad.getKey();
  if (key != '#' && key != '*' ) 
  {
    String buton=String(key);
    code+=buton;
  }
}
