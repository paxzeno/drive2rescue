#include <SoftwareSerial.h>

SoftwareSerial softSerialB(10, 11); // RX, TX

char dataContainer[1000];
int dataContainerLength = 0;
int dataContainerPosition = 0;
int maxContainerCaracters = 1000;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  softSerialB.begin(4800);
}

void loop() {

  char ok[3] =  {'O', 'K', '\r'};
  int okLength = sizeof(ok);
  char httpAction[11] = {'H', 'T', 'T', 'P', 'A', 'C', 'T', 'I', 'O', 'N', ':'};
  int httpActionLength = sizeof(httpAction);
  
  runCommand("AT+CREG?", ok, okLength);
  runCommand("AT+CSQ", ok, okLength);

  runCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", ok, okLength);
  runCommand("AT+SAPBR=3,1,\"APN\",\"net2.vodafone.pt\"", ok, okLength);
  runCommand("AT+SAPBR=3,1,\"USER\",\"\"", ok, okLength);
  runCommand("AT+SAPBR=3,1,\"PWD\",\"\"", ok, okLength);

  runCommand("AT+SAPBR=0,1", ok, okLength);
  runCommand("AT+SAPBR=1,1", ok, okLength);
  runCommand("AT+SAPBR=2,1", ok, okLength);

  runCommand("AT+HTTPTERM", ok, okLength);
  runCommand("AT+HTTPINIT", ok, okLength);
  runCommand("AT+HTTPPARA=\"CID\",1", ok, okLength);
  runCommand("AT+HTTPPARA=\"URL\",\"http://www.m2msupport.net/m2msupport/test.php\"", ok, okLength);
  runCommand("AT+HTTPACTION=0", httpAction, httpActionLength);
  runCommand("AT+HTTPREAD", ok, okLength);
  runCommand("AT+HTTPTERM", ok, okLength);
  runCommand("AT+SAPBR=0,1", ok, okLength);

  delay(60000);

}

void runCommand(String command, char endMsg[], int endMsgLength) {
  sendToGsm(command);

  waitForGsmResponse(endMsg, endMsgLength);

  // print data
  resetContainerPosition();
  while(!currentPositionEqualsLength()){
    Serial.print(getCurrentChar());
  }
  restartContainer();
}

void sendToGsm(String command) {
  if (command == "")
    return; 
  softSerialB.println(command);
}

void waitForGsmResponse(char endMsg[], int endMsgLength) {
  char character = 0; // empty caracter

  // end message
  int possibleEndMsgPosition = 0;
  boolean isPossibleEndMsg = false;

  // error message
  char errorMsg[6] =  {'E', 'R', 'R', 'O', 'R', '\r'};
  int errorMsgLength = sizeof(errorMsg);
  int possibleErrorMsgPosition = 0;
  boolean isPossibleErrorMsg = false;

  softSerialB.flush();

  while (true) {    
    while(softSerialB.available() > 0) {
      character = softSerialB.read();
      
      if (character != (char)-1){
        //Serial.write(character);
        addChar(character);
      }

      if (isEndMessage(isPossibleEndMsg, possibleEndMsgPosition, endMsg, endMsgLength, character)) {
        return;
      } else if (isEndMessage(isPossibleErrorMsg, possibleErrorMsgPosition, errorMsg, errorMsgLength, character)) {
        return;
      }
      
    }
  }
}

boolean isEndMessage(boolean &isPossibleMsg, int &possibleMsgPosition, char msg[], int &msgLength, char character) {

  if (!isPossibleMsg && character == msg[0]) {
    isPossibleMsg = true;
    possibleMsgPosition = 1;
  } else if (isPossibleMsg) {
    if (possibleMsgPosition >= msgLength) {
      possibleMsgPosition = 0;
      isPossibleMsg = false;
      return true;
    } else if (character == msg[possibleMsgPosition] && possibleMsgPosition < msgLength) {
      possibleMsgPosition++;
    } else if (character != msg[possibleMsgPosition]) {
      possibleMsgPosition = 0;
      isPossibleMsg = false;
    }
  }
  
  return false;
}



/*************************************************
 * Container Management
 *************************************************/

// add new character
void addChar(char value) {
  if (dataContainerLength >= maxContainerCaracters) 
    return;
    
  dataContainer[dataContainerLength] = value;
  dataContainerLength++;
}

// get next character
char getCurrentChar() {
  if (dataContainerPosition >= dataContainerLength)
    return -1;

  dataContainerPosition++;
  return dataContainer[dataContainerPosition-1];
}

// is container full ?
boolean containerFull() {
  return (dataContainerLength < maxContainerCaracters) ? true : false;
}

// is current posistion equals to length
boolean currentPositionEqualsLength() {
  return (dataContainerPosition < dataContainerLength) ? false : true;
}

// reset container parse position
void resetContainerPosition() {
  dataContainerPosition = 0;
}

// restart container so that new data can be added
void restartContainer() {
  dataContainerLength = 0;
  dataContainerPosition = 0;
}

/*************************************************
 *************************************************
 *************************************************/ 

