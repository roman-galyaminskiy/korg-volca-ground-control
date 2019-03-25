#include <usbh_midi.h>
// #include <usbhub.h>
// #include <SPI.h>

USB Usb;
USBH_MIDI  Midi(&Usb);

// #ifdef USBCON
// #define _MIDI_SERIAL_PORT Serial1
// #else
// #define _MIDI_SERIAL_PORT Serial
// #endif

#define _MIDI_SERIAL_PORT Serial1
//////////////////////////
// MIDI Pin assign
// 2 : GND
// 4 : +5V(Vcc) with 220ohm
// 5 : TX
//////////////////////////

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

#define KNOB1 21
#define KNOB2 22
#define KNOB3 23
#define KNOB4 24
#define KNOB5 25
#define KNOB6 26
#define KNOB7 27
#define KNOB8 28

#define KNOB_MODE_ALL -1

#define BLACK 0
#define RED 15
#define ORANGE 19
#define YELLOW 21
#define GREEN 24

#define DISPLAY_CODE_LENGTH 10

#define OP1 5
#define OP2 4
#define OP3 3
#define OP4 2
#define OP5 1
#define OP6 0

#define EG_RATE_1 0
#define EG_RATE_3 2
#define EG_RATE_2 1
#define EG_RATE_4 3
#define EG_LEVEL_1 4
#define EG_LEVEL_2 5
#define EG_LEVEL_3 6
#define EG_LEVEL_4 7
#define KEYBOARD_LEVEL_SCALE_BREAK_POINT 8
#define KEYBOARD_LEVEL_SCALE_LEFT_DEPTH 9
#define KEYBOARD_LEVEL_SCALE_RIGHT_DEPTH 10
#define KEYBOARD_LEVEL_SCALE_LEFT_CURVE 11
#define KEYBOARD_LEVEL_SCALE_RIGHT_CURVE 12
#define KEYBOARD_RATE_SCALING 13
#define MODULATION_SENSITIVITY_AMPLITUDE 14
#define OPERATOR_KEY_VELOCITY_SENSITIVITY 15
#define OPERATOR_OUTPUT_LEVEL 16
#define OSCILLATOR_MODE 17
#define OSCILLATOR_FREQUENCY_COARSE 18
#define OSCILLATOR_FREQUENCY_FINE 19
#define DETUNE 20

#define PITCH_EG_RATE_1 0
#define PITCH_EG_RATE_2 1
#define PITCH_EG_RATE_3 2
#define PITCH_EG_RATE_4 3
#define PITCH_EG_LEVEL_1 4
#define PITCH_EG_LEVEL_2 5
#define PITCH_EG_LEVEL_3 6
#define PITCH_EG_LEVEL_4 7
#define ALGORITHM 8
#define FEEDBACK 9
#define OSCILLATOR_SYNC 10
#define LFO_SPEED 11
#define LFO_DELAY 12
#define LFO_PITCH_MODULATION_DEPTH 13
#define LFO_AMPLITUDE_MODULATION_DEPTH 14
#define LFO_SYNC 15
#define LFO_WAVE 16
#define MODULATION_SENSITIVITY_PITCH 17
#define TRANSPOSE 18

#define MODULATOR_ATTACK 0
#define MODULATOR_DECAY 1
#define CARRIER_ATTACK 2
#define CARRIER_DECAY 3
#define LFO_RATE 4
#define LFO_PITCH_DEPTH 5
#define ALGTM 6

#define MODULATOR_ATTACK_P 42
#define MODULATOR_DECAY_P 43
#define CARRIER_ATTACK_P 44
#define CARRIER_DECAY_P 45
#define LFO_RATE_P 46
#define LFO_PITCH_DEPT_P 47
#define ALGTM_P 48

struct Param {
  const char* display_code;
  char max_value;
  char current_value;
  char previous_value;

  Param (const char* a, char b) {
    display_code = a;
    max_value = b;
    current_value = 0;
    previous_value = 0;
  }

  Param (const char* a, char b, char c) {
    display_code = a;
    max_value = b;
    current_value = c;
    previous_value = c;
  }

  Param (const char* a, char b, char c, char d) {
    display_code = a;
    max_value = b;
    current_value = c;
    previous_value = d;
  }

  void parameter_set_value(char value) {
    previous_value = current_value;
    // current_value = max_value * knob_value >> 7;
    current_value = value;
  }

  void parameter_restore_previous_value() {
    char buffer = current_value;
    current_value = previous_value;
    previous_value = buffer;
  }

};

struct AdditionalParam {
  char index;
  char current_value;
  char mapping;

  AdditionalParam (char a, char b, char c) {
    index = a;
    current_value = b;
    mapping = c;
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
  // OPN XXXX NN

  Param parameters[21] = {
    {"egr1", 99}, {"egr2", 99}, {"egr3", 99}, {"egr4", 99},
    {"egl1", 99}, {"egl2", 99}, {"egl3", 99}, {"egl4", 99},
    {"lsbp", 99}, {"lsld", 99}, {"lsrd", 99}, {"lslc", 3},
    {"lslr", 3}, {"krs ", 7}, {"ams ", 7}, {"kvs", 7},
    {"olvl", 99}, {"fixd", 1}, {"fcrs", 31}, {"ffne", 99},
    {"detn", 1}
  };

  void operator_set_param_value(char param_index, char value) {
    parameters[param_index].previous_value = parameters[param_index].current_value;
    parameters[param_index].current_value = parameters[param_index].max_value * value >> 7;

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "%d %4s %2i", index, parameters[param_index].display_code, parameters[param_index].current_value);

    Serial.println(last_modified_parameter_code);
  }

  void operator_power_on() {
      power = 1;
      parameters[OPERATOR_OUTPUT_LEVEL].parameter_set_value(99);
      parameters[OSCILLATOR_MODE].parameter_set_value(0);
      parameters[OSCILLATOR_FREQUENCY_COARSE].parameter_set_value(0);
      parameters[OSCILLATOR_FREQUENCY_FINE].parameter_set_value(0);
      parameters[DETUNE].parameter_set_value(0);

      modified_parameter = 1;
      sprintf(last_modified_parameter_code, "%d on", index);

      Serial.println(last_modified_parameter_code);

    }

  void operator_power_off() {
      power = 0;
      parameters[OPERATOR_OUTPUT_LEVEL].parameter_set_value(0);
      parameters[OSCILLATOR_MODE].parameter_set_value(1);
      parameters[OSCILLATOR_FREQUENCY_COARSE].parameter_set_value(0);
      parameters[OSCILLATOR_FREQUENCY_FINE].parameter_set_value(0);
      parameters[DETUNE].parameter_set_value(0);

      modified_parameter = 1;
      sprintf(last_modified_parameter_code, "%d off", index);

      Serial.println(last_modified_parameter_code);
    }

  void oscillator_fixed_mode() {
    parameters[OSCILLATOR_MODE].parameter_set_value(1);

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "%d fixed", index);

    Serial.println(last_modified_parameter_code);
  }

  void oscillator_ratio_mode() {
    Serial.println("oscillator_ratio_mode");
    parameters[OSCILLATOR_MODE].parameter_set_value(0);

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "%d ratio", index);

    Serial.println(last_modified_parameter_code);
  }

  void amplitude_modulation_on() {
    parameters[MODULATION_SENSITIVITY_AMPLITUDE].parameter_set_value(7);

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "%d mod on", index);

    Serial.println(last_modified_parameter_code);
  }

  void amplitude_modulation_off() {
    parameters[MODULATION_SENSITIVITY_AMPLITUDE].parameter_set_value(0);

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "%d mod off", index);

    Serial.println(last_modified_parameter_code);
  }

  void enable_bass_envelope() {
    parameters[EG_LEVEL_1].parameter_set_value(99);
    parameters[EG_LEVEL_2].parameter_set_value(0);
    parameters[EG_LEVEL_3].parameter_set_value(0);
    parameters[EG_LEVEL_4].parameter_set_value(0);
    parameters[EG_RATE_1].parameter_set_value(88);
    parameters[EG_RATE_2].parameter_set_value(60);
    parameters[EG_RATE_3].parameter_set_value(99);
    parameters[EG_RATE_4].parameter_set_value(99);

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "%d bass", index);

    Serial.println(last_modified_parameter_code);
  }

  void enable_piano_envelope() {
    parameters[EG_LEVEL_1].parameter_set_value(99);
    parameters[EG_LEVEL_2].parameter_set_value(99);
    parameters[EG_LEVEL_3].parameter_set_value(46);
    parameters[EG_LEVEL_4].parameter_set_value(0);
    parameters[EG_RATE_1].parameter_set_value(95);
    parameters[EG_RATE_2].parameter_set_value(25);
    parameters[EG_RATE_3].parameter_set_value(25);
    parameters[EG_RATE_4].parameter_set_value(67);

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "%d piano", index);

    Serial.println(last_modified_parameter_code);
  }

  void enable_organ_envelope() {
    parameters[EG_LEVEL_1].parameter_set_value(99);
    parameters[EG_LEVEL_2].parameter_set_value(99);
    parameters[EG_LEVEL_3].parameter_set_value(99);
    parameters[EG_LEVEL_4].parameter_set_value(0);
    parameters[EG_RATE_1].parameter_set_value(99);
    parameters[EG_RATE_2].parameter_set_value(80);
    parameters[EG_RATE_3].parameter_set_value(22);
    parameters[EG_RATE_4].parameter_set_value(20);

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "%d organ", index);

    Serial.println(last_modified_parameter_code);
  }

  void enable_strings_envelope() {
    parameters[EG_LEVEL_1].parameter_set_value(99);
    parameters[EG_LEVEL_2].parameter_set_value(78);
    parameters[EG_LEVEL_3].parameter_set_value(41);
    parameters[EG_LEVEL_4].parameter_set_value(0);
    parameters[EG_RATE_1].parameter_set_value(46);
    parameters[EG_RATE_2].parameter_set_value(15);
    parameters[EG_RATE_3].parameter_set_value(23);
    parameters[EG_RATE_4].parameter_set_value(33);

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "%d string", index);

    Serial.println(last_modified_parameter_code);
  }

  void get_parameter_values() {
    // Serial.println("Operator: ");

    for (char i = 0; i < 21; i++) {
      _MIDI_SERIAL_PORT.write(parameters[i].current_value);
      // Serial.print(parameters[i].current_value, DEC);
      // Serial.print(" ");
    }
    // Serial.println();
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
    {"lfod", 99, 0, 0}, {"lpmd", 99, 0, 0}, {"lamd", 99, 0, 0}, {"lks ", 1, 0, 0},
    {"lfow", 5, 0, 0}, {"msp ", 7, 0, 0}, {"trsp", 48, 32, 0}
  };

  void all_set_param_value(char param_index, char value) {
    parameters[param_index].previous_value = parameters[param_index].current_value;
    parameters[param_index].current_value = parameters[param_index].max_value * value / 127.0;

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "A %4s %2i", parameters[param_index].display_code, parameters[param_index].current_value);

    Serial.println(last_modified_parameter_code);
  }

  void get_parameter_values() {
    // Serial.println("All: ");

    for (char i = 0; i < 19; i++) {
      _MIDI_SERIAL_PORT.write(parameters[i].current_value);
      // Serial.print(parameters[i].current_value, DEC);
      // Serial.print(" ");
    }
    // Serial.println();
  }

  char* get_last_modified_param() {
    modified_parameter = 0;
    return last_modified_parameter_code;
  }
};

struct Patch {

  Operator operators[6] = {{6}, {5}, {4}, {3}, {2}, {1}};
  All all;

  AdditionalParam additional_parameters[7] {
    {MODULATOR_ATTACK_P, 63, -1},
    {MODULATOR_DECAY_P, 63, -1},
    {CARRIER_ATTACK_P, 63, -1},
    {CARRIER_DECAY_P, 63, -1},
    {LFO_RATE_P, 63, -1},
    {LFO_PITCH_DEPT_P, 63, -1},
    {ALGTM_P, 63, ALGORITHM}
  };

  void patch_set_external_param_value(char param_index, char value) {
    additional_parameters[param_index].current_value = value;

    if (additional_parameters[param_index].mapping > -1) {
      all.all_set_param_value(ALGORITHM, value);
    }
  }

  void send_sysex() {
    // Serial.println("Witing patch: ");

    // Serial.println("Start: ");
    uint8_t patch_start[6] = {0xf0, 0x43, 0x00, 0x00, 0x01, 0x1b};
    for (char i = 0; i < 6; i++) {
      _MIDI_SERIAL_PORT.write(patch_start[i]);
      // Serial.print(patch_start[i], HEX);
      // Serial.print(" ");
    }
    // Serial.println();

    // Serial.println("Body: ");
    for (char i = 0; i < 6; i++) {
      // Serial.print(i - 1, DEC);
      operators[i].get_parameter_values();
    }

    uint8_t patch_name[10] = "patch_name";

    all.get_parameter_values();

    for (char i = 0; i < 6; i++) {
      // Serial.print(i - 1, DEC);
      if (operators[i].modified_parameter == 1) {
        strncpy(patch_name, operators[i].get_last_modified_param(), DISPLAY_CODE_LENGTH);
        break;
      }
    }

    if (all.modified_parameter == 1) {
      strncpy(patch_name, all.get_last_modified_param(), DISPLAY_CODE_LENGTH);
    }

    for (char i = 0; i < 10; i++) {
      _MIDI_SERIAL_PORT.write(patch_name[i]);
      // Serial.print(patch_end[i], HEX);
      // Serial.print(" ");
    }

    char operators_power_status = operators[OP1].power + 2 * operators[OP2].power +
      3 * operators[OP3].power + 4 * operators[OP4].power;

    _MIDI_SERIAL_PORT.write(operators_power_status);
    _MIDI_SERIAL_PORT.write(0xf7);
  }

  void send_short_message(char param_index) {
    uint8_t msg[3] = {176, additional_parameters[param_index].index, additional_parameters[param_index].current_value};
    _MIDI_SERIAL_PORT.write(msg, 3);
  }

};

Patch patch;

struct Option {
  char color;
  void (Operator::*callback)(); // pointer to function
};

struct Pad {
  char index;
  char operator_index;
  char selected_option;
  Option* options;
  char options_len;

  void pad_select_next_option() {
    // Serial.print("Pad #");
    // Serial.print(index, DEC);
    // Serial.print("Operator #");
    // Serial.println(operator_index);
    if (options_len == 0) {
      return;
    }
    if (selected_option + 1 == options_len) {
      selected_option = 0;
    }
    else {
      selected_option++;
    }

    void (Operator::*callback)() = options[selected_option].callback;
    (patch.operators[operator_index].*callback)();
  }
};

struct Controller {
  // (aka 'InControl') Allows customizing pads behaviour
  char extended_mode = 0;

  char knob_mode = KNOB_MODE_ALL;

  Option operator_power_option[2] = {{GREEN, &Operator::operator_power_on}, {RED, &Operator::operator_power_off}};
  Option oscillator_mode_option[2] = {{GREEN, &Operator::oscillator_fixed_mode}, {RED, &Operator::oscillator_ratio_mode}};
  Option amplitude_modulation_option[2] = {{GREEN, &Operator::amplitude_modulation_on}, {RED, &Operator::amplitude_modulation_off}};
  Option envelope_generator_option[4] = {{RED, &Operator::enable_bass_envelope}, {YELLOW, &Operator::enable_piano_envelope},
    {ORANGE, &Operator::enable_organ_envelope}, {GREEN, &Operator::enable_strings_envelope}};

  Pad pads[16] = {
    {PAD1, OP1, 0, operator_power_option, 2},
    {PAD2, OP1, 1, oscillator_mode_option, 2},
    {PAD3, OP2, 0, operator_power_option, 2},
    {PAD4, OP2, 1, oscillator_mode_option, 2},
    {PAD5, OP3, 0, operator_power_option, 2},
    {PAD6, OP3, 1, oscillator_mode_option, 2},
    {PAD7, OP4, 0, operator_power_option, 2},
    {PAD8, OP4, 1, oscillator_mode_option, 2},
    {PAD9, OP1, 0, amplitude_modulation_option, 2},
    {PAD10, OP1, 0, envelope_generator_option, 4},
    {PAD11, OP2, 0, amplitude_modulation_option, 2},
    {PAD12, OP2, 0, envelope_generator_option, 4},
    {PAD13, OP3, 0, amplitude_modulation_option, 2},
    {PAD14, OP3, 0, envelope_generator_option, 4},
    {PAD15, OP4, 0, amplitude_modulation_option, 2},
    {PAD16, OP4, 0, envelope_generator_option, 4}
  };

  void controller_enable_extended_mode() {
    uint8_t msg[3] = {144, 12, 0};

    // TODO: Status check
    /* uint8_t msg[3] = {144, 11, 0};
    Midi.SendData(msg, 1);
    delay(1);
    MIDI_poll(); */

    // Off
    Midi.SendData(msg, 1);
    // On
    delay(100);
    msg[2] = 127;
    Midi.SendData(msg, 1);

    extended_mode = 1;
    Serial.println("Extended mode enabled");
  }

  void controller_render_play_buttons() {
    // 'note_on' message templape
    uint8_t top_play_msg[3] = {144, TOP_PLAY, 0};
    uint8_t bottom_play_msg[3] = {144, BOTTOM_PLAY, 0};

    if (knob_mode > KNOB_MODE_ALL ) {
      top_play_msg[2] = RED;
    }

    Usb.Task();
    Midi.SendData(top_play_msg, 1);
    delay(1);
  }

  void controller_render_pads() {
    // 'note_on' message templape
    uint8_t msg[3] = {144, 0, 0};

    for (char i = 0; i < 16; i++) {
      msg[1] = pads[i].index;

      if (knob_mode == i) {
        msg[2] = BLACK;
      }
      else {
        msg[2] = pads[i].options[pads[i].selected_option].color;
      }

      Usb.Task();
      Midi.SendData(msg, 1);
      delay(1);
    }
  }

  void controller_render_deck() {
    controller_render_pads();
    controller_render_play_buttons();
  }

};

Controller lk;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  _MIDI_SERIAL_PORT.begin(31250);

  Serial.println("Checking USB...");
  if (Usb.Init() == -1) {
    while (1); //halt
  }//if (Usb.Init() == -1...
  delay( 200 );

  Serial.println("SUCCESS");

  lk.extended_mode = 0;
}

char top_play_button_pressed = 0;
char pad_pressed = -1;
char skip_pad_press = 0;

void loop() {
  if (lk.extended_mode == 0) {
    Usb.Task();
    if ( Usb.getUsbTaskState() == USB_STATE_RUNNING ) {
      lk.controller_enable_extended_mode();
      lk.controller_render_deck();

      Serial.write("Send initial patch");
      patch.send_sysex();
    }
  }

  Usb.Task();
  MIDI_poll();

  // put your main code here, to run repeatedly:
  delay(10);
}

// Poll USB MIDI Controler and send to serial MIDI
void MIDI_poll()
{
  extern char top_play_button_pressed;
  extern char pad_pressed;
  extern char skip_pad_press;

  uint8_t outBuf[ 3 ];
  uint8_t size;

  char pad_index;

  do {
    if ( (size = Midi.RecvData(outBuf)) > 0 ) {
      Serial.println(size, DEC);
       Serial.print(outBuf[0]);
       Serial.print(" ");
       Serial.print(outBuf[1]);
       Serial.print(" ");
       Serial.println(outBuf[2]);

      // channel 1 note_on
      if (outBuf[0] == 144) {

        if (outBuf[1] >= PAD1 and outBuf[1] <= PAD8 or outBuf[1] >= PAD9 and outBuf[1] <= PAD16
          or outBuf[1] == TOP_PLAY or outBuf[1] == BOTTOM_PLAY) {

          // Identify pad
          if (outBuf[1] >= PAD1 and outBuf[1] <= PAD8) {
              pad_index = outBuf[1] - PAD1;
          }
          else if (outBuf[1] >= PAD9 and outBuf[1] <= PAD16){
              pad_index = 8 + outBuf[1] - PAD9;
          }
          else {
              pad_index = -1;
          }

          if (pad_index > -1) {
            pad_pressed = outBuf[1];

            if (top_play_button_pressed > 0) {
              // Serial.print("knob_mode ");
              Serial.print(pad_index, DEC);
              Serial.println();
              lk.knob_mode = pad_index;
            }

            lk.controller_render_deck();
          }

          // Identify button
          if (outBuf[1] == TOP_PLAY) {
            Serial.println("TOP_PLAY");
            top_play_button_pressed = 1;

            if (lk.knob_mode != KNOB_MODE_ALL) {
              lk.knob_mode = KNOB_MODE_ALL;
            }

            lk.controller_render_deck();
          }

          else if (outBuf[1] == BOTTOM_PLAY) {
            // Serial.println("BOTTOM_PLAY pressed");
          }
        }
        else {
          _MIDI_SERIAL_PORT.write(outBuf, size);
        }
      }

      else if (outBuf[0] == 128) {
        if (outBuf[1] >= PAD1 and outBuf[1] <= PAD8 or outBuf[1] >= PAD9 and outBuf[1] <= PAD16
          or outBuf[1] == TOP_PLAY or outBuf[1] == BOTTOM_PLAY) {

          // Identify pad
          if (outBuf[1] >= PAD1 and outBuf[1] <= PAD8) {
              pad_index = outBuf[1] - PAD1;
          }
          else if (outBuf[1] >= PAD9 and outBuf[1] <= PAD16){
              pad_index = 8 + outBuf[1] - PAD9;
          }
          else {
              pad_index = -1;
          }

          if (pad_index > -1 and pad_pressed == outBuf[1]) {
            if (skip_pad_press == 0) {
              lk.pads[pad_index].pad_select_next_option();
              patch.send_sysex();
              lk.controller_render_deck();
            }

            else {
              skip_pad_press = 0;
            }

            pad_pressed = -1;
          }

          // Identify button
          if (outBuf[1] == TOP_PLAY) {
            Serial.println("TOP_PLAY");
            top_play_button_pressed = 0;

            lk.controller_render_pads();
          }
        }
      }

      else if (outBuf[0] == 176) {
        if (outBuf[1] >= KNOB1 and outBuf[1] <= KNOB8) {
          if (pad_pressed > -1) {
            skip_pad_press = 1;
            // OP1
            if (outBuf[1] == KNOB1 and pad_pressed == PAD1) {
              patch.operators[OP1].operator_set_param_value(OPERATOR_OUTPUT_LEVEL, outBuf[2]);
              patch.send_sysex();
            }

            else if (outBuf[1] == KNOB2 and pad_pressed == PAD1) {
              patch.operators[OP1].operator_set_param_value(OPERATOR_KEY_VELOCITY_SENSITIVITY, outBuf[2]);
              patch.send_sysex();
            }

            else if (outBuf[1] == KNOB1 and pad_pressed == PAD2) {
              patch.operators[OP1].operator_set_param_value(OSCILLATOR_FREQUENCY_COARSE, outBuf[2]);
              patch.send_sysex();
            }

            else if (outBuf[1] == KNOB2 and pad_pressed == PAD2) {
              patch.operators[OP1].operator_set_param_value(OSCILLATOR_FREQUENCY_FINE, outBuf[2]);
              patch.send_sysex();
            }

            // OP2
            else if (outBuf[1] == KNOB3 and pad_pressed == PAD3) {
              patch.operators[OP2].operator_set_param_value(OPERATOR_OUTPUT_LEVEL, outBuf[2]);
              patch.send_sysex();
            }

            else if (outBuf[1] == KNOB4 and pad_pressed == PAD3) {
              patch.operators[OP2].operator_set_param_value(OPERATOR_KEY_VELOCITY_SENSITIVITY, outBuf[2]);
              patch.send_sysex();
            }

            else if (outBuf[1] == KNOB3 and pad_pressed == PAD4) {
              patch.operators[OP2].operator_set_param_value(OSCILLATOR_FREQUENCY_COARSE, outBuf[2]);
              patch.send_sysex();
            }
            
             else if (outBuf[1] == KNOB4 and pad_pressed == PAD4) {
               patch.operators[OP2].operator_set_param_value(OSCILLATOR_FREQUENCY_FINE, outBuf[2]);
               patch.send_sysex();
             }

             // OP3
             else if (outBuf[1] == KNOB5 and pad_pressed == PAD5) {
               patch.operators[OP3].operator_set_param_value(OPERATOR_OUTPUT_LEVEL, outBuf[2]);
               patch.send_sysex();
             }
            
             else if (outBuf[1] == KNOB6 and pad_pressed == PAD5) {
               patch.operators[OP3].operator_set_param_value(OPERATOR_KEY_VELOCITY_SENSITIVITY, outBuf[2]);
               patch.send_sysex();
             }
            
             else if (outBuf[1] == KNOB5 and pad_pressed == PAD6) {
               patch.operators[OP3].operator_set_param_value(OSCILLATOR_FREQUENCY_COARSE, outBuf[2]);
               patch.send_sysex();
             }
            
             else if (outBuf[1] == KNOB6 and pad_pressed == PAD6) {
               patch.operators[OP3].operator_set_param_value(OSCILLATOR_FREQUENCY_FINE, outBuf[2]);
               patch.send_sysex();
             }
            
             // OP4
             else if (outBuf[1] == KNOB7 and pad_pressed == PAD7) {
               patch.operators[OP4].operator_set_param_value(OPERATOR_OUTPUT_LEVEL, outBuf[2]);
               patch.send_sysex();
             }
            
             else if (outBuf[1] == KNOB8 and pad_pressed == PAD7) {
               patch.operators[OP4].operator_set_param_value(OPERATOR_KEY_VELOCITY_SENSITIVITY, outBuf[2]);
               patch.send_sysex();
             }
            
             else if (outBuf[1] == KNOB7 and pad_pressed == PAD8) {
               patch.operators[OP4].operator_set_param_value(OSCILLATOR_FREQUENCY_COARSE, outBuf[2]);
               patch.send_sysex();
             }
            
             else if (outBuf[1] == KNOB8 and pad_pressed == PAD8) {
               patch.operators[OP4].operator_set_param_value(OSCILLATOR_FREQUENCY_FINE, outBuf[2]);
               patch.send_sysex();
             }
          }

          else {
            if (outBuf[1] == KNOB1) {
              patch.patch_set_external_param_value(CARRIER_DECAY, outBuf[2]);
              patch.send_short_message(CARRIER_DECAY);
            }
            else if (outBuf[1] == KNOB2) {
              patch.patch_set_external_param_value(CARRIER_ATTACK, outBuf[2]);
              patch.send_short_message(CARRIER_ATTACK);
            }
            else if (outBuf[1] == KNOB3) {
              patch.patch_set_external_param_value(MODULATOR_DECAY, outBuf[2]);
              patch.send_short_message(MODULATOR_DECAY);
            }
            else if (outBuf[1] == KNOB4) {
              patch.patch_set_external_param_value(MODULATOR_ATTACK, outBuf[2]);
              patch.send_short_message(MODULATOR_ATTACK);
            }
            else if (outBuf[1] == KNOB5) {
              patch.patch_set_external_param_value(LFO_RATE, outBuf[2]);
              patch.send_short_message(LFO_RATE);
            }
            else if (outBuf[1] == KNOB6) {
              patch.patch_set_external_param_value(LFO_PITCH_DEPTH, outBuf[2]);
              patch.send_short_message(LFO_PITCH_DEPTH);
            }
            else if (outBuf[1] == KNOB7) {
              patch.patch_set_external_param_value(ALGTM, outBuf[2]);
              patch.send_short_message(ALGTM);
            }
          }
        }

      }

    }

  } while (size > 0);
}
