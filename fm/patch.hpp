#ifndef PATCH_HPP
#define PATCH_HPP
#include "definitions.hpp"

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

  void setValue(char value) {
    previous_value = current_value;
    current_value = value;
  }

  void restorePreviousValue() {
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
    {"olvl", 99}, {"fixd", 1, 1}, {"fcrs", 31}, {"ffne", 99},
    {"detn", 14}
  };

  void setParameterValue(char param_index, char value) {
    parameters[param_index].previous_value = parameters[param_index].current_value;
    parameters[param_index].current_value = parameters[param_index].max_value * value * 1.0/ 128.0;

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "%d %4s %2i", index, parameters[param_index].display_code, parameters[param_index].current_value);

    // SERIAL_MONITOR.println(last_modified_parameter_code);
  }

  void turnOn() {
      power = 1;
      parameters[OUTPUT_LEVEL].restorePreviousValue();
      parameters[OSCILLATOR_MODE].setValue(0);
      parameters[OSCILLATOR_FREQUENCY_COARSE].restorePreviousValue();
      parameters[OSCILLATOR_FREQUENCY_FINE].restorePreviousValue();
      parameters[DETUNE].restorePreviousValue();

      modified_parameter = 1;
      sprintf(last_modified_parameter_code, "%d on", index);

      // SERIAL_MONITOR.println(last_modified_parameter_code);

    }

  void turnOff() {
      power = 0;
      parameters[OUTPUT_LEVEL].setValue(0);
      parameters[OSCILLATOR_MODE].setValue(1);
      parameters[OSCILLATOR_FREQUENCY_COARSE].setValue(0);
      parameters[OSCILLATOR_FREQUENCY_FINE].setValue(0);
      parameters[DETUNE].setValue(0);

      modified_parameter = 1;
      sprintf(last_modified_parameter_code, "%d off", index);

      //SERIAL_MONITOR.println(last_modified_parameter_code);
    }

  void getParameterValues() {
    // SERIAL_MONITOR.println("Operator: ");

    for (char i = 0; i < 21; i++) {
      MIDI_SERIAL_PORT_1.write(parameters[i].current_value);
      // SERIAL_MONITOR.print(parameters[i].current_value, DEC);
      // SERIAL_MONITOR.print(" ");
    }
    // SERIAL_MONITOR.println();
  }

  char* getLastModifiedParam() {
    modified_parameter = 0;
    return last_modified_parameter_code;
  }

  void getParameterValue(char param_index) {

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

  void setParameterValue(char param_index, char value) {
    parameters[param_index].previous_value = parameters[param_index].current_value;
    parameters[param_index].current_value = parameters[param_index].max_value * value / 128.0;;

    modified_parameter = 1;
    sprintf(last_modified_parameter_code, "A %4s %2i", parameters[param_index].display_code, parameters[param_index].current_value);

    // SERIAL_MONITOR.println(last_modified_parameter_code);
  }

  void getParameterValues() {
    for (char i = 0; i < 19; i++) {
      MIDI_SERIAL_PORT_1.write(parameters[i].current_value);
    }
  }

  char* getLastModifiedParam() {
    modified_parameter = 0;
    return last_modified_parameter_code;
  }
};

struct Patch {

  Operator operators[5] = {{6}, {5}, {4}, {3}, {2}};
  All all;

  void beginSysex() {
    uint8_t patch_start[6] = {0xf0, 0x43, 0x00, 0x00, 0x01, 0x1b};
    for (char i = 0; i < 6; i++) {
      MIDI_SERIAL_PORT_1.write(patch_start[i]);
    }
  }

  void endSysex() {
    MIDI_SERIAL_PORT_1.write(0xf7);
  }

  void sendPatchData(char *display_message=0) {
    char patch_name[DISPLAY_CODE_LENGTH];

    if (display_message != 0) {
      strncpy(patch_name, display_message, DISPLAY_CODE_LENGTH);
    }
    else {
      for (char i = 0; i < 6; i++) {
        if (operators[i].modified_parameter == 1) {
          strncpy(patch_name, operators[i].getLastModifiedParam(), DISPLAY_CODE_LENGTH);
          break;
        }
      }

      if (all.modified_parameter == 1) {
        strncpy(patch_name, all.getLastModifiedParam(), DISPLAY_CODE_LENGTH);
      }
    }

    for (char i = 0; i < 5; i++) {
      operators[i].getParameterValues();
    }
    operators[4].getParameterValues();

    all.getParameterValues();

    // Looking for a parameter that was changed and sending short descripltion
    // instead of patch name
    // SERIAL_MONITOR.println(patch_name);

    for (char i = 0; i < 10; i++) {
      MIDI_SERIAL_PORT_1.write(patch_name[i]);
    }

    // Collecting operator ON/OFF status
    char operators_power_status = operators[OP2].power + 2 * operators[OP2].power +
      4 * operators[OP3].power + 8 * operators[OP4].power + 16 * operators[OP5].power +
      32 * operators[OP6].power;

    MIDI_SERIAL_PORT_1.write(operators_power_status);
  }

  void sendSysexMessage(char *display_message=0) {
    beginSysex();

    sendPatchData(display_message);

    endSysex();
  }

  void showParameterValue(char patch_subentity, char param_index) {
    beginSysex();

    char parameter_value_text[DISPLAY_CODE_LENGTH];

    if (patch_subentity == -1) {
      sprintf(parameter_value_text, "A %4s %2i",
        all.parameters[param_index].display_code, all.parameters[param_index].current_value);
    }
    else {
      sprintf(parameter_value_text, "%d %4s %2i",
        operators[patch_subentity].index,
        operators[patch_subentity].parameters[param_index].display_code,
        operators[patch_subentity].parameters[param_index].current_value
      );
    }

    // SERIAL_MONITOR.print("Showing following text: ");
    // SERIAL_MONITOR.println(parameter_value_text);

    sendPatchData(parameter_value_text);

    endSysex();
  }
};

#endif // PATCH_HPP
