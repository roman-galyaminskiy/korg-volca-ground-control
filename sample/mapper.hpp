#ifndef MAPPER_HPP
#define MAPPER_HPP

#define LEVEL 7
#define ATTACK 47
#define DECAY 48
#define SPEED 43

#define CONTROLLER_REMAP_MODE_OFF -2
#define CONTROLLER_REMAP_MODE_SELECT_PAD -1

#define CONTROLLER_KEYBOARD_MODE_OFF -1
#define PAD_KEYBOARD_MODE_OFF 0
#define PAD_KEYBOARD_MODE_ON 1

#define CONTROLLER_KNOB_MODE_OFF -1
#define PAD_KNOB_MODE_OFF 0
#define PAD_KNOB_MODE_ON 1


class Mapper {
  struct Pad {
    Pad(const char a, char m): note(a), mapping(m), color(BLACK) {}

    const char note;
    char mapping;
    char color;

    char keyboard_mode = PAD_KEYBOARD_MODE_OFF;
    char knob_mode = PAD_KNOB_MODE_OFF;
  };

private:
  // Pads are used to trigger samples. There are several options for edittting
  // sample attached to pad:
  // * Knob mode
  // * Keyboard mode
  // * Mapper mode

  // You could use only mode at the time.

  // You could change voice mapped to the selected pad
  // 1. To enter press top and bottom 'play' buttons consequently, both play
  // buttons will light up. Press buttons again in same order to exit mode.
  //  * remap_mode = -1
  // 2. Tap a pad that you want to change a mapping for
  //  * remap_mode = pad_index
  // 3. Tap pads 1-10 to select a voice or any other to disable pad
  //  * remap_mode = -2
  char remap_mode = CONTROLLER_REMAP_MODE_OFF;

  // Allows playing selected sample chromatically using keyboard. When this mode is off
  // controller keyboard is not responsive.
  // 1. To enter press bottom 'play' bottom and one of the pads pad successively
  //   * Selected pad turn RED
  //   * knob_mode = pad_index
  // 2. To exit press bottom 'play' bottom one more time
  //   * knob_mode = -1
  char knob_mode = CONTROLLER_KNOB_MODE_OFF;

  // Allows editing selected sample parameter using knobs. When this mode is off
  // controller knobs are not responsive.
  // 1. To enter press top 'play' bottom and one of the pads pad successively
  //   * Selected pad turn ORANGE
  //   * keyboard_mode = pad_index
  // 2. To exit press bottom 'play' bottom one more time
  //   * keyboard_mode = -1
  char keyboard_mode = CONTROLLER_KEYBOARD_MODE_OFF;

  // Auxiliary flags
  char top_play_button_pressed;
  char bottom_play_button_pressed;

  const uint8_t pad_notes[16] = {
    96, 97, 98, 99, 100, 101, 102, 103,
    112, 113, 114, 115, 116, 117, 118, 119
  };

  Pad pads[16] = {
    {PAD1, 7}, {PAD2, 6}, {PAD3, 1}, {PAD4, 5}, {PAD5, 5}, {PAD6, 8}, {PAD7, 6}, {PAD8, 7},
    {PAD9, 2}, {PAD10, 3}, {PAD11, 0}, {PAD12, 4}, {PAD13, 4}, {PAD14, 0}, {PAD15, 3}, {PAD16, 2}
  };

public:
  void keyPressed(uint8_t key_note) {
    uint8_t control_change_msg[3] = {CHANNEL1_CONTROL_CHANGE, SPEED, key_note};
    uint8_t note_on_msg[3] = {CHANNEL1_NOTE_ON, 0, 127};

    if (keyboard_mode > CONTROLLER_KEYBOARD_MODE_OFF) {
      // Sending sample speed edit before and after keypress to avoid pitch ramp
      // between notes
      control_change_msg[0] += pads[keyboard_mode].mapping;
      MIDI_SERIAL_PORT_1.write(control_change_msg, 3);
      MIDI_SERIAL_PORT_1.write(control_change_msg, 3);
      note_on_msg[0] += pads[keyboard_mode].mapping;
      MIDI_SERIAL_PORT_1.write(note_on_msg, 3);
      MIDI_SERIAL_PORT_1.write(control_change_msg, 3);
      MIDI_SERIAL_PORT_1.write(control_change_msg, 3);
    }
  }

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
    else if (bottom_play_button_pressed == 1) {
      enableKeyboardMode(pad_index);
    }
    else {
      if (pads[pad_index].mapping > -1) {
        msg[0] += pads[pad_index].mapping;
        MIDI_SERIAL_PORT_1.write(msg, 3);
      }
    }
    render();
  }

  void topPlayButtonPressed() {
    // Primary mode attached to the button
    if (knob_mode > CONTROLLER_KNOB_MODE_OFF) {
      disableKnobMode();
    }
    else {
      top_play_button_pressed = 1;
    }
    render();
  }

  void bottomPlayButtonPressed() {
    // Primary mode attached to the button
    if (keyboard_mode > CONTROLLER_KEYBOARD_MODE_OFF) {
      disableKeyboardMode();
    }
    // Play buttons combinations handling
    else {
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
    }
    render();
  }

  void topPlayButtonReleased() {
    top_play_button_pressed = 0;
  }

  void bottomPlayButtonReleased() {
    bottom_play_button_pressed = 0;
  }

  void disableKeyboardMode() {
    for (char i = 0; i < 16; i++) {
      pads[i].keyboard_mode = PAD_KEYBOARD_MODE_OFF;
    }
    keyboard_mode = CONTROLLER_KEYBOARD_MODE_OFF;
  }

  void disableKnobMode() {
    for (char i = 0; i < 16; i++) {
      pads[i].knob_mode = PAD_KNOB_MODE_OFF;
    }
    knob_mode = CONTROLLER_KNOB_MODE_OFF;
  }

  void enableKeyboardMode(char pad_index) {
    if (keyboard_mode == CONTROLLER_KEYBOARD_MODE_OFF) {
      remap_mode = CONTROLLER_REMAP_MODE_OFF;
      keyboard_mode = pad_index;
      pads[pad_index].keyboard_mode = PAD_KEYBOARD_MODE_ON;

      disableKnobMode();
    }
  }

  void enableKnobMode(char pad_index) {
    if (knob_mode == CONTROLLER_KNOB_MODE_OFF) {
      remap_mode = CONTROLLER_REMAP_MODE_OFF;
      knob_mode = pad_index;
      pads[pad_index].knob_mode = PAD_KNOB_MODE_ON;

      disableKeyboardMode();
    }
  }

  void mapPadToVoice(char voice_index) {
    if (voice_index > 9) {
      voice_index = -1;
    }
    pads[remap_mode].mapping = voice_index;
    remap_mode = CONTROLLER_REMAP_MODE_OFF;
  }

  void renderActivePads() {
    // 'note_on' message templape
    uint8_t msg[3] = {CHANNEL1_NOTE_ON, 0, 0};

    for (char i = 0; i < 16; i++) {
      msg[1] = pads[i].note;
      if (pads[i].mapping > -1) {
        if (pads[i].knob_mode == PAD_KNOB_MODE_ON) {
          msg[2] = ORANGE;
        }
        else if (pads[i].keyboard_mode == PAD_KEYBOARD_MODE_ON) {
          msg[2] = RED;
        }
        else {
          msg[2] = YELLOW;
        }

      }
      else {
        msg[2] = BLACK;
      }
      Usb.Task();
      Midi.SendData(msg, 1);
      delay(1);
    }
  }

  void renderVoiceSelect() {
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

  void render_play_buttons() {
    // 'note_on' message templape
    uint8_t top_play_msg[3] = {CHANNEL1_NOTE_ON, TOP_PLAY, 0};
    uint8_t bottom_play_msg[3] = {CHANNEL1_NOTE_ON, BOTTOM_PLAY, 0};

    if (knob_mode > CONTROLLER_KNOB_MODE_OFF) {
      top_play_msg[2] = RED;
    }

    if (keyboard_mode > CONTROLLER_KEYBOARD_MODE_OFF) {
      bottom_play_msg[2] = RED;
    }

    if (remap_mode > CONTROLLER_REMAP_MODE_OFF) {
      top_play_msg[2] = RED;
      bottom_play_msg[2] = RED;
    }

    if (remap_mode == CONTROLLER_REMAP_MODE_OFF and knob_mode == CONTROLLER_KNOB_MODE_OFF) {
      top_play_msg[2] = BLACK;
    }

    if (remap_mode == CONTROLLER_REMAP_MODE_OFF and keyboard_mode == CONTROLLER_KEYBOARD_MODE_OFF) {
      bottom_play_msg[2] = BLACK;
    }

    Usb.Task();
    Midi.SendData(top_play_msg, 1);
    delay(1);
    Usb.Task();
    Midi.SendData(bottom_play_msg, 1);
    delay(1);
  }

  void render() {
    if (remap_mode > CONTROLLER_REMAP_MODE_SELECT_PAD) {
      renderVoiceSelect();
    }
    else {
      renderActivePads();
    }

    render_play_buttons();
  }
};

#endif // MAPPER_HPP
