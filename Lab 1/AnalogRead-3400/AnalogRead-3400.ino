const byte PINNAME = A0;
void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    pinMode(7, OUTPUT);
    digitalWrite(7, HIGH);
    pinMode(PINNAME, INPUT);
    pinMode(6, OUTPUT);
    
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  int val = analogRead(PINNAME);
  Serial.println(val);
  int newVal = map(val, 0,1023,0,255);
  analogWrite(6, newVal);
}
