#include <SoftwareSerial.h>

#define _SS_MAX_RX_BUFF 256 // RX buffer size //BEFORE WAS 64

SoftwareSerial softSerialB(10, 11); // RX, TX

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  softSerialB.begin(4800);
}

void loop() {
  //String command = getUserInput();

  runCommand("AT+CREG?", 500, true);  
  runCommand("AT+CSQ", 500, true);
  /*
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
  
  runCommand("AT+HTTPPARA=\"URL\",\"http://www.m2msupport.net/m2msupport/test.php\"", 500, true);
  runCommand("AT+HTTPACTION=0", 30000, false);
  runCommand("AT+HTTPREAD", 30000, true);
  runCommand("AT+HTTPTERM", 500, true);
  runCommand("AT+SAPBR=0,1", 500, true);
  */
  delay(20000);

}

void runCommand(String command, int waitInterval, boolean stopAfterOk) {
  
  sendToGsm(command);

  //String returnData = waitForGsmResponse(waitInterval, stopAfterOk);


  String returnData;
  
  if (command == "AT+HTTPACTION=0") {
    returnData = waitForGsmResponseV2("HTTPACTION:");
  } else {
    returnData = waitForGsmResponseV2("OK");
  }

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
  String removedContent = "";
  int dontStopAfterOk = 0;

  while (waitCount < waitInterval) {
    delay(10);
    waitCount += 10;

    while(softSerialB.available() > 0) {
      character = softSerialB.read();
      //Serial.write(character);
      content += character;
      waitCount = 0;
    }
   
    if (content.length() > 0) {
      if (removedContent.length() == 0 && content.indexOf("OK\r") != -1) {
        if (stopAfterOk) {          
          break;
        } else {
          removedContent = content;
          content = "";
        }
      } else if (content.indexOf("\nERROR\r") != -1) {
        break;
      }
    }
  }

  if (removedContent.length() != 0)
    return removedContent + content;

  return content;
}

String waitForGsmResponseV2(String endingMessage) {
  String content = ""; // complete message
  char character = 0; // one character

  int endingMessageLength = endingMessage.length();
  char firstChar = endingMessage.charAt(0);
  String comparator = "";
  boolean comparatorStart = false;

  softSerialB.flush();

  while (true) {
    //delay(10);
    
    while(softSerialB.available() > 0) {
      character = softSerialB.read();
      Serial.write(character);
      content += character;

      if (firstChar == character) {
        comparatorStart = true;
        Serial.write("1");
      }

      if (comparatorStart && comparator.length() != endingMessageLength) {
        Serial.write("2");
        comparator += character;  
      } else if (comparatorStart && comparator.length() == endingMessageLength && comparator == endingMessage) {
        Serial.write("3");
        return content;
      } else if (comparatorStart && comparator.length() == endingMessageLength && comparator != endingMessage) {
        Serial.write("4");
        comparator = "";
        comparatorStart = false;
      }
      

    }
    
    if (content.length() > 0 && (content.indexOf(endingMessage) != -1 || content.indexOf("\nERROR") != -1)) {
      //Serial.write("--***--");
      return content;
    }
    
  }
}





