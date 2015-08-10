//Receiver

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

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
  
  radio.begin();  
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
  radio.startListening();  
  Serial.println("Listening");
}

void loop()
{
  
    // if there is data ready
    if ( radio.available() )
    {
      Serial.print("Receiver.");
      char transmission;
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &transmission, 1 );

        // Spew it
        Serial.print("Received from transmitter:");
        Serial.println(transmission);
 
  // Delay just a little bit to let the other unit
  // make the transition to receiver
  delay(20);
      }

      // First, stop listening so we can talk
      radio.stopListening();

      // Send the final one back.
      byte response = B0;
      radio.write( &response, sizeof(response) );
      Serial.println("Sent response.\n\r");

      // Now, resume listening so we catch the next packets.
      radio.startListening();

    }  
  
}