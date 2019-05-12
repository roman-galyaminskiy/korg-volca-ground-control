#ifndef BEATS_HPP
#define BEATS_HPP

#define KICK 36
#define SNARE 38
#define LOW_TOM 43
#define HIGH_TOM 50
#define CLOSED_HAT 42
#define OPEN_HAT 46
#define CLAP 39
#define CLAVES 75
#define AGOGO 67
#define CRASH 49

#define KICK_LEVEL 40
#define SNARE_LEVEL 41
#define LOW_TOM_LEVEL 42
#define HIGH_TOM_LEVEL 43
#define CLOSED_HAT_LEVEL 44
#define OPEN_HAT_LEVEL 45
#define CLAP_LEVEL 46
#define CLAVES_LEVEL 47
#define AGOGO_LEVEL 48
#define CRASH_LEVEL 49

#define CLAP_SPEED 50
#define CLAVES_SPEED 51
#define AGOGO_SPEED 52
#define CRASH_SPEED 53

// Reads parsed data from cntroller to light pads and edit synth parameters by
// sending short MIDI messages.
// Use could find Korg Volca Beats implementation here: https://www.korg.com/us/support/download/manual/0/141/1961/
//
// Pads are used to trigger sounds. There are several options for edittting
// sample attached to pad:
// * Knob mode
// * Mapper mode
// Modes are described precisely bellow.
//
// You could use only mode at the time.
class VolcaBeats {
public:
  //###########################################################################
  // Draw functions
  //###########################################################################

  void drawActivePads() {
    // 'note_on' message templape
    uint8_t msg[3] = {CHANNEL1_NOTE_ON, 0, 0};

    for (char i = 0; i < 16; i++) {
      msg[1] = pads[i].note;
      if (pads[i].knob_mode == PAD_KNOB_MODE_ON) {
        if (control_change_flg == 1) {
          msg[2] = BLACK;
        }
        else {
          msg[2] = voices[pads[i].mapping].color;
        }
      }
      else {
        msg[2] = voices[pads[i].mapping].color;
      }

      Usb.Task();
      Midi.SendData(msg, 1);
      delay(1);
    }
  }

  void drawVoiceSelect() {
    // 'note_on' message templape
    uint8_t msg[3] = {CHANNEL1_NOTE_ON, 0, 0};

    char current_mapping = pads[remap_mode].mapping;

    for (char i = 0; i < 16; i++) {
      msg[1] = pads[i].note;
      if (
        (pads[i].note >= PAD1 and pads[i].note <= PAD8 or pads[i].note >= PAD9 and pads[i].note <= PAD10)
        and i != current_mapping
      ) {
        msg[2] = GREEN;
      }
      else if (i == current_mapping) {
        msg[2] = YELLOW;
      }
      else {
        msg[2] = BLACK;
      }
      Usb.Task();
      Midi.SendData(msg, 1);
      delay(1);
    }
  }

  void drawPlayButtons() {
    // 'note_on' message templape
    uint8_t top_play_msg[3] = {CHANNEL1_NOTE_ON, TOP_PLAY, 0};
    uint8_t bottom_play_msg[3] = {CHANNEL1_NOTE_ON, BOTTOM_PLAY, 0};

    if (knob_mode > CONTROLLER_KNOB_MODE_OFF) {
      top_play_msg[2] = RED;
    }

    if (remap_mode > CONTROLLER_REMAP_MODE_OFF) {
      top_play_msg[2] = RED;
      bottom_play_msg[2] = RED;
    }

    if (remap_mode == CONTROLLER_REMAP_MODE_OFF and knob_mode == CONTROLLER_KNOB_MODE_OFF) {
      top_play_msg[2] = BLACK;
    }

    if (remap_mode == CONTROLLER_REMAP_MODE_OFF) {
      bottom_play_msg[2] = BLACK;
    }

    Usb.Task();
    Midi.SendData(top_play_msg, 1);
    delay(1);
    Usb.Task();
    Midi.SendData(bottom_play_msg, 1);
    delay(1);
  }

  void drawMainScreen() {
    if (remap_mode > CONTROLLER_REMAP_MODE_SELECT_PAD) {
      drawVoiceSelect();
    }
    else {
      drawActivePads();
    }

    drawPlayButtons();
  }

  //###########################################################################
  // Controller event handling
  //##########################################################################

  void keyPressed(uint8_t key_note) {}

  void padPressed(char pad_index) {
    uint8_t msg[3] = {CHANNEL1_NOTE_ON, 0, 127};

    if (remap_mode == CONTROLLER_REMAP_MODE_SELECT_PAD) {
      remap_mode = pad_index;
    }
    else if (remap_mode > CONTROLLER_REMAP_MODE_SELECT_PAD) {
      mapPadToVoice(pad_index);
    }
    else if (top_play_button_pressed == 1) {
      enableKnobMode(pad_index);
    }
    else {
      msg[1] += voices[pads[pad_index].mapping].note;
      MIDI_SERIAL_PORT_3.write(msg, 3);
    }
    drawMainScreen();
  }

  void knobRotated(char knob_index, char knob_value) {
    uint8_t msg[3] = {CHANNEL1_CONTROL_CHANGE, 0, knob_value};

    if (knob_mode > CONTROLLER_KNOB_MODE_OFF) {
      if (knob_index == 0) {
        msg[1] = voices[pads[knob_mode].mapping].level_contol_number;
        MIDI_SERIAL_PORT_3.write(msg, 3);
        // Flickering to indicate with pad are we editting
        control_change_flg = 1;
        drawMainScreen();
        delay(10);
        control_change_flg = 0;
        drawMainScreen();
        delay(10);
      }
      else if (knob_index == 1 and voices[pads[knob_mode].mapping].speed_contol_number > -1) {
        msg[1] = voices[pads[knob_mode].mapping].speed_contol_number;
        MIDI_SERIAL_PORT_3.write(msg, 3);
        // Flickering to indicate with pad are we editting
        control_change_flg = 1;
        drawMainScreen();
        delay(10);
        control_change_flg = 0;
        drawMainScreen();
        delay(10);
      }
    }
  }

  void topPlayButtonPressed() {
    // Primary mode attached to the button
    if (knob_mode > CONTROLLER_KNOB_MODE_OFF) {
      disableKnobMode();
    }
    else {
      top_play_button_pressed = 1;
    }
    drawMainScreen();
  }

  void bottomPlayButtonPressed() {
    // Play buttons combinations handling
    if (top_play_button_pressed == 1) {
      if (remap_mode == CONTROLLER_REMAP_MODE_OFF) {
        remap_mode = CONTROLLER_REMAP_MODE_SELECT_PAD;
      }
      else {
        remap_mode = CONTROLLER_REMAP_MODE_OFF;
      }
      top_play_button_pressed = 0;
      bottom_play_button_pressed = 0;
    }
    else {
      bottom_play_button_pressed = 1;
    }
    drawMainScreen();
  }

  void topPlayButtonReleased() {
    top_play_button_pressed = 0;
  }

  void bottomPlayButtonReleased() {
    bottom_play_button_pressed = 0;
  }

  void disableKnobMode() {
    for (char i = 0; i < 16; i++) {
      pads[i].knob_mode = PAD_KNOB_MODE_OFF;
    }
    knob_mode = CONTROLLER_KNOB_MODE_OFF;
  }

  void enableKnobMode(char pad_index) {
    if (knob_mode == CONTROLLER_KNOB_MODE_OFF) {
      remap_mode = CONTROLLER_REMAP_MODE_OFF;
      knob_mode = pad_index;
      pads[pad_index].knob_mode = PAD_KNOB_MODE_ON;
    }
  }

  void mapPadToVoice(char voice_index) {
    if (voice_index <= 9) {
      pads[remap_mode].mapping = voice_index;
    }
    remap_mode = CONTROLLER_REMAP_MODE_OFF;
  }

private:
  struct Voice {
    Voice(const char n, const char l, const char s, const char c): note(n), color(c),
      level_contol_number(l), speed_contol_number(s) {}
    Voice(const char n, const char l, const char c): note(n), color(c),
      level_contol_number(l), speed_contol_number(-1) {}

    const char note;
    const char color;
    const char level_contol_number;
    const char speed_contol_number;

    char key_velocity_sensitivity = 0;
  };

  struct Pad {
    Pad(const char a, char v): note(a), mapping(v) {}

    const char note;
    char mapping;

    char knob_mode = PAD_KNOB_MODE_OFF;
  };

  const uint8_t pad_notes[16] = {
    96, 97, 98, 99, 100, 101, 102, 103,
    112, 113, 114, 115, 116, 117, 118, 119
  };

  Voice voices[10] = {
    {KICK, KICK_LEVEL, ORANGE}, {SNARE, SNARE_LEVEL, RED},
    {LOW_TOM, LOW_TOM_LEVEL, ORANGE}, {HIGH_TOM, HIGH_TOM_LEVEL, ORANGE},
    {CLOSED_HAT, CLOSED_HAT_LEVEL, YELLOW}, {OPEN_HAT, OPEN_HAT_LEVEL, YELLOW},
    {CLAP, CLAP_LEVEL, CLAP_SPEED, GREEN}, {CLAVES, CLAVES_LEVEL, CLAVES_SPEED, GREEN},
    {AGOGO, AGOGO_LEVEL, AGOGO_SPEED, GREEN}, {CRASH, CRASH_LEVEL, CRASH_SPEED, GREEN}
  };

  Pad pads[16] = {
    {PAD1, 9}, {PAD2, 6}, {PAD3, 1}, {PAD4, 5}, {PAD5, 5}, {PAD6, 1}, {PAD7, 6}, {PAD8, 9},
    {PAD9, 2}, {PAD10, 3}, {PAD11, 0}, {PAD12, 4}, {PAD13, 4}, {PAD14, 0}, {PAD15, 3}, {PAD16, 2}
  };

  // You could change voice mapped to the selected pad
  // 1. To enter press top and bottom 'play' buttons consequently, both play
  // buttons will light up. Press buttons again in same order to exit mode.
  //  * remap_mode = -1
  // 2. Tap a pad that you want to change a mapping for
  //  * remap_mode = pad_index
  // 3. Tap pads 1-10 to select a voice or any other to disable pad. Yellow
  // pad represents currently mapped voice
  //  * remap_mode = -2
  char remap_mode = CONTROLLER_REMAP_MODE_OFF;

  // Allows editing selected voice parameter using two knobs. Knobs one contols
  // voices level and knob two controls speed (available only for PCM sounds).
  // Other knobs don't change synth parameter. When this mode is off
  // controller knobs are not responsive.
  // 1. To enter press top 'play' bottom and one of the pads pad successively
  //   * keyboard_mode = pad_index
  //   * Selected pad will flicker on control change
  // 2. To exit press bottom 'play' bottom one more time
  //   * keyboard_mode = -1
  char knob_mode = CONTROLLER_KNOB_MODE_OFF;

  // Auxiliary flags
  char top_play_button_pressed = 0;
  char bottom_play_button_pressed = 0;
  char control_change_flg = 0;
};

#endif // BEATS_HPP
