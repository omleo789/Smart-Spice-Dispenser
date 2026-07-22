#include <SoftwareSerial.h>
#include <Servo.h>

SoftwareSerial mySerial(2, 3); // RX on Pin 2

// Define 5 Servo objects
Servo servoA; // Now Pin 7
Servo servoB; // Now Pin 8
Servo servoC; // Now Pin 9
Servo servoD; // Now Pin 10
Servo servoE; // Now Pin 11

const int ledPin = 13;

void setup() {
  pinMode(2, INPUT_PULLUP); // Internal pull-up to keep line stable
  mySerial.begin(9600);
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // Servos are detached by default
}

// Universal sweep function (30 degree movement)
void moveServo(Servo &s, int pin) {
  s.attach(pin); 
  
  // Sweep Out: 0 to 30 degrees
  for (int pos = 0; pos <= 180; pos += 5) {
    s.write(pos);
    delay(20);
  }
  
  // Sweep Back: 30 to 0 degrees
  for (int pos = 180; pos >= 0; pos -= 5) {
    s.write(pos);
    delay(20);
  }

  s.detach(); // Stop signal to prevent jitter/overheating
}

void loop() {
  if (mySerial.available() > 0) {
    char data = mySerial.read();

    // Check for valid button characters
    if (data == 'A' || data == 'B' || data == 'C' || data == 'D' || data == 'E') {
      
      digitalWrite(ledPin, HIGH); // LED ON during action

      // Updated Pin Assignments
      if (data == 'A')      moveServo(servoA, 7);  
      else if (data == 'B') moveServo(servoB, 8);  
      else if (data == 'C') moveServo(servoC, 9); 
      else if (data == 'D') moveServo(servoD, 10); 
      else if (data == 'E') moveServo(servoE, 11); 

      digitalWrite(ledPin, LOW); // LED OFF when done

      // Clear the serial buffer to prevent repeat triggers
      while(mySerial.available() > 0) {
        mySerial.read();
      }
    }
  }
}
Arduino
