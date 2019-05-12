#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "usb.hpp"

#define CHANNEL1_NOTE_ON 144

// Reads events from Novation Lauchkey Mini contoller like pad or key press,
// button press or knob pootion changing and then passes parsed data to the mapper
// associated with currently binded synth.
class Controller {
public:
  // Enables 'inControl' mode (extended mode according to the official documentation:
  // https://customer.novationmusic.com/sites/customer/files/novation/downloads/10535/launchkey-mk2-programmers-reference-guide.pdf
  // This modes allows you to customize pads lighting.
  void enableExtendedMode() {
    uint8_t msg[3] = {CHANNEL1_NOTE_ON, 12, 0};

    // TODO: Status check
    // uint8_t msg[3] = {CHANNEL1_NOTE_ON, 11, 0};
    // Midi.SendData(msg, 1);
    // delay(1);
    // MIDI_poll();

    // Off
    Usb.Task();
    Midi.SendData(msg, 1);
    delay(1);

    // On
    msg[2] = 127;
    Usb.Task();
    Midi.SendData(msg, 1);
    delay(1);

    extended_mode = 1;
    // SERIAL_MONITOR.println("Extended mode enabled");
    }

  // Returns 1 if 'inControl' mode is enabled and 0 otherwise
  char getExtenderModeStatus() { return extended_mode; }

private:
  // (aka 'InControl') Allows customizing pads behaviour
  char extended_mode = 0;
};

#endif // CONTROLLER_HPP
