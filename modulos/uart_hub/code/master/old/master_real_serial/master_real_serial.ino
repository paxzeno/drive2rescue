const String ARDUINO_NAME = "MASTER";

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
	hello("PUMP");
	delay(10000);
  hello("GPS");
  delay(10000);
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

 void hello(String destination) {

    String sendMessage = "MASTER:";
    sendMessage.concat(destination);
    sendMessage.concat(":AYT");

    Serial.println(sendMessage);

    while(true) {
      struct MGS_PARTS msgParts = listeningSerialChannel();
  
      String orig = msgParts.origin;
      String dest = msgParts.destination;
      String opr = msgParts.operation;
      String data = msgParts.data;
  
      if (dest == ARDUINO_NAME && opr == "AYT") {
        return;
      }
      else {
        Serial.println(".............");
        Serial.println("#" + orig + "#");
        Serial.println("#" + dest + "#");
        Serial.println("#" + opr + "#");
        Serial.println("#" + data + "#");
        Serial.println(".............");
      }
    }

    return;
 }

