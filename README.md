# McGill-EngGames-Machine-2016


In order to have the following two codes work, you need to get the processing sketch made by B.Huang. I slightly
modified it in order to use with the Serial communication instead of XBee, I also plan to add the keyboard as input for
the robot.
Just run the processing code in processing with your Logitech controller f310 connected to your p.c. The code automatically 
detects the port in which your f310 is connected. You have to change the port in which the arduino is connected as well as its
baud rate to reflect the same values as in your arduino code. Always reset this value in Unix, because the arduino ports might
differ everytime you connect your arduino. 
You can download Arduino here: https://www.arduino.cc/en/Main/Software
And processing here: https://processing.org/download/
The controller used is the following : http://gaming.logitech.com/en-ca/product/f310-gamepad


->Communication Codes between a controller and an arduino mega on the actual robot

 * Erik-Olivier Riendeau, McGill Jeux du Genie, 2016
 * 
 * Communication system part 1 THIS IS THE SENDER
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
 
 // ---------------------------------------------------------------------------- //
 
 * Erik-Olivier Riendeau, McGill Jeux du Genie, 2016
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
