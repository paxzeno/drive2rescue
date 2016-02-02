#include <SoftwareSerial.h>

SoftwareSerial softSerialB(10, 11); // RX, TX

/*************************************
 * GSM Get return message variables
 *************************************/
char dataContainer[1100];
int dataContainerLength = 0;
int dataContainerPosition = 0;
const int maxContainerCaracters = 1100;

/*************************************
 *  wait for GSM response timeout
 *************************************/
const long timeWaitGsmForResponse = 20000; // 20 seconds

void setup() {  
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  softSerialB.begin(4800);
}

void loop() {

  char ok[3] =  {'O', 'K', '\r'}; // OK\r
  int okLength = sizeof(ok);
  char httpAction[11] = {'H', 'T', 'T', 'P', 'A', 'C', 'T', 'I', 'O', 'N', ':'}; // HTTPACTION:
  int httpActionLength = sizeof(httpAction);
  
  runCommand(F("AT+CREG?"), ok, okLength);
  runCommand(F("AT+CSQ"), ok, okLength);

  runCommand(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""), ok, okLength);
  runCommand(F("AT+SAPBR=3,1,\"APN\",\"net2.vodafone.pt\""), ok, okLength);
  runCommand(F("AT+SAPBR=3,1,\"USER\",\"\""), ok, okLength);
  runCommand(F("AT+SAPBR=3,1,\"PWD\",\"\""), ok, okLength);

  runCommand(F("AT+SAPBR=0,1"), ok, okLength);
  runCommand(F("AT+SAPBR=1,1"), ok, okLength);
  runCommand(F("AT+SAPBR=2,1"), ok, okLength);

  runCommand(F("AT+HTTPTERM"), ok, okLength);
  runCommand(F("AT+HTTPINIT"), ok, okLength);
  runCommand(F("AT+HTTPPARA=\"CID\",1"), ok, okLength);
  runCommand(F("AT+HTTPPARA=\"URL\",\"http://31.22.253.69:4567/garbage/10\""), ok, okLength);
  runCommand(F("AT+HTTPACTION=0"), httpAction, httpActionLength);
  runCommand(F("AT+HTTPREAD"), ok, okLength);
  runCommand(F("AT+HTTPTERM"), ok, okLength);
  runCommand(F("AT+SAPBR=0,1"), ok, okLength);

  delay(120000);

}

/*************************************************
 * Process GSM connection
 *************************************************/
void runCommand(String command, char endMsg[], int endMsgLength) {
  
  sendToGsm(command);

  char output = waitForGsmResponse(endMsg, endMsgLength);

  String value;
  switch (output) {
    case 0:
      value.concat("ERROR: ");
      value.concat(command);
      Serial.println(value);
      break;
    case 1:
      printOutput();
      break;
    case 2:
      value.concat("TIMEOUT: ");
      value.concat(command);
      Serial.println(value);
      break;
  }
}

void printOutput() {
  resetContainerPosition();
  
  while(!currentPositionEqualsLength()){ 
    Serial.print(getCurrentChar()); 
  }
  
  restartContainer();
}

void sendToGsm(String command) {
  if (command != "")
    softSerialB.println(command);
}

char waitForGsmResponse(char endMsg[], int endMsgLength) {
  // end message
  int possibleEndMsgPosition = 0;
  boolean isPossibleEndMsg = false;

  // error message
  char errorMsg[6] =  {'E', 'R', 'R', 'O', 'R', '\r'};
  int errorMsgLength = sizeof(errorMsg);
  int possibleErrorMsgPosition = 0;
  boolean isPossibleErrorMsg = false;

  // time to timeout
  unsigned long startMillis = millis();

  // 0 => error
  // 1 => ok
  // 2 => timeout
  char outputStatus = 0;

  softSerialB.flush();

  while (true) {
    if (softSerialB.available() > 0) {
      char character = softSerialB.read();
      
      if (character >= 0 && character <= 127){
        //Serial.write(character);
        addChar(character);
      } 

      if (isEndMessage(isPossibleEndMsg, possibleEndMsgPosition, endMsg, endMsgLength, character)) { // ok output
        return 1;
      } else if (isEndMessage(isPossibleErrorMsg, possibleErrorMsgPosition, errorMsg, errorMsgLength, character)) { // error output
        return 0;
      }
      
      startMillis = millis(); // refresh counter.
      
    } else if ((millis() - startMillis) >= timeWaitGsmForResponse) { // check if is timeout time
        return 2;
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

