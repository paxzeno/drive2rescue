#include <SoftwareSerial.h>

//#define _SS_MAX_RX_BUFF 256 // RX buffer size //BEFORE WAS 64

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
  
  runCommand("AT+CREG?", "OK\r");  
  runCommand("AT+CSQ", "OK\r");

  runCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", "OK\r");
  runCommand("AT+SAPBR=3,1,\"APN\",\"net2.vodafone.pt\"", "OK\r");
  runCommand("AT+SAPBR=3,1,\"USER\",\"\"", "OK\r");
  runCommand("AT+SAPBR=3,1,\"PWD\",\"\"", "OK\r");

  runCommand("AT+SAPBR=0,1", "OK\r");
  runCommand("AT+SAPBR=1,1", "OK\r");
  runCommand("AT+SAPBR=2,1", "OK\r");

  runCommand("AT+HTTPTERM", "OK\r");
  runCommand("AT+HTTPINIT", "OK\r");
  runCommand("AT+HTTPPARA=\"CID\",1", "OK\r");
  runCommand("AT+HTTPPARA=\"URL\",\"http://www.m2msupport.net/m2msupport/test.php\"", "OK\r");
  //runCommand("AT+HTTPPARA=\"URL\",\"http://www.xxx.php\"", "OK\r");
  runCommand("AT+HTTPACTION=0", "HTTPACTION:");
  runCommand("AT+HTTPREAD", "OK\r");
  runCommand("AT+HTTPTERM", "OK\r");
  runCommand("AT+SAPBR=0,1", "OK\r");

  delay(20000);

}

void runCommand(String command, String endingMessage) {
  
  sendToGsm(command);

  String returnData = waitForGsmResponse(endingMessage);
  Serial.println(returnData);
}

void sendToGsm(String command) {
  if (command == "") { return; }
  softSerialB.println(command);
}

String waitForGsmResponse(String endingMessage) {
  String content = ""; // complete message
  char character = 0; // one character

  int endingMessageLength = endingMessage.length();
  char firstChar = endingMessage.charAt(0);
  String comparator = "";
  boolean comparatorStart = false;

  softSerialB.flush();

  while (true) {    
    while(softSerialB.available() > 0) {
      character = softSerialB.read();
      
      if (character != (char)-1){
        //Serial.write(character);
        content += character;
      }

      if (gsmResponseSelector(firstChar, comparatorStart, character, comparator, endingMessageLength, endingMessage)) {
        return content;
      }
    }
    
    if (content.length() > 0 && content.length() < 100 && content.indexOf("ERROR\r") != -1) {
      return content;
    }
  }
}


boolean gsmResponseSelector(char &firstChar, boolean &comparatorStart, char character, String &comparator, int &endingMessageLength, String &endingMessage) {

  int comparatorLength = comparator.length();
  
  if (firstChar == character) {
        comparatorStart = true;
  }

  if (comparatorStart && comparatorLength != endingMessageLength) {
    comparator += character;  
  } else if (comparatorStart && comparatorLength == endingMessageLength && comparator == endingMessage) {
    return true;
  } else if (comparatorStart && comparatorLength == endingMessageLength && comparator != endingMessage) {
    comparator = "";
    comparatorStart = false;
  }

  return false;
}





