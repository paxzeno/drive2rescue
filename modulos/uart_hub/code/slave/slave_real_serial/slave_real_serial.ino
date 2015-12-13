const String ARDUINO_NAME = "GPS";

struct MGS_PARTS {
  String origin;
  String destination;
  String operation;
  String data;
};

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop() {
  //String message = listeningSerialChannel();

  struct MGS_PARTS msgParts = listeningSerialChannel();

  if (isThisMe(msgParts.destination)) {
    runOperation(msgParts);
    Serial.println(buildResponseMessage(msgParts)); 
  }
}


/**************************************************
 * Wait for something in the serial channel
 **************************************************/
struct MGS_PARTS listeningSerialChannel() {
  String content = ""; // complete message
  char character; // one character
  
  while(true) {

    if (Serial.available() > 0) {
      character = Serial.read();
      content.concat(character);
    }

    if (character == 13) {
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
  int endOri = message.indexOf(':'); // Ori:
  int endDest = message.indexOf(':', endOri + 1); // Dest:
  int endOpr = message.indexOf(':', endDest + 1); // Opr:

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

/**************************************************
 * Rebuild response message
 **************************************************/
String buildResponseMessage(struct MGS_PARTS &msgParts) {
  String message = "";

  message.concat(msgParts.destination);
  message.concat(":");
  message.concat(msgParts.origin);
  message.concat(":");
  message.concat(msgParts.operation);
  message.concat(":");
  message.concat(msgParts.data);

  return message;
}

/**************************************************
 * Operations
 **************************************************/

/* hello response */
String helloResponse() {
  return "OK";
}

String readGps() {
  return "41 24.2028, 2 10.4418";
}

String readPumpStatus() {
  return "1";
}

String readWaterTank() {
  return "3";
}

String read3gOnlineStatus() {
  return "1";
}

/**************************************************
 * Operations Manager
 **************************************************/
void runOperation(struct MGS_PARTS &msgParts) {
  String opr = msgParts.operation;
  String dest = msgParts.destination;

  if (opr == "AYT") {
    
    msgParts.data = helloResponse();
        
  } else if (opr == "STATUS" && dest == "PUMP") {
    
    msgParts.data = readPumpStatus();
    
  } else if (opr == "LEVEL" && dest == "WTANK") {
    
    msgParts.data = readWaterTank();
    
  } else if (opr == "WAI" && dest == "GPS") {
    
    msgParts.data = readGps();
    
  } else if (opr == "ISONLINE" && dest == "3G") {
    
    msgParts.data = read3gOnlineStatus();
    
  }
}


