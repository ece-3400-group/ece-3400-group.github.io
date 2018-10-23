
#include "radios.h"
#include "printf.h"

// Send two packets per transmission
//
// volatile byte positionPacket; // bits 0-3 encode x-coordinate, bits 4-7 encode y-coordinate
//                     // current position of LABron
// volatile byte metaPacket;
// bit 0-1 = [00] North [11] South [10] West [01] East
// bit 2 = 1 if wall detected here
// bit 3 = 1 if  other robot detected
// bit 4 = 1 if visited before
// bit 5-6 = treasure shape (00 NONE, 01 TRIANGLE, 10 CIRCLE, 11 SQUARE)
// bit 7-8 = treasure color (00 NONE, 01 RED, 10 BLUE, 11 GREEN)

// we do not need two packets
// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
RF24 radio(9, 10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0x0000000070LL, 0x0000000071LL };
// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_pong_back = 0} role_e;
// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

// The role of the current running sketch
role_e role = role_ping_out;

void setupRadios(void)
{
  pinMode(RADIO_DEBUG_LED, OUTPUT);
  printf_begin();
  printf("\nRF24/examples/GettingStarted/\n");
  printf("ROLE: "); printf(role_friendly_name[role]);
  //
  // Setup and configure rf radio
  //
  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15, 15);
  radio.setAutoAck(true);
  // set the channel
  radio.setChannel(0x50);
  // set the power
  // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  radio.setPALevel(RF24_PA_MIN);
  //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_250KBPS);
  //printf("MIDDLE of radio steup\n");
  //
  // Dump the configuration of the rf unit for debugging
  //

 
  role = role_ping_out;
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1, pipes[1]);
  
  radio.startListening();
  radio.printDetails();
 // printf("end of radio steup\n");

}

byte packetTransmission(int positionPacket, byte DEBUG=1)
{
  //
  // Ping out role.  Repeatedly send the current time
  //
  byte success = 0;
  // First, stop listening so we can talk.
  //Serial.println("IN PACKET TRANSMISSION");
  radio.stopListening();
  //Serial.println("Stop Listening");

  // Take the time, and send it.  This will block until complete
  unsigned long startTime = millis();
  Serial.print("Now sending %lu..."); 
  bool ok1 = radio.write( &positionPacket, sizeof(int));
 // bool ok2 = radio.write( &metaPacket, sizeof(metaPacket));
  Serial.println("Written");

  if (ok1)
    Serial.println("Transmission sent with positionPacket and metaPacket");
  else
    Serial.println("Transmission failed.\n\r");

  // Now, continue listening
  radio.startListening();

  // Wait here until we get a response, or timeout (250ms)
  unsigned long started_waiting_at = millis();
  bool timeout = false;
  while ( ! radio.available() && ! timeout )
    if (millis() - started_waiting_at > 200 )
      timeout = true;

  // Describe the results
  if ( timeout )
  {
   // Serial.println(F("Failed! Response timed out.\n\r"));
  }
  else
  {
    // Grab the response, compare, and send to debugging spew
    unsigned long got_time;
    radio.read( &got_time, sizeof(got_time) );

    // Spew it
    if (DEBUG)
    success = 1;
    digitalWrite(RADIO_DEBUG_LED, !digitalRead(RADIO_DEBUG_LED));
    Serial.print("Got response"); Serial.print(got_time);
    //Serial.print(", round-trip delay: "); Serial.println(millis() - got_time);
  }
  return success;
  // Try again 100ms later
  //delay(100);
}
// vim:cin:ai:sts=2 sw=2 ft=cpp
