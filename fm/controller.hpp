#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "usb.hpp"
#include "definitions.hpp"
#include "mapper.hpp"

class Controller {
public:
  Mapper mapper;

  char getExtenderModeStatus() { return extended_mode; }

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

  void listen() {
      uint8_t msg[3];
      char size;

      char pad_index = -1;
      static char knob_index = -1;
      static char knob_value = 0;
      static char non_empty_message_flg = 0;

      Usb.Task();
      if ( (size = Midi.RecvData(msg)) > 0 ) {
        // SERIAL_MONITOR.print(msg[0], DEC);
        // SERIAL_MONITOR.print(" ");
        // SERIAL_MONITOR.print(msg[1], DEC);
        // SERIAL_MONITOR.print(" ");
        // SERIAL_MONITOR.print(msg[2], DEC);
        // SERIAL_MONITOR.println();

        if (msg[0] == CHANNEL1_NOTE_ON) {
          if (msg[1] >= PAD1 and msg[1] <= PAD8 or msg[1] >= PAD9 and msg[1] <= PAD16
            or msg[1] == TOP_PLAY or msg[1] == BOTTOM_PLAY) {

            // Identify pad
            if (msg[1] >= PAD1 and msg[1] <= PAD8) {
                pad_index = msg[1] - PAD1;
            }
            else if (msg[1] >= PAD9 and msg[1] <= PAD16){
                pad_index = 8 + msg[1] - PAD9;
            }
            else {
                pad_index = -1;
            }

            if (pad_index > -1) {
              mapper.padPressed(pad_index);
            }

            // Identify buttons
            if (msg[1] == TOP_PLAY) {}

            else if (msg[1] == BOTTOM_PLAY) {}
          }
          // Keyboard
          else {
            MIDI_SERIAL_PORT_1.write(msg, size);
          }
        }
        else if (msg[0] == CHANNEL1_NOTE_OFF) {
          if (msg[1] >= PAD1 and msg[1] <= PAD8 or msg[1] >= PAD9 and msg[1] <= PAD16
            or msg[1] == TOP_PLAY or msg[1] == BOTTOM_PLAY) {
            knob_index = -1;
            knob_value = 0;
            non_empty_message_flg = 1;

            // Identify pad
            if (msg[1] >= PAD1 and msg[1] <= PAD8) {
                pad_index = msg[1] - PAD1;
            }
            else if (msg[1] >= PAD9 and msg[1] <= PAD16){
                pad_index = 8 + msg[1] - PAD9;
            }
            else {
                pad_index = -1;
            }

            if (pad_index > -1) {}

            // Identify buttons
            if (msg[1] == TOP_PLAY) {}

            else if (msg[1] == BOTTOM_PLAY) {}
          }
          // Keyboard
          else {
            MIDI_SERIAL_PORT_1.write(msg, size);
          }
        }
        else if (msg[0] == CHANNEL1_CONTROL_CHANGE) {
          non_empty_message_flg = 1;
          // Knobs
          if (msg[1] >= KNOB1 and msg[1] <= KNOB8) {
            knob_index= msg[1] - KNOB1 ;
            knob_value = msg[2];
          }
          // Other buttons
          else if (msg[1] == TRACK_LEFT and msg[2] == 127) {
            mapper.trackChanged(-1);
          }
          else if (msg[1] == TRACK_RIGHT and msg[2] == 127) {
            mapper.trackChanged(1);
          }
          else if (msg[1] = SCENE_UP and msg[2] == 127) {}
          else if (msg[1] = SCENE_DOWN and msg[2] == 127) {}
        }
      }
      else {
        if (non_empty_message_flg == 1) {
          if (knob_index > -1 and knob_value) {
              mapper.knobRotated(knob_index, knob_value);
            }
          }
          non_empty_message_flg = 0;
        }
      delay(1);
    }

private:
  // (aka 'InControl') Allows customizing pads behaviour
  char extended_mode = 0;
};

#endif // CONTROLLER_HPP
