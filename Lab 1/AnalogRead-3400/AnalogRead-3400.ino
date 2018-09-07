// Define pin attachments
const int POT = A0;          // attach potentiometer to A0 pin
const int INDICATOR_LED = 7; // attach indicator LED to pin 7
const int LED = 6;

void setup() {
    Serial.begin(9600);      // Setup Serial monitor to look at pot values
    pinMode(POT, INPUT);     // Setup potentiometer as an input

    // Turn on a indicator LED
    pinMode(INDICATOR_LED, OUTPUT);   
    digitalWrite(INDICATOR_LED, HIGH);
}

void loop() {
  int val = analogRead(POT); // Read the potentiometer value
  Serial.println(val);       // Print value to the Serial monitor

  // Convert the potentiometer analog value to 8-bit analog output
  int pot_val = map(val, 0, 1023, 0, 255);
  pot_val = constrain(pot_val, 0, 255);

  // Write the mapped potentiometer value to the LED
  analogWrite(LED, pot_val); 
  delay(100);
}
