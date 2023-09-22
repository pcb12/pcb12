// author qualchriss


// transmitter code

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
const int trigPin =6;
const int echoPin =5;
long duration;
int distance;
const byte address[6] = "00001";

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  sensor();
  radio.write(&distance, sizeof(distance));
  delay(1000);
}

void sensor()
{
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
duration = pulseIn(echoPin, HIGH);
distance= (duration/2)/29.1;
Serial.print("Distance: ");
Serial.println(distance);
delay(300);
}


