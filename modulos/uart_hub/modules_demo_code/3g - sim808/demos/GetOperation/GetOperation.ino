#include <SoftwareSerial.h>

SoftwareSerial softSerialB(7, 6); // RX, TX

/*************************************
 * GSM Get return message variables
 *************************************/
char dataContainer[1100];
int dataContainerLength = 0;
int dataContainerPosition = 0;
const int maxContainerCaracters = 1100;

/*************************************
 * Wait for GSM response timeout
 *************************************/
const long timeWaitGsmForResponse = 20000; // 20 seconds

/*************************************
 * Default expected messages
 *************************************/
// ON\n
char okMsg[3] =  {'O', 'K', '\r'}; 
int okMsgLength = sizeof(okMsg);
// HTTPACTION:
char httpActionMsg[11] = {'H', 'T', 'T', 'P', 'A', 'C', 'T', 'I', 'O', 'N', ':'}; 
int httpActionMsgLength = sizeof(httpActionMsg);


/*************************************/
void setup() {  
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  softSerialB.begin(4800);
}

void loop() {
  byte outputCode;

  outputCode = sendInformation();
  getOutput(outputCode);

  closeGsmConnection();

  Serial.println("retry");

  delay(10000);

}

/*************************************************
 * Operations
 *************************************************/
byte sendInformation() {
  byte outputCode;
  
  // 0.0.0.0
  char noIp[7] = {'0', '.', '0', '.', '0', '.', '0'};
  int  noIpLength = sizeof(noIp);

  // 0,200
  char okHttpReturn[5] = {'0', ',', '2', '0', '0'};
  int  okHttpReturnLength = sizeof(okHttpReturn);

  outputCode = runATCommand(F("AT+CSQ"), okMsg, okMsgLength); // check signat
  
  // returns if error or timeout
  if (outputCode == 0) {
    return 5; // error while start talking to GSM module.
  } else if (outputCode == 2) {
    return 6; // timeout while start talking to GSM module.
  }

  // internet connection settings
  runATCommand(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""), okMsg, okMsgLength);
  runATCommand(F("AT+SAPBR=3,1,\"APN\",\"net2.vodafone.pt\""), okMsg, okMsgLength);
  runATCommand(F("AT+SAPBR=3,1,\"USER\",\"\""), okMsg, okMsgLength);
  runATCommand(F("AT+SAPBR=3,1,\"PWD\",\"\""), okMsg, okMsgLength);

  
  runATCommand(F("AT+SAPBR=0,1"), okMsg, okMsgLength); // close connection 

  // try to connect
  outputCode = runATCommand(F("AT+SAPBR=1,1"), okMsg, okMsgLength); 
  
  if (outputCode == 0) {
    return 7; // error while opening GSM connection.
  } else if (outputCode == 2) {
    return 8; // timeout while opening GSM connection.
  }


  // check internet ip
  outputCode = runATCommand(F("AT+SAPBR=2,1"), okMsg, okMsgLength); 

  if (outputCode == 1 && containerIncludes(noIp, noIpLength)) {
    getOutput(outputCode); // is not connected return 3
    return 3;
  } else if (outputCode != 1) {
    return outputCode;
  }

  // close http connection
  runATCommand(F("AT+HTTPTERM"), okMsg, okMsgLength); 
  
  outputCode = runATCommand(F("AT+HTTPINIT"), okMsg, okMsgLength); 
  if (outputCode == 0) {
    return 11; // error while opening HTTP connection.
  } else if (outputCode == 2) {
    return 12; // timeout while opening HTTP connection.
  }

  runATCommand(F("AT+HTTPPARA=\"CID\",1"), okMsg, okMsgLength); 
  
  // set url
  runATCommand(F("AT+HTTPPARA=\"URL\",\"http://www.m2msupport.net/m2msupport/test.php\""), okMsg, okMsgLength); 
  
  // send command
  runATCommand(F("AT+HTTPACTION=0"), httpActionMsg, httpActionMsgLength); 

  delay(2000);
  
  // get data
  outputCode = runATCommand(F("AT+HTTPREAD"), okMsg, okMsgLength); 
  
  //getOutput(outputCode);
  if (outputCode == 1 && !containerIncludes(okHttpReturn, okHttpReturnLength)) {
    return 4; // not 200 http return
  }  else if (outputCode == 0) {
    return 9; // error in html data return.
  } else if (outputCode == 2) {
    return 10; // timeout while waiting for html data return.
  }
  
  return outputCode;
}

/*************************************************
 * Manage GSM connection
 *************************************************/
byte runATCommand(String command, char endMsg[], int &endMsgLength) {
  if (command == "")
    return 0;

  softSerialB.println(command); // send uart command

  return waitForGsmResponse(endMsg, endMsgLength);
}

/* get data container output 
  ERROR Codes:
    0 => error returned from AT command
    1 => ok
    2 => timeout waiting for gsm response
    3 => error opening gsm connection ip returned 0.0.0.0
    4 => error opening page != http 200
    5 => error while start talking to GSM module (reading signal).
    6 => timeout while start talking to GSM module (reading signal).
    7 => error while opening GSM connection.
    8 => timeout while opening GSM connection.
    9 => error in html data return.
    10 => timeout while waiting for html data return.
    11 => error while opening HTTP connection. ()
    12 => timeout while opening HTTP connection.
*/
void getOutput(byte &outputCode) {
  switch(outputCode) {
    case 0:
      Serial.println(F("ERROR_RETURN_FROM_AT_COMMAND"));
    break;
    case 1:
      printOutput();
    break;
    case 2:
      Serial.println(F("TIMEOUT_WAITING_FOR_GSM_RESPONSE"));
    break;
    case 3:
      Serial.println(F("NOT_CONNECTED"));      
    break;
    case 4:
      Serial.println(F("HTTP_GET_PAGE_ERROR"));
    break;    
    case 5:
      Serial.println(F("ERROR_START_GSM"));
    break;
    case 6:
      Serial.println(F("TIMEOUT_START_GSM"));
    break;
    case 7:
      Serial.println(F("ERROR_OPENING_GSM_CONNECTION"));
    break;
    case 8:
      Serial.println(F("TIMEOUT_OPENING_GSM_CONNECTION"));
    break;
    case 9:
      Serial.println(F("ERROR_IN_HTML_DATA_RETURN"));
    break;
    case 10:
      Serial.println(F("TIMEOUT_WHILE_WAITING_FOR_HTML_DATA"));
    break;
    case 11:
      Serial.println(F("ERROR_WHILE_OPENNING_HTML_CONNECTION"));
    break;
    case 12:
      Serial.println(F("TIMEOUT_WHILE_OPENNING_HTML_CONNECTION"));
    break;
  }
}

void printOutput() {
  resetContainerPosition();
  
  while(!currentPositionEqualsLength()) { 
    Serial.print(getCurrentChar()); 
  }
}

void closeGsmConnection() {
  // terminate http connection
  runATCommand(F("AT+HTTPTERM"), okMsg, okMsgLength);
  
  // close connection
  runATCommand(F("AT+SAPBR=0,1"), okMsg, okMsgLength);
}

// wait for response, contains timeout so that will free the usart connection.
byte waitForGsmResponse(char endMsg[], int endMsgLength) {
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
  softSerialB.listen();

  restartContainer(); //restart data container to get new data from GSM channel

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

// check if this is a break communication message
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

// is current position equals to length
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

/* includes text ? */
boolean containerIncludes(char text[], int textLength) {
  // end message
  int possibleMsgPosition = 0;
  boolean isPossibleMsg = false;

  char character;
  
  if (dataContainerLength < textLength) {
    return false;
  }

  resetContainerPosition();
  
  while(!currentPositionEqualsLength()){ 
    character = getCurrentChar();

    if (isEndMessage(isPossibleMsg, possibleMsgPosition, text, textLength, character)) {
      return true;
    }
  }

  return false;
}


/*************************************************
 *************************************************
 *************************************************/

















