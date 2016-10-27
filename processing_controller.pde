// DriverStation.pde
//
// This example uses two community supported libraries - gamecontrolplus and vsync
// you need to install both of these libraries before running this example.
//
// gamecontrolplus handles the interface with any standard USB controller
// vsync is a great library for passing data between processing and arduino
//
// Written by: B. Huang, Sparkfun Electronics -- 
// rev. September 12, 2014
//
// Modified by Erik-Olivier Riendeau, Mcgill EngGames, 2016 .
//

import processing.serial.*;
import org.gamecontrolplus.*;
import vsync.*;



/***************************************************************************************
 // vsync objects
 // Create a sender and a receiver for incoming and outgoing synchronization.
 ****************************************************************************************/
ValueReceiver receiver;
ValueSender sender;

// vsync public variables for passing to Arduino
public int leftX;
public int leftY;
public int rightX;
public int rightY;
public int hatPosition;

public int b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12;

// variables for receiving debug data back from Arduino
public int debugVar0, debugVar1, debugVar2, debugVar3;

/***************************************************************************************
 // gamecontrolplus objects 
 // Objects for managing and hangling interface to the joystick controller
 ****************************************************************************************/
ControlIO control;
ControlDevice device;

ControlButton[] button; // Array of buttons - one for each button control on joystick
ControlHat hat;         // The 'hat' is the up/down/left/right control on joystick

Stick rightStick;       // We are using a custom Stick class which combines two ControlSliders for each analog "stick"
Stick leftStick;        // each ControlSlider returns a single axis. In the constructor for the Stick object,
// define the X and Y ControlSlider.
int controllerID; 
String controllerName;

int numDevices;         // number of 'gamecontrol' devices connected. This includes any I/O including mouse, keyboard, touchscreen...
int numButtons;         // number of buttons on joystick
int numRumblers;        // number of rumblers on joystick
int numSliders;         // number of 'sliders' or analog joystick axes

String[] hatPositionNames = {
  "Center", "Northwest", "North", "Northeast", "East", "Southeast", "South", "Southwest", "West"
};
/********************************************************
 / Misc variables for displaying text to graphics window.
/********************************************************/
int xPos = 10;
int yPos = 20;
int txtLineSpace = 20;

public class Stick {
  float x;
  float y;
  public ControlSlider sliderX; 
  public ControlSlider sliderY; 

  public Stick(ControlDevice controldevice, int sliderXnum, int sliderYnum) {
    sliderX = controldevice.getSlider(sliderXnum);
    sliderY = controldevice.getSlider(sliderYnum);
  }

  public float getX() {
    return sliderX.getValue();
  };

  public float getY() {
    return sliderY.getValue();
  };
}
/********************************************************
 / setup function 
/********************************************************/
public void setup() {
  size(800, 600);  // Define the window size.

  // List out and setup the Serial com port interface
  print("Serial Com Ports Available: ");
  println(Serial.list());
  println();




  // normal serial communication ( wired)
  Serial serial = new Serial(this, "/dev/ttyACM1", 19200); // VERY IMPORTANT TO CHANGE THIS PORT TO THE CORRESPONDING PORT AND BAUD RATE USED
	// IN YOUR ARDUINO CODE. ALSO, WITH UBUNTU THE PORT MIGHT CHANGE RESULTING IN ERRORS. MAKE SURE THIS LINE IS UPDATED ALL THE TIME. 
	// FINALLY, THIS WILL LOOK SOMETHING LIKE "/DEV/COM6" IN WINDOWS

  // Setup the gamecontrol device object
  control = ControlIO.getInstance(this);      // instantiated the controlIO object
  numDevices = control.getNumberOfDevices();  // find the number of connected controllers

  // Search for the "Joystick" controller on USB
  for (int x = 0; x < numDevices; x++) {
    println("[" + x + "] " + control.getDevice(x).getName() + " - Type: " + control.getDevice(x).getTypeName());
    if (control.getDevice(x).getTypeName() == "Stick")  // Tested using the Logitech GamePad F310
      controllerID = x;                                 // It enumerates as a type "Stick"
  }
  println();

  device = control.getDevice(controllerID);   // device object points to the joystick controller
  controllerName = device.getName();  

  numButtons = device.getNumberOfButtons();
  numSliders = device.getNumberOfSliders();
  numRumblers = device.getNumberOfRumblers();



  // define the right and left paddles on the controller.
  // Stick class combines both the up/down and left/right sliders into one object
  rightStick = new Stick(device, 1, 0); // Analog joysticks
  leftStick = new Stick(device, 3, 2);  // Analog joysticks

  button = new ControlButton[numButtons]; // creates an array of button objects for all buttons on device.
  for (int x = 0; x < numButtons; x++) {
    button[x] = device.getButton(x);
  }
  // setup for vSync control. Receiver object takes in data from Arduino. Sender object sends data to Arduino
  // the number of objects and the order of these must match between Processing and Arduino.
  receiver = new ValueReceiver(this, serial).observe("debugVar0").observe("debugVar1").observe("debugVar2").observe("debugVar3");
  sender = new ValueSender(this, serial).observe("hatPosition").observe("leftX").observe("leftY").observe("rightX").observe("rightY").observe("b0").observe("b1").observe("b2").observe("b3").observe("b4").observe("b5").observe("b6").observe("b7").observe("b8").observe("b9").observe("b10").observe("b11").observe("b12");
}

/********************************************************
 / draw function 
/********************************************************/

public void draw() {
  xPos = 20;  // starting text position
  yPos = 20;   
  refreshLocalVariables();  // fetches the state of the button presses and joystick to local Variables -- used with vSync Library
  background(255);
  fill(255, 0, 0); // red for LEFT stick (slider)

  ellipse(map(leftStick.getX(), -1, 1, 20, 780) - 20, map(leftStick.getY(), -1, 1, 0, 600), 20, 20);

  fill(0, 0, 255); // blue for RIGHT stick (slider)
  ellipse(map(rightStick.getX(), -1, 1, 20, 780) + 20, map(rightStick.getY(), -1, 1, 0, 600), 20, 20);

  text("Joystick Controller: " + device.getName(), xPos, yPos);
  yPos += txtLineSpace;
  text("NumButtons: " + numButtons, xPos, yPos);
  yPos += txtLineSpace;
  text("NumSliders: " + numSliders, xPos, yPos);
  yPos += txtLineSpace;
  text("NumRumblers: " + numRumblers, xPos, yPos);
  yPos += 2*txtLineSpace; // double-space

  text("*********Dashboard*********", xPos, yPos);
  yPos += txtLineSpace;
  text("Left Analog Stick: (" + leftX + ", " + leftY + ")", xPos, yPos);
  yPos += txtLineSpace;
  text("Right Analog Stick: (" + rightX + ", " + rightY + ")", xPos, yPos);
  yPos += txtLineSpace;

  for (int x = 0; x < numButtons; x++) {
    text("Button [" + x + "]: " + (int)(button[x].pressed() ? 1 : 0), xPos, yPos);
    yPos += txtLineSpace;
  }

  text("Hat Position: " + hatPositionNames[hatPosition], xPos, yPos);
  yPos += 20;
}

void refreshLocalVariables()
{
  // type casts the boolean .pressed() to an integer type.
  b0 = button[0].pressed() ? 1 : 0;
  b1 = button[1].pressed() ? 1 : 0; 
  b2 = button[2].pressed() ? 1 : 0; 
  b3 = button[3].pressed() ? 1 : 0; 
  b4 = button[4].pressed() ? 1 : 0; 
  b5 = button[5].pressed() ? 1 : 0; 
  b6 = button[6].pressed() ? 1 : 0; 
  b7 = button[7].pressed() ? 1 : 0; 
  b8 = button[8].pressed() ? 1 : 0; 
  b9 = button[9].pressed() ? 1 : 0; 
  b10 = button[10].pressed() ? 1 : 0; 
  b11 = button[11].pressed() ? 1 : 0; 
  b12 = button[12].pressed() ? 1 : 0;

  // scales the -1 to +1 state of the slider to -255 to +255
  leftX = (int) map(leftStick.getX(), -1, 1, -255, 255);
  leftY = (int) map(leftStick.getY(), -1, 1, -255, 255);

  rightX = (int) map(rightStick.getX(), -1, 1, -255, 255); 
  rightY = (int) map(rightStick.getY(), -1, 1, -255, 255);

  hatPosition = hat.getPos();
}
