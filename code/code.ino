#include <LiquidCrystal_I2C.h>
#include<SoftwareSerial.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define rxPin 7
#define txPin 8
SoftwareSerial sim800l(rxPin, txPin);
#include <Keypad.h>
const byte ROWS = 4;
const byte COLS = 3;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {A3, 13, 12, 11};
byte colPins[COLS] = {A0, A1, A2};
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
String code = "";
char customKey;
boolean didpencemode = false;
boolean tankmode = false;
const int  homebutton=3;
const int valve = 6;
const int tankinput = 9;
const int led = 5;
const int buzzer = 4;
unsigned long timeone = millis();
unsigned long delayone = 500;
int sensorpin = 2;
volatile long pulse;
int volume;
//----initialization of all function used to the system----//
void lcdprint(String message1, String message2);
void send_sms( String number, String ujumbe);

void setup()
{
  lcd.init();
  lcd.backlight();
  sim800l.begin(9600);
  Serial.begin(9600);
  pinMode(homebutton, INPUT_PULLUP);
  pinMode(valve, OUTPUT);
  pinMode(sensorpin, INPUT);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(tankinput, INPUT_PULLUP);
  digitalWrite(valve, LOW);
  attachInterrupt(digitalPinToInterrupt(sensorpin), increase, RISING);
  lcdprint("*-IATT Company-*", "Place:Dar-es salm");
  delay(2000);
  lcd.clear();
  lcdprint("", "Inapakia........");
  delay(2000);
  lcd.clear();
  lcd.print("Mtandao:");
  lcd.setCursor(0, 1);
  lcd.print("Nguvu: ");
  // Check if SIM800L is ready
  sim800l.println("AT");
  delay(100);
  while (sim800l.available()) {
    char c = sim800l.read();
    Serial.write(c);
  }
  // Get network information
  sim800l.println("AT+CSQ");
  delay(1000);
  String response = "";
  while (sim800l.available()) {
    char c = sim800l.read();
    response += c;
  }
  int index = response.indexOf("+CSQ:");
  if (index != -1) {
    int start = index + 6;
    int end = response.indexOf(',', start);
    if (end != -1) {
      String signalStr = response.substring(start, end);
      int signalValue = signalStr.toInt();
      int signalPercentage = map(signalValue, 0, 31, 0, 100);
      lcd.setCursor(8, 1);
      lcd.print(signalPercentage);
      lcd.print("%   ");
    }
  }
  // Get network type information
  sim800l.println("AT+CREG?");
  delay(1000);
  while (sim800l.available()) {
    char c = sim800l.read();
    if (c == '"') {
      String networkType = sim800l.readStringUntil('"');
      lcd.setCursor(0, 0);
      lcd.print(networkType);
      lcd.print("   ");
    }
  }

}
//----main loop function of whole project----//
void loop()
{
  lcdprint("1.Pima Maji", "2.Chagua  chanjo");
  digitalWrite(valve, HIGH);
  menu();
}


//---main menu function initialization----//
void menu()
{
  readkeypad();
  if (customKey == '1')
  {
    measuring();
  }
  if (customKey == '2')
  {
    // DO SOMETHING
  }
}


void readkeypad()
{
  customKey = customKeypad.getKey();
  if (customKey != '#' && customKey != '*')
  {
    String buton = String(customKey);
    code += buton;
  }
}

void sensorreading()
{
  unsigned long timenow = millis();
  if (timenow - timeone > delayone)
  {
    timenow = timeone;
    volume = (2.663 * pulse) * 1000;
  }
}

void increase()
{
  pulse++;
}

//---measuring function of water----//
void measuring()
{
  code = "";
  digitalWrite(valve, LOW);
  lcd.clear();
  lcdprint("-Ingiza   Kiasi-", "Lita:");
  didpencemode = true;
  do
  {
    readkeypad();
    lcd.setCursor(7, 1);
    lcd.print(code);
    if (customKey == '*')
    {
      digitalWrite(valve, LOW);
      lcd.clear();
      lcdprint("-Ingiza   Kiasi-", "Lita:");
      code = "";
    }
    
    if (digitalRead(homebutton) == LOW)
    {
      delay(60);
      lcdprint("    Mpangilio     ", "   Umesitishwa   ");
      delay(1000);
      lcd.clear();
      didpencemode = false;
    }
    if (customKey == '#')
    {
      if(code.toInt() >= 1)
      {
      digitalWrite(valve, HIGH);
      }
      else
      {
        lcd.clear();
        code="";
        digitalWrite(valve, LOW);
        lcdprint("     Kiwango   ", " *--Si sahihi--*");
        delay(1000);
        lcd.clear();
        measuring();
      }
      if (volume == code.toInt())
      {
        digitalWrite(valve, LOW);
      }
  }
  }
  while (didpencemode);
}

//----sending sms function for all kind of information----//
void send_sms( String text, String Phone)
{
    sim800l.print("AT+CMGF=1\r");
    delay(1000);
    sim800l.print("AT+CMGS=\""+Phone+"\"\r");
    delay(1000);
    sim800l.print(text);
    delay(100);
    sim800l.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
    delay(1000);
    Serial.println("SMS Sent Successfully.");
}
void Tank_satus()
{
  if (digitalRead(tankinput) == HIGH)
  {
    tank();
  }
  else if (digitalRead(tankinput) == LOW)
  {
    lcd.clear();
    didpencemode = true;
    tankmode = false;
  }
}

void tank()
{
  lcd.clear();
  tankmode = true;
  do
  {
    lcd.setCursor(0, 0);
    lcd.print("Kiwango Cha Maji");
    lcd.setCursor(0, 1);
    lcd.print("  **Kidogo**    ");
  }
  while (tankmode);
}
//----display information functoion-----//
void lcdprint(String message1, String message2)
{
  lcd.setCursor(0, 0);
  lcd.print(message1);
  lcd.setCursor(0, 1);
  lcd.print(message2);
}

