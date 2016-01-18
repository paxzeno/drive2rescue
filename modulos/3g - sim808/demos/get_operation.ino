#include <SoftwareSerial.h>

SoftwareSerial softSerialB(10, 11); // RX, TX

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  softSerialB.begin(9600);
}

void loop() {
  //String command = getUserInput();

  runCommand("AT+CREG?", 500, true);  
  runCommand("AT+CSQ", 500, true);
  
  runCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", 500, true);
  runCommand("AT+SAPBR=3,1,\"APN\",\"net2.vodafone.pt\"", 500, true);
  runCommand("AT+SAPBR=3,1,\"USER\",\"\"", 500, true);
  runCommand("AT+SAPBR=3,1,\"PWD\",\"\"", 500, true);

  runCommand("AT+SAPBR=0,1", 500, true);
  runCommand("AT+SAPBR=1,1", 5000, true);
  runCommand("AT+SAPBR=2,1", 1000, true);

  runCommand("AT+HTTPTERM", 500, true);
  runCommand("AT+HTTPINIT", 1000, true);
  runCommand("AT+HTTPPARA=\"CID\",1", 500, true);
  
  runCommand("AT+HTTPPARA=\"URL\",\"http://www.checkupdown.com/status/E500_pt.html\"", 500, true);
  runCommand("AT+HTTPACTION=0", 30000, false);
  runCommand("AT+HTTPREAD", 30000, true);
  runCommand("AT+HTTPTERM", 500, true);
  runCommand("AT+SAPBR=0,1", 500, true);
  
  delay(60000);

}

void runCommand(String command, int waitInterval, boolean stopAfterOk) {
  
  sendToGsm(command);

  String returnData = waitForGsmResponse(waitInterval, stopAfterOk);

  Serial.println(returnData);
}


String getUserInput() {
  String content = ""; // complete message
  char character = 0; // one character

  while (true) {
    if (Serial.available() > 0) {
      character = Serial.read();

      if (character == 13 || character == 10) {
        break;
      }

      content.concat(character);
    }
  }

  return content;
}

void sendToGsm(String command) {
  if (command == "") { return; }
  softSerialB.println(command);
}

String waitForGsmResponse(int waitInterval, boolean stopAfterOk) {
  String content = ""; // complete message
  char character = 0; // one character
  int waitCount = 0;

  while (waitCount < waitInterval || softSerialB.available() > 0) {
    delay(100);
    waitCount += 100;

    character = softSerialB.read();
    
    if(character != (char)-1) {
      content.concat(character);
    }
    
    // if data exists disable wait
    if (softSerialB.available() > 0) {
      waitCount = 0; // reset counter
    }

    if (content.length() > 0) {
      
      if (content.indexOf("\nOK\r") != -1) {
        if (stopAfterOk) {          
          break;
        } else {
          content = "";
          waitCount = 0;
        }
      } else if (content.indexOf("\nERROR\r") != -1) {
        break;
      }
    }
  }

  return content;
}


