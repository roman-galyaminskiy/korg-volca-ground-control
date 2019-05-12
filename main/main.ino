#include "usb.hpp"
#include "definitions.hpp"
#include "controller.hpp"

Controller controller;

void setup() {
  SERIAL_MONITOR.begin(9600);
  MIDI_SERIAL_PORT_1.begin(31250);
  MIDI_SERIAL_PORT_2.begin(31250);
  MIDI_SERIAL_PORT_3.begin(31250);

  SERIAL_MONITOR.println("Checking USB...");
  if (Usb.Init() == -1) {
    while (1); //halt
  }//if (Usb.Init() == -1...
  delay( 200 );

  SERIAL_MONITOR.println("SUCCESS");
}

void loop() {
  static char screen_initialized;
  // Enable 'inContorol' mode in order to light up pads
  if (controller.getExtenderModeStatus() == 0) {
    Usb.Task();
    if ( Usb.getUsbTaskState() == USB_STATE_RUNNING ) {
      controller.enableExtendedMode();
      SERIAL_MONITOR.println("Controller is ready to use");
      screen_initialized = 0;
    }
  }
  else {
    if (screen_initialized == 0) {
      controller.initializeScreen();
      screen_initialized = 1;
    }
    controller.listen();
  }
}
