const String ArduinoName = "slave1";

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop() {
  String message = listeningSerialChannel();

  if (isThisMe(message)) {
    Serial.println("yes");
  } else {
    Serial.println("no");
  }

  Serial.println(getOperation(message));
}

String listeningSerialChannel() {
  String content = ""; // complete message
  char character; // one character
  
  while(true) {

    if (Serial.available() > 0) {
      character = Serial.read();
      content.concat(character);
    }

    if (character == 13) {
      return content;
    }
  }
}

boolean isThisMe(String message) {
  // master:slave:operation
  int start = message.indexOf(':'); // master:
  int end = message.indexOf(':', start + 1); // slave:

  if (start != -1 && end != -1) {
    if (message.substring(start + 1, end) == ArduinoName) {
      return true;
    }
  }
  
  return false;
}

String getOperation(String message) {
  // master:slave:operation
  int start = message.indexOf(':'); // master:
  int end = message.indexOf(':', start + 1); // slave:

  if (start != -1 && end != -1) {
    return message.substring(end + 1);  
  }

  return "null";
}
