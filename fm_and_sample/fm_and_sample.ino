#include <usbh_midi.h>
#include <usbhub.h>
#include <SPI.h>

USB Usb;
USBH_MIDI  Midi(&Usb);

#define SERIAL_MONITOR Serial
#define MIDI_SERIAL_PORT_1 Serial1
#define MIDI_SERIAL_PORT_2 Serial2
//////////////////////////
// MIDI Pin assign
// 2 : GND
// 4 : +5V(Vcc) with 220ohm
// 5 : TX
//////////////////////////

// controller specific macro
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

// fm specific macro
#define DISPLAY_CODE_LENGTH 10

#define OP1 5
#define OP2 4
#define OP3 3
#define OP4 2
#define OP5 1
#define OP6 0

#define OPERATOR_EG_RATE_1 0
#define OPERATOR_EG_RATE_2 1
#define OPERATOR_EG_RATE_3 2
#define OPERATOR_EG_RATE_4 3
#define OPERATOR_EG_LEVEL_1 4
#define OPERATOR_EG_LEVEL_2 5
#define OPERATOR_EG_LEVEL_3 6
#define OPERATOR_EG_LEVEL_4 7
#define OPERATOR_KEYBOARD_LEVEL_SCALE_BREAK_POINT 8
#define OPERATOR_KEYBOARD_LEVEL_SCALE_LEFT_DEPTH 9
#define OPERATOR_KEYBOARD_LEVEL_SCALE_RIGHT_DEPTH 10
#define OPERATOR_KEYBOARD_LEVEL_SCALE_LEFT_CURVE 11
#define OPERATOR_KEYBOARD_LEVEL_SCALE_RIGHT_CURVE 12
#define OPERATOR_KEYBOARD_RATE_SCALING 13
#define OPERATOR_MODULATION_SENSITIVITY_AMPLITUDE 14
#define OPERATOR_OPERATOR_KEY_VELOCITY_SENSITIVITY 15
#define OPERATOR_OUTPUT_LEVEL 16
#define OPERATOR_OSCILLATOR_MODE 17
#define OPERATOR_OSCILLATOR_FREQUENCY_COARSE 18
#define OPERATOR_OSCILLATOR_FREQUENCY_FINE 19
#define OPERATOR_DETUNE 20

#define ALL_PITCH_EG_RATE_1 0
#define ALL_PITCH_EG_RATE_2 1
#define ALL_PITCH_EG_RATE_3 2
#define ALL_PITCH_EG_RATE_4 3
#define ALL_PITCH_EG_LEVEL_1 4
#define ALL_PITCH_EG_LEVEL_2 5
#define ALL_PITCH_EG_LEVEL_3 6
#define ALL_PITCH_EG_LEVEL_4 7
#define ALL_ALGORITHM 8
#define ALL_FEEDBACK 9
#define ALL_OSCILLATOR_SYNC 10
#define ALL_LFO_SPEED 11
#define ALL_LFO_DELAY 12
#define ALL_LFO_PITCH_MODULATION_DEPTH 13
#define ALL_LFO_AMPLITUDE_MODULATION_DEPTH 14
#define ALL_LFO_SYNC 15
#define ALL_LFO_WAVE 16
#define ALL_MODULATION_SENSITIVITY_PITCH 17
#define ALL_TRANSPOSE 18

#define PROGRAM_VELOCITY 41
#define PROGRAM_MODULATOR_ATTACK 42
#define PROGRAM_MODULATOR_DECAY 43
#define PROGRAM_CARRIER_ATTACK 44
#define PROGRAM_CARRIER_DECAY 45
#define PROGRAM_LFO_RATE 46
#define PROGRAM_LFO_PITCH_DEPTH 47
#define PROGRAM_ALGORITHM 48
#define PROGRAM_ARP_TYPE  49
#define PROGRAM_ARP_DIV 50

// sample specific macros
#define CONTROLLER_REMAP_MODE_OFF -2
#define CONTROLLER_REMAP_MODE_SELECT_PAD -1

#define CONTROLLER_KEYBOARD_MODE_OFF -1
#define PAD_KEYBOARD_MODE_OFF 0
#define PAD_KEYBOARD_MODE_ON 1

#define CONTROLLER_KNOB_MODE_OFF -1
#define PAD_KNOB_MODE_OFF 0
#define PAD_KNOB_MODE_ON 1

// other
#define SAMPLE 1
#define FM 2

struct Param {
  const char* display_code;
  char max_value;
  char current_value;
  char previous_value;

  Param (const char* d, char m) {
    display_code = d;
    max_value = m;
    current_value = 0;
    previous_value = 0;
  }

  Param (const char* d, char m, char c) {
    display_code = d;
    max_value = m;
    current_value = c;
    previous_value = c;
  }

  Param (const char* d, char m, char c, char p) {
    display_code = d;
    max_value = m;
    current_value = c;
    previous_value = p;
  }

  void parameter_set_value(char value) {
    previous_value = current_value;
    current_value = value;
  }

  void parameter_restore_previous_value() {
    char buffer = current_value;
    current_value = previous_value;
    previous_value = buffer;
  }

};

struct Operator {
  Operator (char a) {
    index = a;
  }

  char index;
  char power = 0;

  char modified_parameter = 0;
  char last_modified_parameter_code[DISPLAY_CODE_LENGTH];

  Param parameters[21] = {
    {"egr1", 99}, {"egr2", 99}, {"egr3", 99}, {"egr4", 99},
    {"egl1", 99}, {"egl2", 99}, {"egl3", 99}, {"egl4", 99},
    {"lsbp", 99}, {"lsld", 99}, {"lsrd", 99}, {"lslc", 3},
    {"lslr", 3}, {"krs ", 7}, {"ams ", 7}, {"kvs", 7},
    {"olvl", 99}, {"fixd", 1}, {"fcrs", 31}, {"ffne", 99},
    {"detn", 14}
  };

  void operator_set_param_value(char param_index, char value) {
    parameters[param_index].previous_value = parameters[param_index].current_value;
    parameters[param_index].current_value = parameters[param_index].max_value * value * 1.0/ 128.0;

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "%d %4s %2i", index, parameters[param_index].display_code, parameters[param_index].current_value);

    //SERIAL_MONITOR.println(last_modified_parameter_code);
  }

  void operator_power_on() {
      power = 1;
      parameters[OPERATOR_OUTPUT_LEVEL].parameter_restore_previous_value();
      parameters[OPERATOR_OSCILLATOR_MODE].parameter_restore_previous_value();
      parameters[OPERATOR_OSCILLATOR_FREQUENCY_COARSE].parameter_restore_previous_value();
      parameters[OPERATOR_OSCILLATOR_FREQUENCY_FINE].parameter_restore_previous_value();
      parameters[OPERATOR_DETUNE].parameter_restore_previous_value();

      modified_parameter = 1;
      sprintf(last_modified_parameter_code, "%d on", index);

      //SERIAL_MONITOR.println(last_modified_parameter_code);

    }

  void operator_power_off() {
      power = 0;
      parameters[OPERATOR_OUTPUT_LEVEL].parameter_set_value(0);
      parameters[OPERATOR_OSCILLATOR_MODE].parameter_set_value(1);
      parameters[OPERATOR_OSCILLATOR_FREQUENCY_COARSE].parameter_set_value(0);
      parameters[OPERATOR_OSCILLATOR_FREQUENCY_FINE].parameter_set_value(0);
      parameters[OPERATOR_DETUNE].parameter_set_value(0);

      modified_parameter = 1;
      sprintf(last_modified_parameter_code, "%d off", index);

      //SERIAL_MONITOR.println(last_modified_parameter_code);
    }

  void oscillator_fixed_mode() {
    parameters[OPERATOR_OSCILLATOR_MODE].parameter_set_value(1);

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "%d fixed", index);

    //SERIAL_MONITOR.println(last_modified_parameter_code);
  }

  void oscillator_ratio_mode() {
    //SERIAL_MONITOR.println("oscillator_ratio_mode");
    parameters[OPERATOR_OSCILLATOR_MODE].parameter_set_value(0);

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "%d ratio", index);

    //SERIAL_MONITOR.println(last_modified_parameter_code);
  }

  void amplitude_modulation_on() {
    parameters[OPERATOR_MODULATION_SENSITIVITY_AMPLITUDE].parameter_set_value(7);

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "%d mod on", index);

    //SERIAL_MONITOR.println(last_modified_parameter_code);
  }

  void amplitude_modulation_off() {
    parameters[OPERATOR_MODULATION_SENSITIVITY_AMPLITUDE].parameter_set_value(0);

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "%d mod off", index);

    //SERIAL_MONITOR.println(last_modified_parameter_code);
  }

  void enable_bass_envelope() {
    parameters[OPERATOR_EG_LEVEL_1].parameter_set_value(99);
    parameters[OPERATOR_EG_LEVEL_2].parameter_set_value(0);
    parameters[OPERATOR_EG_LEVEL_3].parameter_set_value(0);
    parameters[OPERATOR_EG_LEVEL_4].parameter_set_value(0);
    parameters[OPERATOR_EG_RATE_1].parameter_set_value(88);
    parameters[OPERATOR_EG_RATE_2].parameter_set_value(60);
    parameters[OPERATOR_EG_RATE_3].parameter_set_value(99);
    parameters[OPERATOR_EG_RATE_4].parameter_set_value(99);

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "%d bass", index);

    //SERIAL_MONITOR.println(last_modified_parameter_code);
  }

  void enable_piano_envelope() {
    parameters[OPERATOR_EG_LEVEL_1].parameter_set_value(99);
    parameters[OPERATOR_EG_LEVEL_2].parameter_set_value(46);
    parameters[OPERATOR_EG_LEVEL_3].parameter_set_value(0);
    parameters[OPERATOR_EG_LEVEL_4].parameter_set_value(0);
    parameters[OPERATOR_EG_RATE_1].parameter_set_value(95);
    parameters[OPERATOR_EG_RATE_2].parameter_set_value(25);
    parameters[OPERATOR_EG_RATE_3].parameter_set_value(25);
    parameters[OPERATOR_EG_RATE_4].parameter_set_value(67);

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "%d piano", index);

    //SERIAL_MONITOR.println(last_modified_parameter_code);
  }

  void enable_organ_envelope() {
    parameters[OPERATOR_EG_LEVEL_1].parameter_set_value(99);
    parameters[OPERATOR_EG_LEVEL_2].parameter_set_value(99);
    parameters[OPERATOR_EG_LEVEL_3].parameter_set_value(99);
    parameters[OPERATOR_EG_LEVEL_4].parameter_set_value(0);
    parameters[OPERATOR_EG_RATE_1].parameter_set_value(99);
    parameters[OPERATOR_EG_RATE_2].parameter_set_value(80);
    parameters[OPERATOR_EG_RATE_3].parameter_set_value(22);
    parameters[OPERATOR_EG_RATE_4].parameter_set_value(90);

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "%d organ", index);

    //SERIAL_MONITOR.println(last_modified_parameter_code);
  }

  void enable_strings_envelope() {
    parameters[OPERATOR_EG_LEVEL_1].parameter_set_value(99);
    parameters[OPERATOR_EG_LEVEL_2].parameter_set_value(78);
    parameters[OPERATOR_EG_LEVEL_3].parameter_set_value(41);
    parameters[OPERATOR_EG_LEVEL_4].parameter_set_value(0);
    parameters[OPERATOR_EG_RATE_1].parameter_set_value(46);
    parameters[OPERATOR_EG_RATE_2].parameter_set_value(15);
    parameters[OPERATOR_EG_RATE_3].parameter_set_value(23);
    parameters[OPERATOR_EG_RATE_4].parameter_set_value(33);

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "%d string", index);

    //SERIAL_MONITOR.println(last_modified_parameter_code);
  }

  void get_parameter_values() {
    // SERIAL_MONITOR.println("Operator: ");

    for (char i = 0; i < 21; i++) {
      MIDI_SERIAL_PORT_2.write(parameters[i].current_value);
      // SERIAL_MONITOR.print(parameters[i].current_value, DEC);
      // SERIAL_MONITOR.print(" ");
    }
    // SERIAL_MONITOR.println();
  }

  char* get_last_modified_param() {
    modified_parameter = 0;
    return last_modified_parameter_code;
  }

};

struct All {

  char modified_parameter = 0;
  char last_modified_parameter_code[DISPLAY_CODE_LENGTH];

  Param parameters[19] = {
    {"ptr1", 99, 99, 0}, {"ptr2", 99, 99, 0}, {"ptr3", 99, 99, 0}, {"ptr4", 99, 99, 0},
    {"ptl1", 99, 50, 0}, {"ptl2", 99, 50, 0}, {"ptl3", 99, 50, 0}, {"ptl4", 99, 50, 0},
    {"algo", 31, 0, 0}, {"feed", 7, 0, 0}, {"oks ", 1, 0, 0}, {"lfor", 99, 0, 0},
    {"lfod", 99, 0, 0}, {"lpmd", 99, 0, 0}, {"lamd", 99, 99, 0}, {"lks ", 1, 0, 0},
    {"lfow", 5, 0, 0}, {"msp ", 7, 0, 0}, {"trsp", 48, 24, 0}
  };

  void all_set_param_value(char param_index, char value) {
    parameters[param_index].previous_value = parameters[param_index].current_value;
    parameters[param_index].current_value = parameters[param_index].max_value * value / 128.0;;

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "A %4s %2i", parameters[param_index].display_code, parameters[param_index].current_value);

    SERIAL_MONITOR.println(last_modified_parameter_code);
  }

  void get_parameter_values() {
    for (char i = 0; i < 19; i++) {
      MIDI_SERIAL_PORT_2.write(parameters[i].current_value);
    }
  }

  char* get_last_modified_param() {
    modified_parameter = 0;
    return last_modified_parameter_code;
  }
};

struct Patch {

  Operator operators[6] = {{6}, {5}, {4}, {3}, {2}, {1}};
  All all;

  void send_sysex() {
    uint8_t patch_start[6] = {0xf0, 0x43, 0x00, 0x00, 0x01, 0x1b};
    for (char i = 0; i < 6; i++) {
      MIDI_SERIAL_PORT_2.write(patch_start[i]);
    }

    for (char i = 0; i < 6; i++) {
      operators[i].get_parameter_values();
    }

    char patch_name[DISPLAY_CODE_LENGTH] = {'p', 'a', 't', 'c', 'h', '_' , 'n', 'a', 'm', 'e'};

    all.get_parameter_values();

    for (char i = 0; i < 6; i++) {
      if (operators[i].modified_parameter == 1) {
        strncpy(patch_name, operators[i].get_last_modified_param(), DISPLAY_CODE_LENGTH);
        break;
      }
    }

    if (all.modified_parameter == 1) {
      strncpy(patch_name, all.get_last_modified_param(), DISPLAY_CODE_LENGTH);
    }

    SERIAL_MONITOR.println(patch_name);

    for (char i = 0; i < 10; i++) {
      MIDI_SERIAL_PORT_2.write(patch_name[i]);
    }

    SERIAL_MONITOR.print(operators[OP1].power, DEC);
    SERIAL_MONITOR.print(" ");
    SERIAL_MONITOR.print(operators[OP2].power, DEC);
    SERIAL_MONITOR.print(" ");
    SERIAL_MONITOR.print(operators[OP3].power, DEC);
    SERIAL_MONITOR.print(" ");
    SERIAL_MONITOR.print(operators[OP4].power, DEC);
    SERIAL_MONITOR.println();

    char operators_power_status = operators[OP1].power + 2 * operators[OP2].power +
      4 * operators[OP3].power + 8 * operators[OP4].power;

    MIDI_SERIAL_PORT_2.write(operators_power_status);
    MIDI_SERIAL_PORT_2.write(0xf7);
  }

};

Patch patch;

char program_algorithm_mapping[128] =  {1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 32};

struct Program {
  struct Param {
    char index;
    char default_value;
    char value;
    char patch_param_index;
    char* patch_param_mapping;

    Param (const char i, char d) {
      index = i;
      default_value = d;
      value = d;
      patch_param_index = -1;
    }

    Param (const char i, char d, char p, const char* m) {
      index = i;
      default_value = d;
      value = d;
      patch_param_index = p;
      patch_param_mapping = m;
    }
  };

  Param parameters[7] = {
    {PROGRAM_MODULATOR_ATTACK, 64},
    {PROGRAM_MODULATOR_DECAY, 64},
    {PROGRAM_CARRIER_ATTACK, 64},
    {PROGRAM_CARRIER_DECAY, 64},
    {PROGRAM_LFO_RATE, 64},
    {PROGRAM_LFO_PITCH_DEPTH, 0},
    {PROGRAM_ALGORITHM, 7, ALL_ALGORITHM, program_algorithm_mapping}
  };

  void program_set_param_value(char param_index, char value) {
    uint8_t msg[3] = {176, param_index, value};

    for (char i = 0; i < 7; i++) {
      if (parameters[i].index == param_index and parameters[i].default_value != value) {
        parameters[i].value = value;

        // SERIAL_MONITOR.print(value, DEC);
        // SERIAL_MONITOR.println();

        MIDI_SERIAL_PORT_2.write(msg, 3);

        if (parameters[i].patch_param_index > -1) {
          patch.all.parameters[parameters[i].patch_param_index].current_value = parameters[i].patch_param_mapping[value];
          patch.all.modified_parameter = 1;
          sprintf(patch.all.last_modified_parameter_code, "A %4s %2i", patch.all.parameters[parameters[i].patch_param_index].display_code, patch.all.parameters[parameters[i].patch_param_index].current_value);
        }
      }
    }
  }

  void send_programm() {
    uint8_t msg[3] = {176, 0, 0};

    for (char i = 0; i < 7; i++) {
      if (parameters[i].default_value != parameters[i].value) {
        msg[1] = parameters[i].index;
        msg[2] = parameters[i].value;

        // SERIAL_MONITOR.print(parameters[i].default_value, DEC);
        // SERIAL_MONITOR.print(" ");
        // SERIAL_MONITOR.print(parameters[i].value, DEC);
        // SERIAL_MONITOR.println();

        MIDI_SERIAL_PORT_2.write(msg, 3);
        delay(1);
      }
    }
  }
};

Program program;

struct Option {
  char color;
  void (Operator::*callback)(); // pointer to function
};

struct Pad {
  Pad(const char a, char m): address(a), mapping(m), color(BLACK) {}

  const char address;
  char mapping;
  char color;
};

struct Pad1 : Pad {
  Pad1(const char a, char m, Option *o, char l):
    Pad(a, m), options(o), options_len(l), selected_option(0) {}

  Pad1(const char a, char m, Option *o, char l, char s):
    Pad(a, m), options(o), options_len(l), selected_option(s) {}

  Option* options;
  char options_len;
  char selected_option;

  void pad_select_next_option() {
    if (options_len == 0) {
      return;
    }
    if (selected_option + 1 == options_len) {
      selected_option = 0;
    }
    else {
      selected_option++;
    }

    color = options[selected_option].color;

    void (Operator::*callback)() = options[selected_option].callback;
    (patch.operators[mapping].*callback)();
  }
};

struct Pad2 : Pad {
  Pad2(const char a, char m): Pad(a, m) {}

  char keyboard_mode = PAD_KEYBOARD_MODE_OFF;
  char knob_mode = PAD_KNOB_MODE_OFF;
};

struct Mapper1 {
  Option operator_power_option[2] = {{GREEN, &Operator::operator_power_on}, {RED, &Operator::operator_power_off}};
  Option oscillator_mode_option[2] = {{GREEN, &Operator::oscillator_fixed_mode}, {RED, &Operator::oscillator_ratio_mode}};
  Option amplitude_modulation_option[2] = {{GREEN, &Operator::amplitude_modulation_on}, {RED, &Operator::amplitude_modulation_off}};
  Option envelope_generator_option[4] = {{RED, &Operator::enable_bass_envelope}, {YELLOW, &Operator::enable_piano_envelope},
    {ORANGE, &Operator::enable_organ_envelope}, {GREEN, &Operator::enable_strings_envelope}};

  Pad1 pads[16] = {
    {PAD1, OP1, operator_power_option, 2},
    {PAD2, OP1, oscillator_mode_option, 2},
    {PAD3, OP2, operator_power_option, 2},
    {PAD4, OP2, oscillator_mode_option, 2},
    {PAD5, OP3, operator_power_option, 2},
    {PAD6, OP3, oscillator_mode_option, 2},
    {PAD7, OP4, operator_power_option, 2},
    {PAD8, OP4, oscillator_mode_option, 2},
    {PAD9, OP1, amplitude_modulation_option, 2},
    {PAD10, OP1, envelope_generator_option, 4},
    {PAD11, OP2, amplitude_modulation_option, 2},
    {PAD12, OP2, envelope_generator_option, 4},
    {PAD13, OP3, amplitude_modulation_option, 2},
    {PAD14, OP3, envelope_generator_option, 4},
    {PAD15, OP4, amplitude_modulation_option, 2},
    {PAD16, OP4, envelope_generator_option, 4}
  };

  char pressed_pad_address = 0;
  char ignore_pad_press = 0;

  void mapper_pad_pressed(char pad_index) {
    pressed_pad_address = pads[pad_index].address;
  }

  void mapper_pad_released(char pad_index) {
    if (pressed_pad_address == pads[pad_index].address) {
      pressed_pad_address = 0;
      if (ignore_pad_press == 0) {
        pads[pad_index].pad_select_next_option();
        patch.send_sysex();
        program.send_programm();
      }
      else {
        ignore_pad_press = 0;
      }
    }
    mapper_render();
  }

  void mapper_knob_rotated(char knob_address, char knob_value) {
    // knob only
    if (pressed_pad_address == 0) {
      if (knob_address == KNOB1) {
        program.program_set_param_value(PROGRAM_CARRIER_DECAY, knob_value);
      }
      else if (knob_address == KNOB2) {
        program.program_set_param_value(PROGRAM_CARRIER_ATTACK, knob_value);
      }
      else if (knob_address == KNOB3) {
        program.program_set_param_value(PROGRAM_MODULATOR_DECAY, knob_value);
      }
      else if (knob_address == KNOB4) {
        program.program_set_param_value(PROGRAM_MODULATOR_ATTACK, knob_value);
      }
      else if (knob_address == KNOB5) {
        program.program_set_param_value(PROGRAM_LFO_RATE, knob_value);
      }
      else if (knob_address == KNOB6) {
        program.program_set_param_value(PROGRAM_LFO_PITCH_DEPTH, knob_value);
      }
      else if (knob_address == KNOB7) {
        program.program_set_param_value(PROGRAM_ALGORITHM, knob_value);
        patch.send_sysex();
        program.send_programm();
      }
      else if (knob_address == KNOB8) {
        patch.all.all_set_param_value(ALL_FEEDBACK, knob_value);
        patch.send_sysex();
        program.send_programm();
      }
    }
    // pad + knob combos
    else {
      ignore_pad_press = 1;
      // OP1
      if (knob_address == KNOB1 and pressed_pad_address == PAD1) {
        patch.operators[OP1].operator_set_param_value(OPERATOR_OUTPUT_LEVEL, knob_value);
        patch.send_sysex();
        program.send_programm();
      }

      else if (knob_address == KNOB2 and pressed_pad_address == PAD1) {
        patch.operators[OP1].operator_set_param_value(OPERATOR_OPERATOR_KEY_VELOCITY_SENSITIVITY, knob_value);
        patch.send_sysex();
        program.send_programm();
      }

      else if (knob_address == KNOB1 and pressed_pad_address == PAD2) {
        patch.operators[OP1].operator_set_param_value(OPERATOR_OSCILLATOR_FREQUENCY_COARSE, knob_value);
        patch.send_sysex();
        program.send_programm();
      }

      else if (knob_address == KNOB2 and pressed_pad_address == PAD2) {
        patch.operators[OP1].operator_set_param_value(OPERATOR_DETUNE, knob_value);
        patch.send_sysex();
        program.send_programm();
      }

      // OP2
      else if (knob_address == KNOB3 and pressed_pad_address == PAD3) {
        patch.operators[OP2].operator_set_param_value(OPERATOR_OUTPUT_LEVEL, knob_value);
        patch.send_sysex();
        program.send_programm();
      }

      else if (knob_address == KNOB4 and pressed_pad_address == PAD3) {
        patch.operators[OP2].operator_set_param_value(OPERATOR_OPERATOR_KEY_VELOCITY_SENSITIVITY, knob_value);
        patch.send_sysex();
        program.send_programm();
      }

      else if (knob_address == KNOB3 and pressed_pad_address == PAD4) {
        patch.operators[OP2].operator_set_param_value(OPERATOR_OSCILLATOR_FREQUENCY_COARSE, knob_value);
        patch.send_sysex();
        program.send_programm();
      }

      else if (knob_address == KNOB4 and pressed_pad_address == PAD4) {
         patch.operators[OP2].operator_set_param_value(OPERATOR_DETUNE, knob_value);
         patch.send_sysex();
         program.send_programm();
       }

      // OP3
      else if (knob_address == KNOB5 and pressed_pad_address == PAD5) {
         patch.operators[OP3].operator_set_param_value(OPERATOR_OUTPUT_LEVEL, knob_value);
         patch.send_sysex();
         program.send_programm();
       }

      else if (knob_address == KNOB6 and pressed_pad_address == PAD5) {
         patch.operators[OP3].operator_set_param_value(OPERATOR_OPERATOR_KEY_VELOCITY_SENSITIVITY, knob_value);
         patch.send_sysex();
         program.send_programm();
       }

      else if (knob_address == KNOB5 and pressed_pad_address == PAD6) {
         patch.operators[OP3].operator_set_param_value(OPERATOR_OSCILLATOR_FREQUENCY_COARSE, knob_value);
         patch.send_sysex();
         program.send_programm();
       }

      else if (knob_address == KNOB6 and pressed_pad_address == PAD6) {
         patch.operators[OP3].operator_set_param_value(OPERATOR_DETUNE, knob_value);
         patch.send_sysex();
         program.send_programm();
       }

       // OP4
      else if (knob_address == KNOB7 and pressed_pad_address == PAD7) {
         patch.operators[OP4].operator_set_param_value(OPERATOR_OUTPUT_LEVEL, knob_value);
         patch.send_sysex();
         program.send_programm();
       }

      else if (knob_address == KNOB8 and pressed_pad_address == PAD7) {
         patch.operators[OP4].operator_set_param_value(OPERATOR_OPERATOR_KEY_VELOCITY_SENSITIVITY, knob_value);
         patch.send_sysex();
         program.send_programm();
       }

      else if (knob_address == KNOB7 and pressed_pad_address == PAD8) {
         patch.operators[OP4].operator_set_param_value(OPERATOR_OSCILLATOR_FREQUENCY_COARSE, knob_value);
         patch.send_sysex();
         program.send_programm();
       }

      else if (knob_address == KNOB8 and pressed_pad_address == PAD8) {
         patch.operators[OP4].operator_set_param_value(OPERATOR_DETUNE, knob_value);
         patch.send_sysex();
         program.send_programm();
       }
    }
  }

  void mapper_render() {
    // 'note_on' message templape
    uint8_t msg[3] = {CHANNEL1_NOTE_ON, 0, 0};

    for (char i = 0; i < 16; i++) {
      msg[1] = pads[i].address;
      msg[2] = pads[i].options[pads[i].selected_option].color;

      Usb.Task();
      Midi.SendData(msg, 1);
      delay(1);
    }
  }

};

struct Mapper2 {
  /*You could you use knob and keyboard modes simultaneously, but when you enter remap mode,
  you exit knob and keyboard modes automaticaly */
  char remap_mode = CONTROLLER_REMAP_MODE_OFF;

  /* Allows playing selected sample chromatically using keyboard. When this mode is off
  lk keyboard is not responsive.
  1. To enter press bottom 'play' buttom and one of the pads pad successively
    * Selected pad turn RED
    * Flag is equal to pad note (ex: 96)
  2. To exit press bottom 'play' buttom one more time
    * Flag is equal 0
  */
  char knob_mode = CONTROLLER_KNOB_MODE_OFF;

  /* Allows editing selected sample parameter using knobs. When this mode is off
  lk knobs are not responsive.
  1. To enter press top 'play' buttom and one of the pads pad successively
    * Selected pad turn ORAN
    * Flag is equal to pad note (ex: 96)
  2. To exit press bottom 'play' buttom one more time
    * Flag is equal 0
  */
  char keyboard_mode = CONTROLLER_KEYBOARD_MODE_OFF;

  Pad2 pads[16] = {
    {PAD1, 7}, {PAD2, 6}, {PAD3, 1}, {PAD4, 5}, {PAD5, 5}, {PAD6, 8}, {PAD7, 6}, {PAD8, 7},
    {PAD9, 2}, {PAD10, 3}, {PAD11, 0}, {PAD12, 4}, {PAD13, 4}, {PAD14, 0}, {PAD15, 3}, {PAD16, 2}
  };

  char top_play_button_pressed;
  char bottom_play_button_pressed;

  void mapper_pad_pressed(char pad_index) {
    uint8_t msg[3] = {CHANNEL1_NOTE_ON, 0, 127};

    if (remap_mode == CONTROLLER_REMAP_MODE_SELECT_PAD) {
      remap_mode = pad_index;
    }
    else if (remap_mode > CONTROLLER_REMAP_MODE_SELECT_PAD) {
      mapper_map_pad_to_voice(pad_index);
    }
    else if (top_play_button_pressed == 1) {
      mapper_enable_knob_mode(pad_index);
    }
    else if (bottom_play_button_pressed == 1) {
      mapper_enable_keyboard_mode(pad_index);
    }
    else {
      if (pads[pad_index].mapping > -1) {
        msg[0] += pads[pad_index].mapping;
        MIDI_SERIAL_PORT_1.write(msg, 3);
      }
    }
  }

  void mapper_top_play_button_pressed() {
    if (knob_mode > CONTROLLER_KNOB_MODE_OFF) {
      mapper_disable_knob_mode();
    }
    else {
      top_play_button_pressed = 1;
    }
  }

  void mapper_bottom_play_button_pressed() {
    if (keyboard_mode > CONTROLLER_KEYBOARD_MODE_OFF) {
      mapper_disable_keyboard_mode();
    }
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
  }

  void mapper_top_play_button_released() {
    top_play_button_pressed = 0;
  }

  void mapper_bottom_play_button_released() {
    bottom_play_button_pressed = 0;
  }

  void mapper_disable_keyboard_mode() {
    for (char i = 0; i < 16; i++) {
      pads[i].keyboard_mode = PAD_KEYBOARD_MODE_OFF;
    }
    keyboard_mode = CONTROLLER_KEYBOARD_MODE_OFF;
    mapper_render();
  }

  void mapper_disable_knob_mode() {
    for (char i = 0; i < 16; i++) {
      pads[i].knob_mode = PAD_KNOB_MODE_OFF;
    }
    knob_mode = CONTROLLER_KNOB_MODE_OFF;
    mapper_render();
  }

  void mapper_enable_keyboard_mode(char pad_index) {
    if (keyboard_mode == CONTROLLER_KEYBOARD_MODE_OFF) {
      remap_mode = CONTROLLER_REMAP_MODE_OFF;
      keyboard_mode = pad_index;
      pads[pad_index].keyboard_mode = PAD_KEYBOARD_MODE_ON;

      mapper_disable_knob_mode();
    }
  }

  void mapper_enable_knob_mode(char pad_index) {
    if (knob_mode == CONTROLLER_KNOB_MODE_OFF) {
      remap_mode = CONTROLLER_REMAP_MODE_OFF;
      knob_mode = pad_index;
      pads[pad_index].knob_mode = PAD_KNOB_MODE_ON;

      mapper_disable_keyboard_mode();
    }
  }

  void mapper_map_pad_to_voice(char voice_index) {
    if (voice_index > 9) {
      voice_index = -1;
    }
    pads[remap_mode].mapping = voice_index;
    remap_mode = CONTROLLER_REMAP_MODE_OFF;
    mapper_render();
  }

  void mapper_render_active_pads() {
    // 'note_on' message templape
    uint8_t msg[3] = {CHANNEL1_NOTE_ON, 0, 0};

    for (char i = 0; i < 16; i++) {
      msg[1] = pads[i].address;
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

  void mapper_render_voice_select() {
    // 'note_on' message templape
    uint8_t msg[3] = {CHANNEL1_NOTE_ON, 0, 0};

    for (char i = 0; i < 16; i++) {
      msg[1] = pads[i].address;
      if (pads[i].address >= PAD1 and pads[i].address <= PAD8 or pads[i].address >= PAD9 and pads[i].address <= PAD10) {
        msg[2] = GREEN;
      }
      else {
        msg[2] = BLACK;
      }
      Usb.Task();
      Midi.SendData(msg, 1);
      delay(1);
    }
  }

  void mapper_render_play_buttons() {
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

  void mapper_render() {
    if (remap_mode > 0) {
      mapper_render_voice_select();
    }
    else {
      mapper_render_active_pads();
    }

    mapper_render_play_buttons();
  }
};

struct Controller {
  // (aka 'InControl') Allows customizing pads behaviour
  char extended_mode = 0;
  char selected_synth = FM;

  Mapper1 mapper1;
  Mapper2 mapper2;

  void controller_enable_extended_mode() {
    uint8_t msg[3] = {CHANNEL1_NOTE_ON, 12, 0};

    // TODO: Status check
    /* uint8_t msg[3] = {CHANNEL1_NOTE_ON, 11, 0};
    Midi.SendData(msg, 1);
    delay(1);
    MIDI_poll(); */

    // Off
    Usb.Task();
    Midi.SendData(msg, 1);
    delay(1);

    delay(100);

    // On
    msg[2] = 127;
    Usb.Task();
    Midi.SendData(msg, 1);
    delay(1);

    extended_mode = 1;
    SERIAL_MONITOR.println("Extended mode enabled");
  }

  void controller_listen() {
    uint8_t msg[3];
    char size;

    char pad_index = -1;
    static char knob_address = 0;
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
          knob_address = 0;
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

          if (pad_index > -1) {
            if (selected_synth == FM) {
              mapper1.mapper_pad_pressed(pad_index);
            }
            else if (selected_synth == SAMPLE) {
              mapper2.mapper_pad_pressed(pad_index);
            }
          }

          // Identify buttons
          if (msg[1] == TOP_PLAY) {
            if (selected_synth == SAMPLE) {
              mapper2.mapper_top_play_button_pressed();
            }
          }

          else if (msg[1] == BOTTOM_PLAY) {
            if (selected_synth == SAMPLE) {
              mapper2.mapper_bottom_play_button_pressed();
            }
          }
        }
        // Keyboard
        else {
          MIDI_SERIAL_PORT_1.write(msg, size);
        }
      }
      else if (msg[0] == CHANNEL1_NOTE_OFF) {
        if (msg[1] >= PAD1 and msg[1] <= PAD8 or msg[1] >= PAD9 and msg[1] <= PAD16
          or msg[1] == TOP_PLAY or msg[1] == BOTTOM_PLAY) {
          knob_address = 0;
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

          if (pad_index > -1) {
            if (selected_synth == FM) {
              mapper1.mapper_pad_released(pad_index);
            }
          }

          // Identify buttons
          if (msg[1] == TOP_PLAY) {
            if (selected_synth == SAMPLE) {
              mapper2.mapper_top_play_button_released();
            }
          }

          else if (msg[1] == BOTTOM_PLAY) {
            if (selected_synth == SAMPLE) {
              mapper2.mapper_bottom_play_button_released();
            }
          }
        }
        // Keyboard
        else {}
      }
      else if (msg[0] == CHANNEL1_CONTROL_CHANGE) {
        non_empty_message_flg = 1;
        // Knobs
        if (msg[1] >= KNOB1 and msg[1] <= KNOB8) {
          knob_address = msg[1];
          knob_value = msg[2];
        }
        // Other buttons
        else if (msg[1] == TRACK_LEFT and msg[2] == 127) {
          SERIAL_MONITOR.println("FM selected");
          selected_synth = FM;
          mapper1.mapper_render();
        }
        else if (msg[1] == TRACK_RIGHT and msg[2] == 127) {
          SERIAL_MONITOR.println("SAMPLE selected");
          selected_synth = SAMPLE;
          mapper2.mapper_render();
        }
        else if (msg[1] = SCENE_UP and msg[2] == 127) {}
        else if (msg[1] = SCENE_DOWN and msg[2] == 127) {}
      }
    }
    else {
      if (non_empty_message_flg == 1) {
        if (knob_address and knob_value) {
          if (selected_synth == FM) {
            mapper1.mapper_knob_rotated(knob_address, knob_value);
          }
        }
        non_empty_message_flg = 0;
      }
    }
    delay(1);
  }
};

Controller launchkey_mini;

void setup() {
  SERIAL_MONITOR.begin(9600);
  MIDI_SERIAL_PORT_1.begin(31250);
  MIDI_SERIAL_PORT_2.begin(31250);

  SERIAL_MONITOR.println("Checking USB...");
  if (Usb.Init() == -1) {
    while (1); //halt
  }//if (Usb.Init() == -1...
  delay( 200 );

  SERIAL_MONITOR.println("SUCCESS");
}

void loop()
{
  if (launchkey_mini.extended_mode == 0) {
    Usb.Task();
    if ( Usb.getUsbTaskState() == USB_STATE_RUNNING ) {
      launchkey_mini.controller_enable_extended_mode();
      SERIAL_MONITOR.println("Controller is ready to use");
    }
  }
  else {
    launchkey_mini.controller_listen();
  }
}
