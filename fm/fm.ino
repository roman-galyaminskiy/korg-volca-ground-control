#include "usb.hpp"
#include "definitions.hpp"
#include "controller.hpp"

Controller launchkey_mini;

void setup() {
  // SERIAL_MONITOR.begin(9600);
  MIDI_SERIAL_PORT_1.begin(31250);

  // SERIAL_MONITOR.println("Checking USB...");
  if (Usb.Init() == -1) {
    while (1); //halt
  }//if (Usb.Init() == -1...
  delay( 200 );

  // SERIAL_MONITOR.println("SUCCESS");
}

void loop() {
  static char flag;
  if (launchkey_mini.getExtenderModeStatus() == 0) {
    Usb.Task();
    if ( Usb.getUsbTaskState() == USB_STATE_RUNNING ) {
      launchkey_mini.enableExtendedMode();
      // SERIAL_MONITOR.println("Controller is ready to use");
      flag = 0;
    }
  }
  else {
    if (flag == 0) {
      launchkey_mini.mapper.drawMainScreen();
      flag = 1;
    }
    launchkey_mini.controller_listen();
  }
}
