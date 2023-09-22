#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#inclue<SoftwareSerial.h>
#define rxPin 13 
#define txPin 12
SoftwareSerial sim800(rxPin,txPin);
LiquidCrystal_I2C lcd(0x27,20,4);
const byte ROWS = 4;
const byte COLS = 3;
char hexaKeys[ROWS][COLS] ={
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {A3, 3, 4,5};
byte colPins[COLS] = {A2, A1, A0};
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
String code="";
char customKey;
boolean dispencemode=false;
boolean menumode=false;
boolean balancemode=false;
int counter;
unsigned long timeone=millis();
unsigned long delayone=500;
int sensorpin=2;
volatile long pulse;
int volume;
boolean pumpmode=false;
#define buzzer 10
#define homebutton 11
#define switchchanganya 7
#define motor 8
#define levelsensor 9
boolean waterindication=false;
boolean changenumber=false;
#define solenoid 6 
boolean settimemode=false;
#define menubutton 1
boolean number=false;


void setup()
{
lcd.init(); 
lcd.backlight();
sim800.begin(9600);
pinMode(sensorpin,INPUT);
pinMode(solenoid,OUTPUT);
pinMode(buzzer,OUTPUT);
pinMode(homebutton,INPUT_PULLUP);
pinMode(motor,OUTPUT);
pinMode(switchchanganya,INPUT_PULLUP);
pinMode(levelsensor,INPUT_PULLUP);
pinMode(menubutton,INPUT_PULLUP);
digitalWrite(solenoid,HIGH);
attachInterrupt(digitalPinToInterrupt(sensorpin),increase,RISING);
lcd.setCursor(0,0);
lcd.print("*-IATT Company Ltd-*");
lcd.setCursor(0,1);
lcd.print("   *-Deals with-*   ");
lcd.setCursor(0,2);
lcd.print(">Electronic Services");
lcd.setCursor(0,3);
lcd.print(">Electrical Services");
delay(3000);
lcd.clear();
}
  
void loop()
{
menuoption();  
choosemenu();
leveltank();
} 


void dispence()
{
code="";
lcd.clear();
lcd.setCursor(0,0);
lcd.print(" Weka kiasi cha Maji");   
lcd.setCursor(2,1);
lcd.print("kiasi:");
lcd.setCursor(13,1);
lcd.print("Lita:");
digitalWrite(solenoid,LOW);
dispencemode=true;
do
{
sensorreading();
readkeypad();
lcd.setCursor(9,1);
lcd.print(code);
lcd.setCursor(0,2);
if(customKey=='*')
{
lcd.clear();
lcd.setCursor(0,0);
lcd.print(" Weka kiasi cha Maji");   
lcd.setCursor(2,1);
lcd.print("kiasi:");
lcd.setCursor(13,1);
lcd.print("Lita:");
digitalWrite(solenoid,LOW);
code="";
}
if(customKey=='#')
{
digitalWrite(solenoid,HIGH);
pumpmode=true;
}
if(code.toInt()>=50)  //hapa nitaweka condition kama maji yametimia kiasi entered
{
code="";
volume=0;
pulse=0;
lcd.clear();
lcd.setCursor(0,1);
lcd.print(" Upimaji Umekamilika");
lcd.setCursor(0,2);
lcd.print("    -Weka Chanjo-    ");
digitalWrite(solenoid,LOW);
digitalWrite(buzzer,HIGH);
delay(3000);
digitalWrite(buzzer,LOW);
}
if(digitalRead(switchchanganya)==LOW)
{
digitalWrite(motor,HIGH);
delay(3000);
digitalWrite(motor,LOW);
}
if(digitalRead(homebutton)==LOW)
{
 lcd.clear();
 dispencemode=false;
 digitalWrite(solenoid,HIGH);
}
}
while(dispencemode);
}


void readkeypad()
{
customKey = customKeypad.getKey();
if (customKey != '#' && customKey != '*') //condition  keypad not display this key 
{
String buton=String(customKey);
code+=buton;
}
}
void increase()
{
pulse++; 
}

void sensorreading()
{
unsigned long timenow=millis();
if(timenow-timeone>delayone)
{
timenow=timeone; 
volume=(2.663*pulse)*1000;
}
}

void alarmchanjo()
{
  code="";
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Chagua Aina ya Chanjo");
  lcd.setCursor(0,1);
  lcd.print(" 1:Gumboro");
  lcd.setCursor(0,2);
  lcd.print(" 2:Kideri");
  lcd.setCursor(0,3);
  lcd.print(" 3:Kifafa");
  changenumber=true;
  while(changenumber)
  {
 readkeypad();
 lcd.setCursor(8,1);
 lcd.print(code);
 if(customKey=='*')
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Chagua Aina ya Chanjo");
  lcd.setCursor(0,1);
  lcd.print(" 1:Gumboro");
  lcd.setCursor(0,2);
  lcd.print(" 2:Kideri");
  lcd.setCursor(0,3);
  lcd.print(" 3:Kifafa");
  code="";
}
if(digitalRead(homebutton)==LOW)
  {
    lcd.clear();
   changenumber=false;
  }
}
 
}


void choosemenu()
{
 readkeypad();
 if(customKey=='1')
 {
  dispence();  
 }
 if(customKey=='2')
 {
 alarmchanjo();  
 }
 if(customKey=='3')
 {
  setnumber(); 
 }
  if(customKey=='4')
 {
settime();   
 }
}

void settime()
{
  code="";
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" -Mpangilio wa Mda-");
  lcd.setCursor(0,1);
  lcd.print("  1:Kutumika chanjo");
  settimemode=true;
  do
  {
   readkeypad();
    if(customKey=='1')
  {
  code="";
  lcd.clear();
  mudawachanjokutumika(); 
  }
  if(digitalRead(homebutton)==LOW)
  {
    lcd.clear();
   settimemode=false; 
  }
  }
  while(settimemode);
}

void mudawachanjokutumika()
{
   lcd.setCursor(0,0);
   lcd.print("--Andika muda Wako--");
  do
  {
    readkeypad();
    lcd.setCursor(0,1);
    lcd.print(code);
  }
  while(1);
}

void leveltank()
{
  if(digitalRead(levelsensor)==LOW)
{
 waterindication=true;
 do
 {
  lcd.clear();
 lcd.setCursor(0,1);
 lcd.print("---Kiasi cha Maji---");
 lcd.setCursor(0,2);
 lcd.print(" Kidogo kwa Matumizi");
 digitalWrite(buzzer,HIGH);
 delay(300);
 //SEND SMS TO OWNER  KUFUGA
if(digitalRead(levelsensor)==HIGH)
{
 lcd.clear();
 digitalWrite(buzzer,LOW);
 waterindication=false; 
}
}
while(waterindication);
}
}

void menuoption()
{
 
  if(digitalRead(menubutton)==LOW)
  {
   counter==counter++;
   delay(100);
  }
  if(counter==0)
  {
   //lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Chagua kwenye Orodha");
   lcd.setCursor(0,1);
   lcd.print(" 1:Kupima Maji");
   lcd.setCursor(0,2);
   lcd.print(" 2:Alarm ya Chanjo  ");
   lcd.setCursor(0,3);
   lcd.print(" 3:Kuseti Namba"); 
  }
  if(counter==1)
  {
   lcd.setCursor(0,0);
   lcd.print("Chagua kwenye Orodha");
   lcd.setCursor(0,1);
   lcd.print(" 4:Mpangilio wa muda");
   lcd.setCursor(0,2);
   lcd.print(" 5:Kiasi cha Chakula");
    lcd.setCursor(0,3);
   lcd.print("                    ");
   counter==0;
  }
  if(counter==2)
  {
    counter=0;
  }
}

void setnumber()
{
code="";
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Chagua kwenye Orodha");
lcd.setCursor(0,1);
lcd.print("1:Badili Namba");
lcd.setCursor(0,2);
lcd.print("2:Namba Mpya");
number=true;
do {
    
readkeypad();
if(digitalRead(homebutton)==LOW)
{
 lcd.clear();
number=false;
}
}
while (number);
}
