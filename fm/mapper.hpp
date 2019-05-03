#ifndef MAPPER_HPP
#define MAPPER_HPP

#include "usb.hpp"
#include "definitions.hpp"
#include "patch.hpp"

#define RETURN_PAD_INDEX 8 // PAD9
#define RETURN_PAD_COLOR 19 // ORANGE

#define OPERATOR_GROUPS 3
#define ALL_GROUPS 3
#define MAIN_SCREEN 0
#define PARAM_GROUP_SCREEN 1
#define PARAM_SCREEN 2

class Mapper {
public:
  Patch patch;

  void drawMainScreen() {
    // SERIAL_MONITOR.println("Main screen");
    active_screen_type = MAIN_SCREEN;

    // Drawing top raw
    for (char pad_index = 0; pad_index < 8; pad_index++) {
      // Pads 1-6 represent operators
      if (pad_index < 6) {
        changePadColor(pad_index, YELLOW);
      }
      // The rightmost pad is for 'All' page
      else if (pad_index == 6) {
        changePadColor(pad_index, ORANGE);
      }
      else {
        changePadColor(pad_index, BLACK);
      }
    }
    // Drawing bottom raw
    for (char pad_index = 8; pad_index < 16; pad_index++) {
      if (pad_index == 8) {
        if (patch.operators[OP1].power == 1) {
          changePadColor(pad_index, GREEN);
        }
        else {
          changePadColor(pad_index, RED);
        }
      }
      else if (pad_index == 9) {
        if (patch.operators[OP2].power == 1) {
          changePadColor(pad_index, GREEN);
        }
        else {
          changePadColor(pad_index, RED);
        }
      }
      else if (pad_index == 10) {
        if (patch.operators[OP3].power == 1) {
          changePadColor(pad_index, GREEN);
        }
        else {
          changePadColor(pad_index, RED);
        }
      }
      else if (pad_index == 11) {
        if (patch.operators[OP4].power == 1) {
          changePadColor(pad_index, GREEN);
        }
        else {
          changePadColor(pad_index, RED);
        }
      }
      else if (pad_index == 12) {
        if (patch.operators[OP5].power == 1) {
          changePadColor(pad_index, GREEN);
        }
        else {
          changePadColor(pad_index, RED);
        }
      }
      else if (pad_index == 13) {
        if (patch.operators[OP6].power == 1) {
          changePadColor(pad_index, GREEN);
        }
        else {
          changePadColor(pad_index, RED);
        }
      }
      else {
        changePadColor(pad_index, BLACK);
      }
    }
  }

  void drawGroupScreen() {
    // SERIAL_MONITOR.println("Group screen");
    active_screen_type = PARAM_GROUP_SCREEN;
    active_param_group = -1;

    // Drawing top raw
    for (char pad_index = 0; pad_index < 8; pad_index++) {
      if (editing_operator_index > -1) {
        if (pad_index < OPERATOR_GROUPS) {
          changePadColor(pad_index, YELLOW);
        }
        else {
          changePadColor(pad_index, BLACK);
        }
      }
      if (editing_all_flag == 1) {
        if (pad_index < ALL_GROUPS) {
          changePadColor(pad_index, YELLOW);
        }
        else {
          changePadColor(pad_index, BLACK);
        }
      }
    }
    // Drawing bottom raw
    for (char pad_index = 8; pad_index < 16; pad_index++) {
      if (pad_index == RETURN_PAD_INDEX) {
        changePadColor(pad_index, RETURN_PAD_COLOR);
      }
      else {
        changePadColor(pad_index, BLACK);
      }
    }
  };

  void drawParamScreen(char param_group) {
    // SERIAL_MONITOR.println("Param screen");
    active_screen_type = PARAM_SCREEN;
    active_param_group = param_group;

    // Drawing top raw
    for (char pad_index = 0; pad_index < 8; pad_index++) {
      if (editing_operator_index > -1) {
        if (pad_index < parameter_group_lengths[0][param_group]) {
          changePadColor(pad_index, YELLOW);
        }
        else {
          changePadColor(pad_index, BLACK);
        }
      }
      else if (editing_all_flag == 1) {
        if (pad_index < parameter_group_lengths[1][param_group]) {
          changePadColor(pad_index, YELLOW);
        }
        else {
          changePadColor(pad_index, BLACK);
        }
      }
    }
    // Drawing top raw
    for (char pad_index = 8; pad_index < 16; pad_index++) {
      if (pad_index == RETURN_PAD_INDEX) {
        changePadColor(pad_index, RETURN_PAD_COLOR);
      }
      else {
        changePadColor(pad_index, BLACK);
      }
    }
  }

  void padPressed(char pad_index) {
    // SERIAL_MONITOR.print("Pad pressed ");
    if (active_screen_type == MAIN_SCREEN) {
      // SERIAL_MONITOR.println("from main screen");
      // Processing top row
      if (pad_index >= 0 and pad_index <= 6) {
        if (pad_index == 0) {
          editing_operator_index = OP1;
          editing_all_flag = 0;
        }
        else if (pad_index == 1) {
          editing_operator_index = OP2;
          editing_all_flag = 0;
        }
        else if (pad_index == 2) {
          editing_operator_index = OP3;
          editing_all_flag = 0;
        }
        else if (pad_index == 3) {
          editing_operator_index = OP4;
          editing_all_flag = 0;
        }
        else if (pad_index == 4) {
          editing_operator_index = OP5;
          editing_all_flag = 0;
        }
        else if (pad_index == 5) {
          editing_operator_index = OP6;
          editing_all_flag = 0;
        }
        else if (pad_index == 6) {
          editing_operator_index = -1;
          editing_all_flag = 1;
        }

        if (editing_operator_index > -1 or editing_all_flag == 1) {
          drawGroupScreen();
        }
      }
      // Processing bottom row
      else if (pad_index >= 8 and pad_index <= 13) {
        if (pad_index == 8) {
          if (patch.operators[OP1].power == 0) {
            patch.operators[OP1].turnOn();
          }
          else {
            patch.operators[OP1].turnOff();
          }
        }
        else if (pad_index == 9) {
          if (patch.operators[OP2].power == 0) {
            patch.operators[OP2].turnOn();
          }
          else {
            patch.operators[OP2].turnOff();
          }
        }
        else if (pad_index == 10) {
          if (patch.operators[OP3].power == 0) {
            patch.operators[OP3].turnOn();
          }
          else {
            patch.operators[OP3].turnOff();
          }
        }
        else if (pad_index == 11) {
          if (patch.operators[OP4].power == 0) {
            patch.operators[OP4].turnOn();
          }
          else {
            patch.operators[OP4].turnOff();
          }
        }
        else if (pad_index == 12) {
          if (patch.operators[OP5].power == 0) {
            patch.operators[OP5].turnOn();
          }
          else {
            patch.operators[OP5].turnOff();
          }
        }
        else if (pad_index == 13) {
          if (patch.operators[OP6].power == 0) {
            patch.operators[OP6].turnOn();
          }
          else {
            patch.operators[OP6].turnOff();
          }
        }
        patch.sendSysexMessage();
        // Redraw
        drawMainScreen();
      }
    }
    else if (active_screen_type == PARAM_GROUP_SCREEN) {
      // SERIAL_MONITOR.println("from group screen");
      // Processing top row
      if (editing_operator_index > -1 and pad_index < OPERATOR_GROUPS) {
        drawParamScreen(pad_index);
      }
      // Processing bottom row
      else if (editing_all_flag == 1 and pad_index < ALL_GROUPS) {
        drawParamScreen(pad_index);
      }
      else if (pad_index == RETURN_PAD_INDEX) {
        drawMainScreen();
      }
    }
    else if (active_screen_type == PARAM_SCREEN) {
      // SERIAL_MONITOR.println("from param screen");
      // Processing top row
      if (pad_index >= 0 and pad_index < 8) {
        if (editing_operator_index > -1) {
          if (pad_index < parameter_group_lengths[0][active_param_group]) {
            // SERIAL_MONITOR.print("Show param ");
            // SERIAL_MONITOR.print(pad_index, DEC);
            // SERIAL_MONITOR.print(" from group ");
            // SERIAL_MONITOR.print(active_param_group, DEC);
            // SERIAL_MONITOR.print(" or ");
            // SERIAL_MONITOR.print(parameters[0][active_param_group][pad_index], DEC);
            // SERIAL_MONITOR.print(" for operator ");
            // SERIAL_MONITOR.print(editing_operator_index, DEC);
            // SERIAL_MONITOR.println();
            patch.showParameterValue(editing_operator_index, parameters[0][active_param_group][pad_index]);
          }
        }
        else {
          if (pad_index < parameter_group_lengths[1][active_param_group]) {
            // SERIAL_MONITOR.print("Show param ");
            // SERIAL_MONITOR.print(pad_index, DEC);
            // SERIAL_MONITOR.print(" from group ");
            // SERIAL_MONITOR.print(active_param_group, DEC);
            // SERIAL_MONITOR.print(" or ");
            // SERIAL_MONITOR.print(parameters[0][active_param_group][pad_index], DEC);
            // SERIAL_MONITOR.print(" for all ");
            // SERIAL_MONITOR.print(editing_operator_index, DEC);
            // SERIAL_MONITOR.println();
            patch.showParameterValue(editing_operator_index, parameters[1][active_param_group][pad_index]);
          }
        }
      }
      // Processing bottom row
      else if (pad_index >= 8 and pad_index <= 13) {
        if (pad_index == RETURN_PAD_INDEX) {
          drawGroupScreen();
        }
      }
    }
  }

  void knobRotated(char knob_index, char value) {
    if (active_screen_type == PARAM_SCREEN) {
      if (editing_operator_index > -1) {
        if (knob_index < parameter_group_lengths[0][active_param_group]) {
          // SERIAL_MONITOR.print("Edit param ");
          // SERIAL_MONITOR.print(knob_index, DEC);
          // SERIAL_MONITOR.print(" from group ");
          // SERIAL_MONITOR.print(active_param_group, DEC);
          // SERIAL_MONITOR.print(" or ");
          // SERIAL_MONITOR.print(parameters[0][active_param_group][knob_index], DEC);
          // SERIAL_MONITOR.print(" for operator ");
          // SERIAL_MONITOR.print(editing_operator_index, DEC);
          // SERIAL_MONITOR.println();
          patch.operators[editing_operator_index].setParameterValue(parameters[0][active_param_group][knob_index], value);
          patch.sendSysexMessage();
        }
      }
      else {
        if (knob_index < parameter_group_lengths[1][active_param_group]) {
          // SERIAL_MONITOR.print("Edit param ");
          // SERIAL_MONITOR.print(knob_index, DEC);
          // SERIAL_MONITOR.print(" from group ");
          // SERIAL_MONITOR.print(active_param_group, DEC);
          // SERIAL_MONITOR.print(" or ");
          // SERIAL_MONITOR.print(parameters[1][active_param_group][knob_index], DEC);
          // SERIAL_MONITOR.println(" for all");
          patch.all.setParameterValue(parameters[1][active_param_group][knob_index], value);
          patch.sendSysexMessage();
        }
      }
    }
  }

  void trackChanged(char direction) {
    char display_message[DISPLAY_CODE_LENGTH];

    if (active_screen_type == PARAM_GROUP_SCREEN or active_screen_type == PARAM_SCREEN) {
      if (direction == 1) {
        if (editing_operator_index > -1 and editing_operator_index > OP6) {
          editing_operator_index--;
          sprintf(display_message, "OP %1i", patch.operators[editing_operator_index].index);
          patch.sendSysexMessage(display_message);
        }
        else if (editing_operator_index > -1 and editing_operator_index == OP6) {
          editing_operator_index = -1;
          editing_all_flag = 1;
          sprintf(display_message, "ALL");
          patch.sendSysexMessage(display_message);
        }

      }
      else if (direction == -1) {
        if (editing_all_flag == 1) {
          editing_all_flag = 0;
          editing_operator_index = OP6;
          sprintf(display_message, "OP %1i", patch.operators[editing_operator_index].index);
          patch.sendSysexMessage(display_message);
        }
        else if (editing_operator_index > -1 and editing_operator_index < OP1) {
          editing_operator_index++;
          sprintf(display_message, "OP %1i", patch.operators[editing_operator_index].index);
          patch.sendSysexMessage(display_message);
        }
      }
    }
  }

private:
  char active_screen_type = 0;
  char editing_operator_index = -1;
  char editing_all_flag = 0;
  char active_param_group = -1;

  const uint8_t pad_notes[16] = {
    96, 97, 98, 99, 100, 101, 102, 103,
    112, 113, 114, 115, 116, 117, 118, 119
  };

  void changePadColor(uint8_t pad_index, uint8_t color_code) {
    uint8_t msg[3] = {CHANNEL1_NOTE_ON, pad_notes[pad_index], color_code};

    Usb.Task();
    Midi.SendData(msg, 1);
    delay(1);
  }

  const char parameters[2][3][8] = {
    // Operator parameters
    {
      {
        EG_RATE_1, EG_RATE_2, EG_RATE_3, EG_RATE_4, EG_LEVEL_1, EG_LEVEL_2, EG_LEVEL_3, EG_LEVEL_4
      },
      {
        KEYBOARD_LEVEL_SCALE_BREAK_POINT, KEYBOARD_LEVEL_SCALE_LEFT_DEPTH,
        KEYBOARD_LEVEL_SCALE_RIGHT_DEPTH, KEYBOARD_LEVEL_SCALE_LEFT_CURVE,
        KEYBOARD_LEVEL_SCALE_RIGHT_CURVE, KEYBOARD_RATE_SCALING
      },
      {
        OUTPUT_LEVEL, OSCILLATOR_MODE, OSCILLATOR_FREQUENCY_COARSE, OSCILLATOR_FREQUENCY_FINE,
        DETUNE, KEY_VELOCITY_SENSITIVITY, MODULATION_SENSITIVITY_AMPLITUDE
      }
    },
    // All parameters
    {
      {
        PITCH_EG_RATE_1, PITCH_EG_RATE_2, PITCH_EG_RATE_3, PITCH_EG_RATE_4,
        PITCH_EG_LEVEL_1, PITCH_EG_LEVEL_2, PITCH_EG_LEVEL_3, PITCH_EG_LEVEL_4
      },
      {
        LFO_SPEED, LFO_DELAY, LFO_PITCH_MODULATION_DEPTH, LFO_AMPLITUDE_MODULATION_DEPTH,
        LFO_SYNC, LFO_WAVE, MODULATION_SENSITIVITY_PITCH
      },
      {
        ALGORITHM, FEEDBACK, TRANSPOSE, OSCILLATOR_SYNC
      }
    }
  };

  const char parameter_group_lengths[2][3] = {
    {8, 6, 7}, {8, 6, 4}
  };
};

#endif // MAPPER_HPP
