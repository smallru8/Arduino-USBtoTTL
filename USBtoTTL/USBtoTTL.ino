#include <SoftwareSerial.h>
#include <pt.h>
#include <string.h>
#define BUFSIZE 1024
#define RXPIN 2
#define TXPIN 3

static struct pt serial_H,serial_S;

SoftwareSerial PiSerial(RXPIN,TXPIN);

byte serialH_B[BUFSIZE] = {0},serialS_B[BUFSIZE] = {0};
int serialH_B_size = 0,serialS_B_size = 0;

static int hardSerial(struct pt *pt){
  static unsigned long timer = 0;
  PT_BEGIN(pt);
  while(1){
    timer = millis();
    PT_WAIT_UNTIL(pt, Serial.available()||serialS_B_size);
    if(serialS_B_size){
      Serial.write(serialS_B,serialS_B_size);
      serialS_B_size = 0;
    }
    if(Serial.available()>BUFSIZE)
      serialH_B_size = Serial.readBytes(serialH_B, BUFSIZE);
    else
      serialH_B_size = Serial.readBytes(serialH_B, Serial.available());
    PT_WAIT_UNTIL(pt, millis()-timer>20);
  }
  PT_END(pt);
}

static int softSerial(struct pt *pt){
  static unsigned long timer = 0;
  PT_BEGIN(pt);
  while(1){
    timer = millis();
    PT_WAIT_UNTIL(pt, PiSerial.available()||serialH_B_size);
    if(serialH_B_size){
      PiSerial.write(serialH_B,serialH_B_size);
      serialH_B_size = 0;
    }
    if(PiSerial.available()>BUFSIZE)
      serialS_B_size = PiSerial.readBytes(serialS_B, BUFSIZE);
    else
      serialS_B_size = PiSerial.readBytes(serialS_B, PiSerial.available());
    PT_WAIT_UNTIL(pt, millis()-timer>20);
  }
  PT_END(pt);
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.print("Set device's baud rate:");
  int baudRate = 9600;

  char tmp[2] = {'\0'},baudRateStr[10] = {'\0'};
  do{
    if(Serial.available()){
      tmp[0] = Serial.read();
      strcat(baudRateStr,tmp);
    }
  }while(tmp[0]!='\n'&&tmp[0]!='\r');
  
  baudRate = atoi(baudRateStr);
  Serial.println(baudRate);
  Serial.println("Starting connection...");
  PiSerial.begin(baudRate);
  while(!PiSerial);
  PiSerial.write('\n');
  PiSerial.write('\n');
  PT_INIT(&serial_H);
  PT_INIT(&serial_S);
}

void loop() {
  hardSerial(&serial_H);
  softSerial(&serial_S);
}
