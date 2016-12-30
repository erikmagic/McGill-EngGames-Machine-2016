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
 *    http://erniejunior.github.io/VSync-for-Processing/
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
 * This code also controls the robot itself as it is uploaded in the Arduino mega in the robot.
 */





// ------------------ import needed libraries --------------- //
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

// --------------- used pins definition for arduino ---------- //
#define CE_PIN  49
#define CSN_PIN 53
// even though there are 4 motors, the 2 at the right are linked together and the two at the left are linked together
// effectively there are 2 sets of motors which are considered the right and left motors.
#define MOTOR_DIR_L 4 // digital left direction
#define MOTOR_SPD_L 3 // analog left speed
#define MOTOR_DIR_R 2 // digital right direction
#define MOTOR_SPD_R 1 // analog right speed

// -------------------- declare object ----------------- //
RF24 radio(CE_PIN, CSN_PIN);


// -------------------- declare variables ----------------- /



// Debug variables to pass back to processing using vSync.
int debugVal0, debugVal1, debugVal2, debugVal3; 

byte addresses[][6] = {"1Node", "2Node"}; // name of the pipes
unsigned long timeNow; // grabs current time
unsigned long started_waiting_at;
boolean timeout; 

boolean hasHardware = false; // allows testing w/o hardware
//boolean hasHardware = true;

// motor variables
int direction = 0;
int rotation_side = 0;
int speed_right = 0;
int speed_left = 0;

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

  // set pin modes for motors
  pinMode(MOTOR_DIR_L, INPUT);
  pinMode(MOTOR_DIR_R, INPUT);
  pinMode(MOTOR_SPD_L, INPUT);
  pinMode(MOTOR_SPD_R, INPUT);


  // debugging informations
  //radio.printDetails();



}
// ------------------- LOOPS CONSTANTLY ------------------------ //
void loop() {

	// ----------------------- automation part ----------------------- //


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
      for (int i = 0; i < 12; i++){
      	Serial.printf(F("Button[%d] = ", i ));
      	Serial.println(myData.button[i]);
      }
      Serial.printf(F("Hat Position = "));
      Serial.println(myData.hatPosition);
      Serial.printf(F("Right joystick in x = "));
      Serial.println(myData.rightX);
      Serial.printf(F("Right joystick in y = "));
      Serial.println(myData.rightY);
      Serial.printf(F("Left joystick in x = "));
      Serial.println(myData.leftX);
      Serial.printf(F("Left joystick in y = "));
      Serial.println(myData.leftY);
      Serial.printf(F("Hat position = "));
      Serial.println(myData.hatPosition);

    }
    if (hasHardware)
    {
    	// ------------------ driving system  ----------------- //

	      // -output : the final output that will be delivered to the motor 
      int outputRight = 0;
      int outputLeft  = 0;

      // -threshold : the minimum value to get the motors to turn.
      int const threshold = 50;

      // -max : the maximum value of the analog range
      int const max = 255;

      // -joystickX : the x component of the joystick, between -255 and 255, both included.
      //              The joystick used is the LEFT joystick
      int joystickX = myData.leftX;

      // -joystickY : the y component of the joystick, between -255 and 255, both included.
      //              The joystick used is the LEFT joystick.
      int joystickY = myData.leftY;

      // -direction : the direction the robot is going, either forwards (1) or backwards (0). 
      //              Determined by the positive or negative values on joystickY
      int direction = 1;

      // -status : determines whether the input from the joystick will be considered
      //           , this is because at rest the joystick may yield some small values
      //           close to 0 but not exactly 0. Default status is 1, but if the input is
      //           detected to be very small, the status will be 0.
      int status = 1;

      if (joystickY > -10 && joystickY < 10) status = 0;

      if (status){

        // Y input - throttle 

        // if the joystickY is negative, change the direction and update the joystickY to positive
        if (joystickY < 0){
          direction = 0;
          joystickY *= -1;
        }

        // if the joystickY value is below the threshold, apply a threshold correction
        if (joystickY < threshold) joystickY = threshold;

        // drive 
        outputRight = constrain(joystickY + joystickX, 0, 255);
        outputLeft = constrain(joystickY - joystickX, 0, 255);
      }
      // ------------------ fixed rotation ------------------ //

      // I am unsure whether the right joystick or the hat was not working properly
      // I will use the right joystick but if it is broken, change to the hat
	    
	    
      // ------------------ cannon -------------------------- //




    // ------------------- pole climbers ------------------- //
    }
}
