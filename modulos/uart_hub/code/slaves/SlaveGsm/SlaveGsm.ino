#include <SoftwareSerial.h>

SoftwareSerial softSerialA(10, 11); // RX, TX
SoftwareSerial softSerialB(7, 6); // RX, TX

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
const String ARDUINO_NAME = "3G";
const char DIVIDER = '|';

struct MGS_PARTS {
  String origin;
  String destination;
  String operation;
  String data;
};

void setup() {
  softSerialA.begin(9600);
  softSerialB.begin(4800); // GSM communication
  
  pinMode(13, OUTPUT);
}

void loop() {
  //String message = listeningSerialChannel();

  struct MGS_PARTS msgParts = listeningSerialChannel();

  if (isThisMe(msgParts.destination)) {
    digitalWrite(13, HIGH); // start processing information
    
    runOperation(msgParts);
    //softSerialA.println(buildResponseMessage(msgParts));
    //softSerialA.print(buildResponseMessage(msgParts));
    //softSerialA.print("\r\n");

    delay(1000);            // wait for a second
    digitalWrite(13, LOW);  // end processing information
  }
}

/**************************************************
 * MSG Management
 **************************************************/

/* Wait for something in the serial channel */
struct MGS_PARTS listeningSerialChannel() {
  String content = ""; // complete message
  char character; // one character

  softSerialA.listen();
  
  while(true) {

    if (softSerialA.available() > 0) {
      character = softSerialA.read();
      content.concat(character);
    }

    if (character == 13) {
      return buildMessageParts(content);
    }
  }
}

/* Identification */
boolean isThisMe(String name) {
  return (name == ARDUINO_NAME) ? true : false;
}

/* Parse raw message and build each message part */
struct MGS_PARTS buildMessageParts(String message) {
  // Ori:Dest:Opr:Data
  int endOri = message.indexOf(DIVIDER); // Ori:
  int endDest = message.indexOf(DIVIDER, endOri + 1); // Dest:
  int endOpr = message.indexOf(DIVIDER, endDest + 1); // Opr:

  struct MGS_PARTS messageParts;

  if (endOri != -1 && endDest != -1) {
    String origin = message.substring(0, endOri);
    origin.trim();

    String destination = message.substring(endOri + 1, endDest);
    destination.trim();

    String operation = (endOpr != -1 ? message.substring(endDest + 1, endOpr) : message.substring(endDest + 1, message.length() - 1));
    operation.trim();

    String data = (endOpr != -1 ? message.substring(endOpr + 1) : "null");
    data.trim();
    
    messageParts = { origin, destination, operation, data };
  } else {
    messageParts = { "null", "null", "null", "null" };
  }

  return messageParts;
}

/* Rebuild response message */
String buildResponseMessage(struct MGS_PARTS &msgParts) {
  String message = "";

  message.concat(msgParts.destination);
  message.concat(DIVIDER);
  message.concat(msgParts.origin);
  message.concat(DIVIDER);
  message.concat(msgParts.operation);
  message.concat(DIVIDER);
  message.concat(msgParts.data);

  return message;
}

void sendResponseMessage(struct MGS_PARTS &msgParts, boolean &printNewLine) {
  //softSerialA.println(buildResponseMessage(msgParts));
    softSerialA.print(buildResponseMessage(msgParts));

    if (printNewLine == true)
      softSerialA.print("\r\n");
}

/**************************************************
 * Operations
 **************************************************/

/* hello response */
String helloResponse() {
  return "OK";
}

String readGpsPosition() {
  return "$GPGLL,3939.53990,N,00822.03619,W,122202.00,A,D*7C";
}

String readPumpStatus() {
  return "1";
}

String readWaterTank() {
  return "3";
}

String readGsmgOnlineStatus() {
  return "1";
}

byte sendDataToGsmChannel() {
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


/**************************************************
 * Operations Manager
 **************************************************/
void runOperation(struct MGS_PARTS &msgParts) {
  String opr = msgParts.operation;
  String dest = msgParts.destination;

  // prints new line to signal the master that this is the end message
  boolean printNewLine = true; 
  boolean sendGsmDataContent = false;
  
  // just used in GSM send data
  byte outputCode;

  if (opr == "AYT") {
    msgParts.data = helloResponse();   
  } else if (opr == "STATUS" && dest == "PUMP") {
    msgParts.data = readPumpStatus();
  } else if (opr == "LEVEL" && dest == "WTANK") {
    msgParts.data = readWaterTank();
  } else if (opr == "WAI" && dest == "GPS") {
    msgParts.data = readGpsPosition();
  } else if (opr == "ISONLINE" && dest == "3G") {
    msgParts.data = readGsmgOnlineStatus();
  } else if (opr == "SEND_DATA" && dest == "3G") {
    outputCode = sendDataToGsmChannel();
    msgParts.data = "";
    printNewLine = false;
    sendGsmDataContent = true;
  }

  sendResponseMessage(msgParts, printNewLine);

  if (sendGsmDataContent == true) {
    getOutput(outputCode);
    softSerialA.print("\r\n");
    closeGsmConnection();
  }
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
      softSerialA.println(F("ERROR_RETURN_FROM_AT_COMMAND"));
    break;
    case 1:
      printOutput();
    break;
    case 2:
      softSerialA.println(F("TIMEOUT_WAITING_FOR_GSM_RESPONSE"));
    break;
    case 3:
      softSerialA.println(F("NOT_CONNECTED"));      
    break;
    case 4:
      softSerialA.println(F("HTTP_GET_PAGE_ERROR"));
    break;    
    case 5:
      softSerialA.println(F("ERROR_START_GSM"));
    break;
    case 6:
      softSerialA.println(F("TIMEOUT_START_GSM"));
    break;
    case 7:
      softSerialA.println(F("ERROR_OPENING_GSM_CONNECTION"));
    break;
    case 8:
      softSerialA.println(F("TIMEOUT_OPENING_GSM_CONNECTION"));
    break;
    case 9:
      softSerialA.println(F("ERROR_IN_HTML_DATA_RETURN"));
    break;
    case 10:
      softSerialA.println(F("TIMEOUT_WHILE_WAITING_FOR_HTML_DATA"));
    break;
    case 11:
      softSerialA.println(F("ERROR_WHILE_OPENNING_HTML_CONNECTION"));
    break;
    case 12:
      softSerialA.println(F("TIMEOUT_WHILE_OPENNING_HTML_CONNECTION"));
    break;
  }
}

/* send data from container to uart hub channel */
void printOutput() {
  resetContainerPosition();
  char character;
  
  while(!currentPositionEqualsLength()) { 
    character = getCurrentChar();

    if (character == 10) {
      character = '_';
    } else if (character == 13) {
      character = '_';
    }

    softSerialA.print(character); 
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
 * GSM Response container management
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


















