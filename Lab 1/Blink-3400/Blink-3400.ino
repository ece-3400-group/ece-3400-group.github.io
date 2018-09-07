/**
 * "Blink-3400.ino" , Team 23
 *  Blinks the external LED of an Arduino
 */
const int LED = 7; // pin 7 to external LED
 
// Setup the pins for the external LED connected to the Arduino
void setup() {
  pinMode(LED, OUTPUT);
}

void loop() {
  digitalWrite(LED, HIGH);     // Turn on LED
  delay(1000);                 // Wait for a second
  digitalWrite(LED, LOW);      // Turn off LED
  delay(1000);                 // wait for a second
}
