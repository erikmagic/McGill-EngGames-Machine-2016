/*
 * Erik-Olivier Riendeau, McGill Jeux du Genie, 2016
 * 
 * Communication system part 2 THIS IS THE SENDER
 * 
 * This code handles the arduino connected to the laptop. The code transfers
 * the data coming from the gamepad controller ( Logitech f310) to another Arduino 
 * connected to our robot.
 * 
 * The proces is the following:
 * 
 * Logitech Controller -> usb -> Processing program -> this.Arduino program ->
 * Serial commmunication (usb) -> Arduino .
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
 * The arduino itself is connected serially to the computer using its standard
 * serial usb cable. The arduino is also connected to a breadboard which features
 * a power module to correct some problems with the RF24 transmitter power input.
 * The RF24 is connected to the Arduino as followed:
 * (  https://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo )
 * It is wired following the way it was wired in the TMRh20 library.
 * ( RF24 pin --> Arduino pin )
 *  1 (GND) --> GND
 *  2 (VCC) --> 3.3v
 *  3 (CE) --> 7
 *  4 (CSN) --> 8
 *  5 (SCK) --> 13
 *  6 (MOSI) --> 11
 *  7 (MISO) --> 12
 *  8 (IRQ) --> NOT USED
 *  
 * The code is maily adapted from 
 * https://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-ExampleSketches#js1
 * for what concerns the data transmission between the two arduinos.
 * 
 * 
 */

// ---------------INCLUDED LIBRARIES --------------- //
#include "VSync.h"  
#include "RF24.h"
#include <SPI.h>
#include "printf.h"
// --------------- used pins definition for arduino ---------- //
#define CE_PIN 7
#define CSN_PIN 8

// -------------------- declare object ----------------- //
RF24 radio(CE_PIN, CSN_PIN);

ValueReceiver<17> receiver; // set # to num items received from Processing 
                            // using vSync library.
ValueSender<4> sender;

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




/*******************
/ setup function 
/*******************/
void setup()
{
  Serial.begin(115200); // transmission baudrate of 115200
  // the F() macro puts the printf() formatting strings into flash.
  Serial.println(F("Send gamepad data to the robot using RF24 transmitters"));
  printf_begin(); // needed for "printDetails"

  radio.begin(); // initialize radio
  radio.setChannel(108); // from 1 to 127, higher values are above wifi frequencies
  radio.setDataRate(RF24_250KBPS); // data rate set 

  // set the power amplifier level of the rf24 to low in order to avoid problems
  radio.setPALevel(RF24_PA_LOW);

  // open the writing and reading pipe on each radio with opposite addresses
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);

  // start radio listening for data
  radio.startListening(); 

  // debugging informations
  //radio.printDetails();
  

// -------------------------------------------------------------- //

// the following block initializes the variables sent and received by the  
// Processing sketch

   if (hasHardware)
   {
    receiver.observe(myData.hatPosition);
    receiver.observe(myData.leftX);
    receiver.observe(myData.leftY);
    receiver.observe(myData.rightX);
    receiver.observe(myData.rightY);
  
    for(int x = 0; x < 13; x++)
      receiver.observe(myData.button[x]);
  
    sender.observe(debugVal0);
    sender.observe(debugVal1);
    sender.observe(debugVal2);
    sender.observe(debugVal3);
   }
   else // giving bogus values
   {
    myData.hatPosition =0;
    myData.leftX =0;
    myData.leftY =0;
    myData.rightX =0;
    myData.rightY =0;
    
    for(int x = 0; x < 13 ; x++)
    {
      myData.button[x] = x;
    }

    debugVal0 = 0;
    debugVal1 = 1;
    debugVal2 =2;
    debugVal3 = 3;
    
   }

// ----------------------------------------------------------------- //


}

/*******************
/ loop function 
/*******************/
void loop()
{ 
  if (hasHardware)
  {
    receiver.sync();     // vSync sync variables between arduino / processing
    sender.sync();
  }
  else
   {
    // give some values to test whether the communication itself is working
    // without using Processing
   }

   myData._micros = micros(); //sent back for timing

   Serial.print(F("Now sending - "));

   if (!radio.write( &myData, sizeof( myData))) // check for errors
   {
    Serial.println(F("transmit failed"));
   }

   radio.startListening(); // continues listening 
   started_waiting_at = micros(); // keep tracks of time
   timeout = false; // timeout is false 

  
   while (!radio.available() ) { // indicate timeout after 200 ms waited
    if( micros() - started_waiting_at > 200000 ) 
    { 
      timeout = true;
      break;
    }
   }

   if (timeout) // if a time out occured
   {
    Serial.print(F("Response timed out"));
   }
   else
   {
    // grab the response, compare and send
    radio.read( &myData, sizeof(myData));
    timeNow = micros();

    // display
    Serial.print(F("Sent "));
    Serial.print(timeNow);
    Serial.print(F(", Got response "));
    Serial.print(myData._micros);
    Serial.print(F(", Round-trip delay "));
    Serial.print(timeNow -myData._micros);
    Serial.print(F(" microseconds "));
   }

  // send again after input
  delay(100);
  


   
}



