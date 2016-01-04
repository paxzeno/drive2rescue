/*
  Software serial multple serial test

 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.

 The circuit:
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)

 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

 Not all pins on the Leonardo support change interrupts,
 so only the following can be used for RX:
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

 created back in the mists of time
 modified 25 May 2012
 by Tom Igoe
 based on Mikal Hart's example

 This example code is in the public domain.

 */
#include <SoftwareSerial.h>

SoftwareSerial mySerial(7, 6); // RX, TX

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

    pinMode(13, OUTPUT);

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
}

void loop() // run over and over
{
  String gpsData = listeningGpsChannel("GPGLL");
  Serial.println("=> " + gpsData);
}

struct String listeningGpsChannel(String messageType) {
  String content = ""; // complete message
  char character; // one character
  
  while(true) {
    if (mySerial.available() > 0) {
      character = mySerial.read();
      content.concat(character);
    }

    if (character == 13) {
      if (content.indexOf(messageType) != -1) {
        content.trim();
        return content;
      }

      content = "";
      character = 0;
    }
  }
}

