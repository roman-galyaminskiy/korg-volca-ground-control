#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "usb.hpp"
#include "definitions.hpp"
#include "fm.hpp"
#include "sample.hpp"
#include "beats.hpp"

#define FM 0
#define SAMPLE 1
#define BEATS 2

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

  // Draw main screen for currently selected synth
  void initializeScreen() {
    if (selected_synth == FM) {
      fm.drawMainScreen();
    }
    else if (selected_synth == SAMPLE) {
      sample.drawMainScreen();
    }
    else if (selected_synth == BEATS) {
      beats.drawMainScreen();
    }
  }

  // Reads data from USB contoller and feeds data to the mapper
  // associated with currently binded synth.
	void listen() {
    uint8_t msg[3];
    char size;

    char pad_index = -1;
    static char knob_index = -1;
    static char knob_value = 0;
    static char non_empty_message_flg = 0;

    Usb.Task();
    if ( (size = Midi.RecvData(msg)) > 0 ) {
      SERIAL_MONITOR.print(msg[0], DEC);
      SERIAL_MONITOR.print(" ");
      SERIAL_MONITOR.print(msg[1], DEC);
      SERIAL_MONITOR.print(" ");
      SERIAL_MONITOR.print(msg[2], DEC);
      SERIAL_MONITOR.println();

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
            if (selected_synth == FM) {
              fm.padPressed(pad_index);
            }
            else if (selected_synth == SAMPLE) {
              sample.padPressed(pad_index);
            }
            else if (selected_synth == BEATS) {
              beats.padPressed(pad_index);
            }
          }

          // Identify buttons
          if (msg[1] == TOP_PLAY) {
            if (selected_synth == FM) {
              // fm.topPlayButtonPressed();
            }
            else if (selected_synth == SAMPLE) {
              sample.topPlayButtonPressed();
            }
            else if (selected_synth == BEATS) {
              beats.topPlayButtonPressed();
            }
          }

          else if (msg[1] == BOTTOM_PLAY) {
            if (selected_synth == FM) {
              // fm.bottomPlayButtonPressed();
            }
            else if (selected_synth == SAMPLE) {
              sample.bottomPlayButtonPressed();
            }
            else if (selected_synth == BEATS) {
              beats.bottomPlayButtonPressed();
            }
          }
        }
        // Keyboard
        else {
          if (selected_synth == FM) {
            fm.keyPressed(msg[1]);
          }
          else if (selected_synth == SAMPLE) {
            sample.keyPressed(msg[1]);
          }
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
          if (msg[1] == TOP_PLAY) {
            if (selected_synth == FM) {
              // fm.topPlayButtonReleased();
            }
            else if (selected_synth == SAMPLE) {
              sample.topPlayButtonReleased();
            }
            else if (selected_synth == BEATS) {
              beats.topPlayButtonReleased();
            }
          }

          else if (msg[1] == BOTTOM_PLAY) {
            if (selected_synth == FM) {
              // fm.bottomPlayButtonReleased();
            }
            else if (selected_synth == SAMPLE) {
              sample.bottomPlayButtonReleased();
            }
            else if (selected_synth == BEATS) {
              beats.bottomPlayButtonReleased();
            }
          }
        }
        // Keyboard
        else {
          if (selected_synth == FM) {
            fm.keyReleased(msg[1]);
            MIDI_SERIAL_PORT_1.write(msg, 3);
          }
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
          if (selected_synth > 0) {
            selected_synth--;

            if (selected_synth == FM) {
              fm.drawMainScreen();
            }
            else if (selected_synth == SAMPLE) {
              sample.drawMainScreen();
            }
            else if (selected_synth == BEATS) {
              beats.drawMainScreen();
            }
          }
        }
        else if (msg[1] == TRACK_RIGHT and msg[2] == 127) {
          if (selected_synth < 2) {
            selected_synth++;

            if (selected_synth == FM) {
              fm.drawMainScreen();
            }
            else if (selected_synth == SAMPLE) {
              sample.drawMainScreen();
            }
            else if (selected_synth == BEATS) {
              beats.drawMainScreen();
            }
          }
        }
        else if (msg[1] = SCENE_UP and msg[2] == 127) {}
        else if (msg[1] = SCENE_DOWN and msg[2] == 127) {}
      }
    }
    else {
      if (non_empty_message_flg == 1) {
        if (knob_index > -1 and knob_value) {
          if (selected_synth == FM) {
            fm.knobRotated(knob_index, knob_value);;
          }
          else if (selected_synth == SAMPLE) {
            sample.knobRotated(knob_index, knob_value);;
          }
          else if (selected_synth == BEATS) {
            beats.knobRotated(knob_index, knob_value);;
          }
        }
        non_empty_message_flg = 0;
      }
    delay(1);
  }
}

private:
  // (aka 'InControl') Allows customizing pads behaviour
  char extended_mode = 0;

  char selected_synth = FM;

  VolcaFM fm;
  VolcaSample sample;
  VolcaBeats beats;
};

#endif // CONTROLLER_HPP
