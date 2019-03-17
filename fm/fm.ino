#include <usbh_midi.h>
#include <usbhub.h>
#include <SPI.h>

USB Usb;
USBH_MIDI  Midi(&Usb);

// #ifdef USBCON
// #define _MIDI_SERIAL_PORT Serial1
// #else
// #define _MIDI_SERIAL_PORT Serial
// #endif
#define _MIDI_SERIAL_PORT Serial1

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

#define BLACK 0
#define RED 15
#define ORANGE 19
#define YELLOW 21
#define GREEN 24

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

struct Param {
  const char* display_code;
  char max_value;
  char current_value;
  char previous_value;

  Param (const char* display_code, char max_value) {
    display_code = display_code;
    max_value = max_value;
    current_value = 0;
    previous_value = 0;
  }

  Param (const char* display_code, char max_value, char current_value) {
    display_code = display_code;
    max_value = max_value;
    current_value = current_value;
    previous_value = current_value;
  }

  Param (const char* display_code, char max_value, char current_value, char previous_value) {
    display_code = display_code;
    max_value = max_value;
    current_value = current_value;
    previous_value = previous_value;
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

struct Operator {

  char power = 0;

  Param parameters[21] = {
    {"egr1", 99}, {"egr2", 99}, {"egr3", 99}, {"egr4", 99},
    {"egl1", 99}, {"egl2", 99}, {"egl3", 99}, {"egl4", 99},
    {"lsbp", 99}, {"lsld", 99}, {"lsrd", 99}, {"lslc", 3},
    {"lslr", 3}, {"krs ", 7}, {"ams ", 7}, {"kvs", 7},
    {"olvl", 99}, {"fixd", 1}, {"fcrs", 31}, {"ffne", 99},
    {"detn", 1}
  };

  void operator_power_on() {
      power = 1;
      parameters[OPERATOR_OUTPUT_LEVEL].parameter_set_value(99);
      parameters[OSCILLATOR_MODE].parameter_set_value(0);
      parameters[OSCILLATOR_FREQUENCY_COARSE].parameter_set_value(0);
      parameters[OSCILLATOR_FREQUENCY_FINE].parameter_set_value(0);
      parameters[DETUNE].parameter_set_value(0);
    }

  void operator_power_off() {
      power = 1;
      parameters[OPERATOR_OUTPUT_LEVEL].parameter_set_value(0);
      parameters[OSCILLATOR_MODE].parameter_set_value(1);
      parameters[OSCILLATOR_FREQUENCY_COARSE].parameter_set_value(0);
      parameters[OSCILLATOR_FREQUENCY_FINE].parameter_set_value(0);
      parameters[DETUNE].parameter_set_value(0);
    }

  void oscillator_fixed_mode() {
    parameters[OSCILLATOR_MODE].parameter_set_value(1);
  }

  void oscillator_ratio_mode() {
    parameters[OSCILLATOR_MODE].parameter_set_value(0);
  }

  void amplitude_modulation_on() {
    parameters[MODULATION_SENSITIVITY_AMPLITUDE].parameter_set_value(7);
  }

  void amplitude_modulation_off() {
    parameters[MODULATION_SENSITIVITY_AMPLITUDE].parameter_set_value(0);
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
  }

  void get_parameter_values() {

    for (char i = 0; i < 21; i++) {
      Serial.print(parameters[i].current_value, DEC);
      Serial.print(" ");
    }
    Serial.println();
  }

};

struct All {
  Param parameters[19] = {
    {"ptr1", 99, 99, 0}, {"ptr2", 99, 99, 0}, {"ptr3", 99, 99, 0}, {"ptr4", 99, 99, 0},
    {"ptl1", 99, 50, 0}, {"ptl2", 99, 50, 0}, {"ptl3", 99, 50, 0}, {"ptl4", 99, 50, 0},
    {"algo", 31, 0, 0}, {"feed", 7, 0, 0}, {"oks ", 1, 0, 0}, {"lfor", 99, 0, 0},
    {"lfod", 99, 0, 0}, {"lpmd", 99, 0, 0}, {"lamd", 99, 0, 0}, {"lks ", 1, 0, 0},
    {"lfow", 5, 0, 0}, {"msp ", 7, 0, 0}, {"trsp", 31, 12, 0}
  };

  void get_parameter_values() {

    for (char i = 0; i < 19; i++) {
      Serial.print(parameters[i].current_value, DEC);
      Serial.print(" ");
    }
    Serial.println();
  }
};

struct Patch {

  Operator operators[6] = {{}, {}, {}, {}, {}, {}};
  All all;

  void send_patch() {
    Serial.println("Writing patch: ");

    Serial.println("Start: ");
    uint8_t patch_start[6] = {0xf0, 0x43, 0x00, 0x00, 0x01, 0x1b};
    for (char i = 0; i < 6; i++) {
      _MIDI_SERIAL_PORT.write(patch_start[i]);
      Serial.print(patch_start[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    Serial.println("Body: ");
    for (char i = 6; i > 0; i--) {
      Serial.print(i - 1, DEC);
      operators[i - 1].get_parameter_values();
    }

    all.get_parameter_values();

    Serial.println("End: ");
    // uint8_t patch_end[12] = {0x63, 0x68, 0x65, 0x63, 0x6b, 0x31, 0x31, 0x33, 0x34, 0x34, 7, 0xf7};
    uint8_t patch_end[12] = {0x73, 0x79, 0x6e, 0x74, 0x68, 0x6d, 0x61, 0x74, 0x61, 0x3f, 7, 0xf7};
    for (char i = 0; i < 12; i++) {
      _MIDI_SERIAL_PORT.write(patch_start[i]);
      Serial.print(patch_start[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
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
    Serial.print("Pad #");
    Serial.print(index, DEC);
    Serial.print("Operator #");
    Serial.println(operator_index);
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

  Option operator_power_option[2] = {{GREEN, &Operator::operator_power_on}, {RED, &Operator::operator_power_on}};
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
  // Pad pads[16] = {
  //   {PAD1, OP1, 0, {operator_power_on, operator_power_off}, 2},
    // {PAD2, OP1, 0, {oscillator_fixed_mode, oscillator_ratio_mode}, 2},
    // {PAD9, OP1, 0, {amplitude_modulation_on, amplitude_modulation_off}, 2},
    // {PAD10, OP1, 0, {enable_bass_envelope, enable_piano_envelope,
    //   enable_organ_envelope, enable_strings_envelope}, 4},
    // {PAD3, OP2, 0, {operator_power_on, operator_power_off}, 2},
    // {PAD4, OP2, 0, {oscillator_fixed_mode, oscillator_ratio_mode}, 2},
    // {PAD11, OP2, 0, {amplitude_modulation_on, amplitude_modulation_off}, 2},
    // {PAD12, OP2, 0, {enable_bass_envelope, enable_piano_envelope,
    //   enable_organ_envelope, enable_strings_envelope}, 4},
    // {PAD5, OP3, 0, {operator_power_on, operator_power_off}, 2},
    // {PAD6, OP3, 0, {oscillator_fixed_mode, oscillator_ratio_mode}, 2},
    // {PAD13, OP3, 0, {amplitude_modulation_on, amplitude_modulation_off}, 2},
    // {PAD14, OP3, 0, {enable_bass_envelope, enable_piano_envelope,
    //   enable_organ_envelope, enable_strings_envelope}, 4},
    // {PAD7, OP4, 0, {operator_power_on, operator_power_off}, 2},
    // {PAD8, OP4, 0, {oscillator_fixed_mode, oscillator_ratio_mode}, 2},
    // {PAD15, OP4, 0, {amplitude_modulation_on, amplitude_modulation_off}, 2},
    // {PAD16, OP4, 0, {enable_bass_envelope, enable_piano_envelope,
    //   enable_organ_envelope, enable_strings_envelope}, 4}
  // };

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

  void controller_render_pads() {
    // 'note_on' message templape
    uint8_t msg[3] = {144, 0, 0};

    for (char i = 0; i < 16; i++) {
      msg[1] = pads[i].index;
      msg[2] = pads[i].options[pads[i].selected_option].color;

      Usb.Task();
      Midi.SendData(msg, 1);
      delay(1);
    }
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

void loop() {
  if (lk.extended_mode == 0) {
    Usb.Task();
    if ( Usb.getUsbTaskState() == USB_STATE_RUNNING ) {
      lk.controller_enable_extended_mode();
      lk.controller_render_pads();
    }
  }
  // put your main code here, to run repeatedly:
  Usb.Task();
  MIDI_poll();

  delay(10);
}

// Poll USB MIDI Controler and send to serial MIDI
void MIDI_poll()
{
  uint8_t outBuf[ 3 ];
  uint8_t size;

  char pad_index;

  do {
    if ( (size = Midi.RecvData(outBuf)) > 0 ) {
      // Serial.print(outBuf[0]);
      // Serial.print(" ");
      // Serial.print(outBuf[1]);
      // Serial.print(" ");
      // Serial.println(outBuf[2]);

      // channel 1 note_on
      if (outBuf[0] == 144) {
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
          Serial.print("PAD ");
          Serial.print(pad_index + 1, DEC);
          Serial.println(" pressed");

          lk.pads[pad_index].pad_select_next_option();
        }

        if (outBuf[1] == TOP_PLAY) {
          Serial.println("TOP_PLAY");
        }

        else if (outBuf[1] == BOTTOM_PLAY) {
          Serial.println("BOTTOM_PLAY pressed");
        }
      }

      // channel 1 note_off
      else if (outBuf[0] == 128) {}

      lk.controller_render_pads();
      patch.send_patch();
    }

  } while (size > 0);
}