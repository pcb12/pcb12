// receiver code
#define buzzer 10
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

void setup(){
  Serial.begin(9600);
  pinMode(buzzer,OUTPUT);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

}

void loop(){
  if(radio.available()){
    int  distance;
    radio.read(&distance, sizeof(distance));
    Serial.println(distance);
    delay(200);
  }
  if(distance<=4)
  {
    digitalWrite(buzzer,LOW)
  }
  else if(distance>=30)
  {
    digitalWrite(buzzer,HIGH);
  }
}
