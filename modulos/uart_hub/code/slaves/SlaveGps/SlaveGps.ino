#include <SoftwareSerial.h>

SoftwareSerial softSerialA(10, 11); // RX, TX
SoftwareSerial softSerialB(7, 6); // RX, TX

/*************************************
 *  wait for Serial B response timeout
 *************************************/
const long timeWaitForSerialBResponse = 20000; // 20 seconds

const String ARDUINO_NAME = "GPS";
const char DIVIDER = '|';

struct MGS_PARTS {
  String origin;
  String destination;
  String operation;
  String data;
};

void setup() {
  
  //Serial.begin(9600); // DEBUG
  //while (!Serial) {
  //  ;
  //}

  softSerialA.begin(9600); // Talking to Master
  softSerialB.begin(9600); // Talking to GPS
  
  pinMode(13, OUTPUT);
}

void loop() {
  //String message = listeningSerialChannel();

  struct MGS_PARTS msgParts = listeningSerialChannel();

  if (isThisMe(msgParts.destination)) {
    digitalWrite(13, HIGH); // start processing information
    
    runOperation(msgParts);

    softSerialA.println(buildResponseMessage(msgParts));

    digitalWrite(13, LOW);  // end processing information
  }
}


/**************************************************
 * Wait for something in the serial channel
 **************************************************/
struct MGS_PARTS listeningSerialChannel() {
  String content = ""; // complete message
  char character; // one character

  softSerialA.listen();
  
  while(true) {
    if (softSerialA.available() > 0) {
      character = softSerialA.read();
      //Serial.write(character); // DEBUG
      content.concat(character);
    }

    //if (character == 13) { // o \n (10) passava para a mensagem seguinte.
    if (character == 10) {
      return buildMessageParts(content);
    }
  }
}

/**************************************************
 * Identification
 **************************************************/
boolean isThisMe(String name) {
  return (name == ARDUINO_NAME) ? true : false;
}

/**************************************************
 * Parse raw message and build each message part 
 **************************************************/
struct MGS_PARTS buildMessageParts(String message) {
  // Ori:Dest:Opr:Data
  int endOri = message.indexOf(DIVIDER); // Ori:
  int endDest = message.indexOf(DIVIDER, endOri + 1); // Dest:
  int endOpr = message.indexOf(DIVIDER, endDest + 1); // Opr:

  struct MGS_PARTS messageParts;

  if (endOri != -1 && endDest != -1) {
    String origin = message.substring(0, endOri);
    String destination = message.substring(endOri + 1, endDest);
    String operation = (endOpr != -1 ? message.substring(endDest + 1, endOpr) : message.substring(endDest + 1, message.length() - 2));
    String data = (endOpr != -1 ? message.substring(endOpr + 1, message.length() - 2) : F("null"));

    // na mensagem segunte estava a passar o \n da mensagem anterior.
    //String operation = (endOpr != -1 ? message.substring(endDest + 1, endOpr) : message.substring(endDest + 1, message.length() - 1));
    //String data = (endOpr != -1 ? message.substring(endOpr + 1) : F("null"));

    messageParts = { origin, destination, operation, data };
  } else {
    messageParts = { "null", "null", "null", "null" };
  }

  return messageParts;
}

/**************************************************
 * Rebuild response message
 **************************************************/
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

/**************************************************
 * Operations
 **************************************************/

/* hello response */
String helloResponse() {
  return F("OK");
}

/* Wait for GPS information */
String readGpsPosition() {
  String messageType = "GPGLL"; // $GPGLL,3939.53990,N,00822.03619,W,122202.00,A,D*7C
  String content = ""; // complete message
  char character; // one character

  unsigned long startMillis = millis(); // time to timeout

  softSerialB.listen(); // start listening to Serial B
  
  while(true) {
    if (softSerialB.available() > 0) {
      character = softSerialB.read();
      content.concat(character);

      startMillis = millis(); // refresh counter.
      
    } else if ((millis() - startMillis) >= timeWaitForSerialBResponse) { // check if is timeout time
        return F("TIMEOUT");
    }

    if (character == 13) {
      if (content.indexOf(messageType) != -1) {
        content.trim();
        return content;
      }

      content = F("");
      character = 0;
    }
  }
}

String readPumpStatus() {
  return F("1");
}

String readWaterTank() {
  return F("3");
}

String readGsmgOnlineStatus() {
  return F("1");
}

/**************************************************
 * Operations Manager
 **************************************************/
void runOperation(struct MGS_PARTS &msgParts) {
  String opr = msgParts.operation;
  String dest = msgParts.destination;

  if (opr == F("AYT")) {
    msgParts.data = helloResponse();   
  } else if (opr == F("STATUS") && dest == F("PUMP")) {
    msgParts.data = readPumpStatus();
  } else if (opr == F("LEVEL") && dest == F("WTANK")) {
    msgParts.data = readWaterTank();
  } else if (opr == F("WAI") && dest == F("GPS")) {
    msgParts.data = readGpsPosition();
  } else if (opr == F("ISONLINE") && dest == F("3G")) {
    msgParts.data = readGsmgOnlineStatus();
  }
}
