void setup() {
	Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop() {
	Serial.println("master:slave1:hello world");
	delay(1000);
	Serial.println("master:slave2:hello world");
	delay(1000);
	Serial.println("master:slave3:hello world");
	delay(1000);
}
