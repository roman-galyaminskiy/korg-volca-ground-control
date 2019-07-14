#include <usbh_midi.h>
#include <usbhub.h>
#include <SPI.h>

#define SERIAL_MONITOR Serial
#define MIDI_SERIAL_PORT_1 Serial2

#define PAD1 96
#define PAD2 97
#define PAD3 98
#define PAD4 99
#define PAD5 100
#define PAD6 101
#define PAD7 102
#define PAD8 103
#define PAD9 112
#define PAD10 113
#define PAD11 114
#define PAD12 115
#define PAD13 116
#define PAD14 117
#define PAD15 118
#define PAD16 119

#define TOP_PLAY 104
#define BOTTOM_PLAY 120
#define TRACK_LEFT 106
#define TRACK_RIGHT 107
#define SCENE_UP 104
#define SCENE_DOWN 105

#define KNOB1 21
#define KNOB2 22
#define KNOB3 23
#define KNOB4 24
#define KNOB5 25
#define KNOB6 26
#define KNOB7 27
#define KNOB8 28

#define BLACK 0
#define RED 15
#define ORANGE 19
#define YELLOW 21
#define GREEN 24

#define CHANNEL1_NOTE_ON 144
#define CHANNEL1_NOTE_OFF 128
#define CHANNEL1_CONTROL_CHANGE 176

// #############################################################################

#define LEVEL 7
#define PAN 10
#define SAMPLE_START_POINT 40
#define SAMPLE_LENGTH 41
#define HI_CUT 42
#define SPEED 43
#define PITCH_INT 44
#define PITCH_ATTACK 45
#define PITCH_DECAY 46
#define AMPLITUDE_ATTACK 47
#define AMPLITUDE_DECAY 48

// #############################################################################

#define CONTROLLER_REMAP_MODE_OFF -2
#define CONTROLLER_REMAP_MODE_SELECT_PAD -1

#define CONTROLLER_KEYBOARD_MODE_OFF -1
#define PAD_KEYBOARD_MODE_OFF 0
#define PAD_KEYBOARD_MODE_ON 1

#define CONTROLLER_KNOB_MODE_OFF -1
#define PAD_KNOB_MODE_OFF 0
#define PAD_KNOB_MODE_ON 1

USB Usb;
USBH_MIDI Midi(&Usb);

class Mediator;

class Controller {
public:
  Controller(Mediator* _mediator) : mediator(_mediator) {}

  // Enables 'inControl' mode (extended mode according to the official documentation:
  // https://customer.novationmusic.com/sites/customer/files/novation/downloads/10535/launchkey-mk2-programmers-reference-guide.pdf
  // This modes allows you to customize pads lighting.
  void enable_extended_mode();

  // Returns 1 if 'inControl' mode is enabled and 0 otherwise
  char get_extended_mode_status();

  // Reads data from USB controller and feeds data to the mapper
  // associated with currently binded synth.
  void listen();

  void change_pad_color(unsigned char, unsigned char);

private:
  Mediator* mediator;

  // (aka 'InControl') Allows customizing pads behaviour
  char extended_mode = 0;

  const unsigned char pad_notes[16] = {
    PAD1, PAD2, PAD3, PAD4, PAD5, PAD6, PAD7, PAD8,
    PAD9, PAD10, PAD11, PAD12, PAD13, PAD14, PAD15, PAD16
  };
};

// #############################################################################

class VolcaSample {
public:
  // Default constructor
  VolcaSample(Mediator* m) : mediator(m) {}

  void change_speed(char, char);
  void change_level(char, char);
  void imitate_velocity(char, char);
  void note_on(char, char, char);

private:
    char level = 127;
    char velocity_sensitivity;
    Mediator* mediator;
};

// #############################################################################

class Mediator {
public:
    Controller* controller = new Controller(this);
    VolcaSample* volca_sample = new VolcaSample(this);

   void padPressed(char);

   void topPlayButtonPressed();

   void bottomPlayButtonPressed();

   void keyPressed(unsigned char);

   void topPlayButtonReleased();

   void bottomPlayButtonReleased();

   void keyReleased(unsigned char);

   void knobRotated(char, char);

   void drawMainScreen();

private:
  struct Pad {
    Pad(const char a, char m): index(a), mapping(m), color(BLACK) {}

    const char index;
    char mapping;
    char color;

    char keyboard_mode = PAD_KEYBOARD_MODE_OFF;
    char knob_mode = PAD_KNOB_MODE_OFF;
  };

  const uint8_t knob_mapping[8] = {LEVEL, SPEED, SAMPLE_START_POINT, SAMPLE_LENGTH,
      PITCH_ATTACK, PITCH_DECAY, AMPLITUDE_ATTACK, AMPLITUDE_DECAY};

  Pad pads[16] = {
    {0, 7}, {1, 6}, {2, 1}, {3, 5}, {4, 5}, {5, 8}, {6, 6}, {7, 7},
    {8, 2}, {9, 3}, {10, 0}, {11, 4}, {12, 4}, {13, 0}, {14, 3}, {15, 2}
  };
};

// #############################################################################

void Controller::enable_extended_mode() {
  unsigned char msg[3] = {CHANNEL1_NOTE_ON, 12, 0};

  // TODO: Status check
  // unsigned char msg[3] = {CHANNEL1_NOTE_ON, 11, 0};
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

char Controller::get_extended_mode_status() { return extended_mode; }

void Controller::listen() {
  unsigned char msg[3];
  char size;

  char pad_index = -1;
  static char knob_index = -1;
  static char knob_value = 0;
  static char non_empty_message_flg = 0;

  if (Usb.Init() == -1) {
    while (1); //halt
  }//if (Usb.Init() == -1...
  delay(1);

  // Enable 'inContorol' mode in order to light up pads
  if (Controller::get_extended_mode_status() == 0) {
    // SERIAL_MONITOR.println("Checking controller status");
    Usb.Task();
    if ( Usb.getUsbTaskState() == USB_STATE_RUNNING ) {
      Controller::enable_extended_mode();
    }
  }
  else {
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
            mediator->padPressed(pad_index);
          }

          // Identify buttons
          if (msg[1] == TOP_PLAY) {
            mediator->topPlayButtonPressed();
          }

          else if (msg[1] == BOTTOM_PLAY) {
            mediator->bottomPlayButtonPressed();
          }
        }
        // Keyboard
        else {
          mediator->keyPressed(msg[1]);
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
            mediator->topPlayButtonReleased();
          }

          else if (msg[1] == BOTTOM_PLAY) {
            mediator->bottomPlayButtonReleased();
          }
        }
        // Keyboard
        else {
          mediator->keyReleased(msg[1]);
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
          // pass
        }
        else if (msg[1] == TRACK_RIGHT and msg[2] == 127) {
          // pass
        }
        else if (msg[1] = SCENE_UP and msg[2] == 127) {}
        else if (msg[1] = SCENE_DOWN and msg[2] == 127) {}
      }
    }
    else {
      if (non_empty_message_flg == 1) {
        if (knob_index > -1 and knob_value) {
          mediator->knobRotated(knob_index, knob_value);
        }
        non_empty_message_flg = 0;
      }
      delay(1);
    }
  }

}

void Controller::change_pad_color(unsigned char pad_index, unsigned char color_code) {
  unsigned char msg[3] = {CHANNEL1_NOTE_ON, pad_notes[pad_index], color_code};

  Usb.Task();
  Midi.SendData(msg, 1);
  delay(1);
}

// #############################################################################

void VolcaSample::change_speed(char channel, char semitones) {
  // Note message doesn't change pitch
  unsigned char sample_speed_message[3] = {CHANNEL1_CONTROL_CHANGE + channel, SPEED, 63 + semitones};
  MIDI_SERIAL_PORT_1.write(sample_speed_message, 3);
}

void VolcaSample::change_level(char channel, char level) {
  // Have to store
  VolcaSample::level = level;

  // Note message doesn't change pitch
  unsigned char sample_level_message[3] = {CHANNEL1_CONTROL_CHANGE + channel, LEVEL, VolcaSample::level};
  MIDI_SERIAL_PORT_1.write(sample_level_message, 3);
}

void VolcaSample::imitate_velocity(char channel, char velocity) {
  char level = VolcaSample::level * (1.0 - VolcaSample::velocity_sensitivity) + VolcaSample::level * VolcaSample::velocity_sensitivity * velocity / 127.0;

  unsigned char sample_level_message[3] = {CHANNEL1_CONTROL_CHANGE + channel, LEVEL, level};
  MIDI_SERIAL_PORT_1.write(sample_level_message, 3);
}

void VolcaSample::note_on(char channel, char semitones=-1, char velocity=-1) {
  // SERIAL_MONITOR.println("note_on");
  // Note message doesn't change pitch
  unsigned char note_on_message[3] = {CHANNEL1_NOTE_ON + channel, 0, 127};

  if (semitones > -1) {
    VolcaSample::change_speed(channel, semitones);
  }
  if (velocity > -1) {
    VolcaSample::imitate_velocity(channel, velocity);
  }
  
  MIDI_SERIAL_PORT_1.write(note_on_message, 3);
}

// #############################################################################

void Mediator::padPressed(char pad_index) {
  // SERIAL_MONITOR.print("padPressed ");
  // SERIAL_MONITOR.print(pad_index, DEC);
  // SERIAL_MONITOR.println();


  for (size_t pad = 0; pad < 16; pad++) {
    if (pad_index == pads[pad].index) {
      volca_sample->note_on(pads[pad].mapping);
    }
  }


}

void Mediator::topPlayButtonPressed() {}

void Mediator::bottomPlayButtonPressed() {}

void Mediator::keyPressed(unsigned char note) {
  Serial.println("keyPressed");
}

void Mediator::topPlayButtonReleased() {}

void Mediator::bottomPlayButtonReleased() {}

void Mediator::keyReleased(unsigned char note) {}

void Mediator::knobRotated(char knob_index, char knob_value) {}

void Mediator::drawMainScreen() {}

// #############################################################################

Mediator mediator;

void setup() {
  // SERIAL_MONITOR.begin(9600);
  MIDI_SERIAL_PORT_1.begin(31250);
}

void loop() {
  mediator.controller->listen();
}
