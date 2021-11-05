// #include "Arduino.h"

// int speakerPin = 8;

// void setup() {
//   pinMode(speakerPin, OUTPUT);
//   Serial.begin(9600);
// }

// void spaceGun(int maximum)
// {
//   for (int i=0; i < maximum; i+=2) {
//     digitalWrite(speakerPin, HIGH);
//     delayMicroseconds(i);
//     digitalWrite(speakerPin, LOW);
//     delayMicroseconds(i);    
//   }
// }

// void randSound(int maximum)
// {
//   tone(speakerPin,random(maximum,10*maximum));
//   delay(maximum);
// }

// void fibonacci(int maximum)
// {
//   long fib = 1;
//   long fib1 = 1;  
//   long fib2 = 2;
//   for (int i=0; i < maximum; i++) {
//     fib = fib1+fib2;
//     fib1 = fib2;
//     fib2 = fib;
//     tone(speakerPin,fib);
//     delay(200);
//   }
//   noTone(speakerPin);
// }

// void loop() {
//   spaceGun(1000);
//   delay(5000);  
//   fibonacci(20);
//   delay(5000);  
//   //randSound(50);
//   //delay(5000);
// }
