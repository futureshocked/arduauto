#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9, 10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

void setup()
{
  Serial.begin(57600);

  radio.begin();

  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1, pipes[0]);

  radio.startListening();
  Serial.println("Listening...");
}

void loop()
{
  // if there is data ready
  if ( radio.available() )
  {
    byte transmission[7];

    while (radio.available()) {
      radio.read( &transmission, sizeof(transmission));
    }

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

    // Delay just a little bit to let the other unit
    // make the transition to receiver
    delay(20);

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
