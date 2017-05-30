#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

const int E1 = 5;  
const int M1 = 4;
const int E2 = 6;  
const int M2 = 7;

#define trigPin 2
#define echoPin 3

const int obstacle_pin = 8; // Use this pin to connect an LED. If an obstacle is detected, the LED will turn on.
                            // Beware that I have not shown this connection in the schematic, it is simply here for
                            // debugging.

class RF24Test: public RF24
{
public: RF24Test(int a, int b): RF24(a,b) {}
};

RF24Test radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting car.");
  pinMode(obstacle_pin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  radio.begin();  
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
  radio.startListening();  
}

void loop()
{
    // if there is data ready
    if ( radio.available() )
    {
      byte transmission[7];
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &transmission, 7 );

        // Spew it
        Serial.print(transmission[0]);
        Serial.print(" ");
        Serial.print(transmission[1]);
        Serial.print(" ");
        Serial.print(transmission[2]);
        Serial.print(" ");
        Serial.print(transmission[3]);
        Serial.print(" ");
        Serial.print(transmission[4]);
        Serial.print(" ");
        Serial.print(transmission[5]);
        Serial.print(" ");
        Serial.print(transmission[6]);
        Serial.print("   ");
        
        byte speedValue = transmission[1];
        byte turnValue = transmission[0];
        
        if (obstacle()) 
        {
//          speed(128);   //This will stop the motor from spinning
          speed(speedValue, true);          
        }
        else
        {
          speed(speedValue, false);
        }

        turn(turnValue);
 
	// Delay just a little bit to let the other unit
	// make the transition to receiver
	delay(10);
      }

      // First, stop listening so we can talk
      radio.stopListening();

      // Send the final one back.
      byte response = B0;
      radio.write( &response, sizeof(response) );

      // Now, resume listening so we catch the next packets.
      radio.startListening();
    } else 
  {
    //speed(128, false); //If there is no contact with the controller, stop the car
  }  
}

void speed(byte speedValue, bool obstacle)
{
  if (speedValue < 127)
         {
            int mappedVal = map(speedValue,0,126,0,255);
            //Going reverse
            Serial.print(mappedVal);
            Serial.print(" - ");
            Serial.println(255-mappedVal);
            digitalWrite(M1,LOW);        
            analogWrite(E1, 255-mappedVal);   //PWM Speed Control
            delay(10); 
         } else if (speedValue > 129 && !obstacle )
         {
            //Going forward
//            int mappedVal = map(speedValue-128,0,255,0,255);
            int mappedVal = map(speedValue,130,255,0,255);
            Serial.print(mappedVal);
            Serial.print(" - ");
            Serial.println(255-mappedVal);
            digitalWrite(M1,HIGH);         
            analogWrite(E1, 255-mappedVal);   //PWM Speed Control
            delay(10); 
         } else
         {
             Serial.println("Obstacle - stopping");
             digitalWrite(M1,LOW); 
             analogWrite(E1, 0);
         }
}

void turn(byte turnValue)
{
  if (turnValue < 125)
         {
           int mappedVal = map(turnValue,0,255,0,255);
           //Turn righ
           digitalWrite(M2,LOW);        
//           analogWrite(E2, mappedVal);   //PWM Speed Control
            analogWrite(E2, 255);
           delay(10); 
         } else if (turnValue > 130)
         {
           //Turn left
           int mappedVal = map(turnValue-128,0,255,0,255);
           digitalWrite(M2,HIGH);         
//           analogWrite(E2, mappedVal);   //PWM Speed Control
           analogWrite(E2, 0); 
           delay(10); 
         } else
         {
           digitalWrite(M2,LOW);
           analogWrite(E2, 0); //Set turning wheels to straight.
         }
}

boolean obstacle()
{
  long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;

  if (distance >= 30 || distance <= 0){
    Serial.println("No obstacle");
    digitalWrite(obstacle_pin,LOW);
    return false;
  }
  else {
//    Serial.print("DANGER! ");
    Serial.print(distance);
    Serial.println(" cm");
    digitalWrite(obstacle_pin,HIGH);
    return true;
  }
}
