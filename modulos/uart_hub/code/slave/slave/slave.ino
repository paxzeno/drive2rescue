#include <SoftwareSerial.h>

const byte rxPin = 10;
const byte txPin = 11;

SoftwareSerial softSerial(rxPin, txPin);

void setup() {
  softSerial.begin(9600); // a ultima porta inicializada é que fica à escuta.
  pinMode(13, OUTPUT);
}

void loop() {
  while(softSerial.available() > 0) {
    softSerial.write(softSerial.read());
  }
}
