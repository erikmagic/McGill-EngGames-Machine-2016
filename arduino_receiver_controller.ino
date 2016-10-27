/*
 * Erik-Olivier Riendeau, McGill Jeux du Genie, 2016 .
 * 
 * Communication system part 2 THIS IS THE RECEIVER
 * 
 * This code handles the arduino MEGA connected to the robot. The code receives the
 * data coming from the gamepad though a pair of RF24 transmitters.
 * 
 * The proces is the following:
 * 
 * Logitech Controller -> usb -> Processing program -> Sender arduino program ->
 * Serial commmunication (usb) -> Arduino -> RF24 -> RF24 -> this.Arduino program.
 * 
 * There is 13 buttons on the logitech controller, 2 joysticks and 1 hat.
 * 
 * Required libraries are: 
 *  ->Vsync to synchronize variables between Arduino and
 *    processing. You can download and get more information here : 
 *    http://erniejunior.github.io/VSync-for-Processing/.
 *  ->RF24 to transfer data from the transmitter
 *    https://maniacbug.wordpress.com/2011/03/19/2-4ghz-rf-radio-transceivers-and-library-8/
 *  
 * The arduino is an arduino MEGA, so some pins needs to be adjusted from the following
 * library,
 * The RF24 is connected to the Arduino as followed:
 * (  https://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo )
 * It is wired following the way it was wired in the TMRh20 library.
 * ( RF24 pin --> Arduino pin )
 *  1 (GND) --> GND
 *  2 (VCC) --> 3.3v
 *  3 (CE) --> 49
 *  4 (CSN) --> 53
 *  5 (SCK) --> 52
 *  6 (MOSI) --> 51
 *  7 (MISO) --> 50
 *  8 (IRQ) --> NOT USED
 *  
 * The code is maily adapted from 
 * https://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-ExampleSketches#js1
 * for what concerns the data transmission between the two arduinos.
 * 
 * 
 */


// ------------------ impor needed libraries --------------- //
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

// --------------- used pins definition for arduino ---------- //
#define CE_PIN  49
#define CSN_PIN 53

// -------------------- declare object ----------------- //
RF24 radio(CE_PIN, CSN_PIN);


// -------------------- declare variables ----------------- //

// Debug variables to pass back to processing using vSync.
int debugVal0, debugVal1, debugVal2, debugVal3; 

byte addresses[][6] = {"1Node", "2Node"}; // name of the pipes
unsigned long timeNow; // grabs current time
unsigned long started_waiting_at;
boolean timeout; 

boolean hasHardware = false; // allows testing w/o hardware
//boolean hasHardware = true;


// Joystick input variables -- received from the vSync library object
struct dataStruct{
  unsigned long _micros; // save response time
  int hatPosition, leftX, leftY, rightX, rightY; // 
  int button[13];
} myData;



// ----------------- SETUP RUNS ONCE! --------------------- //
void setup() {
  Serial.begin(115200); // transmission baudrate of 115200
  // the F() macro puts the printf() formatting strings into flash.
  Serial.println(F("Receives   gamepad data to the robot using RF24 transmitters"));
  printf_begin(); // needed for "printDetails"

  radio.begin(); // initialize radio
  radio.setChannel(108); // from 1 to 127, higher values are above wifi frequencies
  radio.setDataRate(RF24_250KBPS); // data rate set 

  // set the power amplifier level of the rf24 to low in order to avoid problems
  radio.setPALevel(RF24_PA_LOW);

  // open the writing and reading pipe on each radio with opposite addresses
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);

  // start radio listening for data
  radio.startListening(); 

  // debugging informations
  //radio.printDetails();



}
// ------------------- LOOPS CONSTANTLY ------------------------ //
void loop() {
    if ( radio.available())
    {
  
      while (radio.available())   // While there is data ready to be retrieved from the receive pipe
      {
        radio.read( &myData, sizeof(myData) );             // Get the data
      }
  
      radio.stopListening();                               // First, stop listening so we can transmit
      radio.write( &myData, sizeof(myData) );              // Send the received data back.
      radio.startListening();                              // Now, resume listening so we catch the next packets.
  
      Serial.print(F("Packet Received - Sent response "));  // Print the received packet data
      Serial.println(myData._micros);
      // add commands received from the gamepad
      Serial.print(F("Button[0] = "));
      Serial.println(myData.button[0]);
      Serial.print(F("Button[1] = "));
      Serial.println(myData.button[1]);
      Serial.print(F("Button[2] = "));
      Serial.println(myData.button[2]);
      Serial.print(F("Button[3] = "));
      Serial.println(myData.button[3]);
      Serial.print(F("LEFT hat position x: "));
      Serial.print(myData.leftX);
      Serial.print(F("LEFT hat position y: "));
      Serial.println(myData.leftY);
      /*
      for(int x = 0; x < 13; x++)
      {
        Serial.print(myData.button[x]);
        Serial.print(F(" "));
      }
      for(int x = 0; x < 13; x++)
      {
        myData.button[x]++;
      }
      */
    }
    if (hasHardware)
    {
      // add what to do in the robot itself
    }
}
