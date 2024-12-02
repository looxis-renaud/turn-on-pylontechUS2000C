#include <HardwareSerial.h>


// define the pins for the max485module for RS485 communication
#define DE_RE_PIN 25 // GPIO for DE and RE control
#define TX_PIN 17    // ESP32 TX pin (not used in listening mode)
#define RX_PIN 16    // ESP32 RX pin


unsigned long lastMessageTime = 0; // Tracks the last time a message was received
const unsigned long debugInterval = 4000; // Interval to print "nothing incoming..." (in milliseconds)

void setup() {
  pinMode(DE_RE_PIN, OUTPUT);
  digitalWrite(DE_RE_PIN, LOW); // Always in receive mode

  Serial.begin(115200); // For debugging output
  Serial.println("RS485 Listener Initialized");

  Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); // Configure UART1 for RS485 (9600 baud, 8 data bits, no parity, 1 stop bit)
}

void loop() {
  // Check for incoming RS485 data
  if (Serial1.available()) {
    String receivedData = Serial1.readString(); // Read the incoming RS485 data
    Serial.println("Received: " + receivedData); // Print it to the Serial Monitor
    lastMessageTime = millis(); // Update last message time
  }

  // Check if it's time to print "nothing incoming..."
  if (millis() - lastMessageTime > debugInterval) {
    Serial.println("nothing incoming...");
    lastMessageTime = millis(); // Reset the timer to avoid repeated messages
  }
}
