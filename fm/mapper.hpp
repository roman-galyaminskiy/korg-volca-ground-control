#ifndef FM_HPP
#define FM_HPP

#include "usb.hpp"
#include "definitions.hpp"
#include "patch.hpp"

#define RETURN_PAD_INDEX 8 // PAD9
#define RETURN_PAD_COLOR 19 // ORANGE

#define ALGORITHM_RETURN_PAD_INDEX 15 // PAD9
#define ALGORITHM_RETURN_PAD_COLOR 15 // RED

#define OPERATOR_GROUPS 3
#define ALL_GROUPS 3
#define MAIN_SCREEN 0
#define PARAM_GROUP_SCREEN 1
#define PARAM_SCREEN 2
#define ALGORITHM_DISPLAY_SCREEN 3
#define OPERATOR_SELECT_SCREEN 4

// Reads parsed data from controller and light pads ("draw screens") and
// edit FM patch. Patch is send to synth using SYSEX messages.
//
// Patch parameters are divided by subentities (operators and "all" paramter group,
// that effects all operators). Within subentity parameters are divided so that we
// could control each chunk with 8 knobs. Pads and button are used to go throught
// menu. Menu has 3 "screens":
//
// * Patch subentity selection screen (OP/ALL, also allows you to turn each OP on and off completely)
// * Parameter group selection screen (Divided OP/ALL parameters in groups of atmost 8 params)
// * Parameter edit screen
//
// Parameter group and parameter edit screens have a "return" button that is left bottom pad
// drawn orange. Also you could change current subentity on these screens by pressing
// "tracks" buttons (OP1 is leftmost and ALL is rigthmost).
//
// On parameter edit screen you could edit patch parameters. Yellow pads represent active
// knobs, inactive knobs don't effect patch. You could tap active pads to retrieve current
// parameter value and parameter name.
class Mapper {
public:
  Patch patch;

  //###########################################################################
  // Draw functions
  //###########################################################################
  void drawPlayButtons() {
    // 'note_on' message templape
    uint8_t top_play_msg[3] = {CHANNEL1_NOTE_ON, TOP_PLAY, 0};
    uint8_t bottom_play_msg[3] = {CHANNEL1_NOTE_ON, BOTTOM_PLAY, 0};

    if (active_screen_type == OPERATOR_SELECT_SCREEN) {
      top_play_msg[2] = RED;
      bottom_play_msg[2] = RED;
    }
    else {
      top_play_msg[2] = BLACK;
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
    // char display_message[DISPLAY_CODE_LENGTH] = "main";
    // patch.sendSysexMessage(display_message);

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
      // Pads 9-14 are used to turn OP on/off
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

    // Drawing buttons
    drawPlayButtons();
  }

  void drawGroupScreen() {
    SERIAL_MONITOR.println("drawGroupScreen_started");
    active_screen_type = PARAM_GROUP_SCREEN;
    active_param_group = -1;

    char display_message[DISPLAY_CODE_LENGTH];

    SERIAL_MONITOR.print("editing_operator_index ");
    SERIAL_MONITOR.print(editing_operator_index, DEC);
    SERIAL_MONITOR.print(" editing_all_flag ");
    SERIAL_MONITOR.print(editing_all_flag, DEC);
    SERIAL_MONITOR.println();

    if (editing_operator_index > -1) {
      sprintf(display_message, "OP %i", patch.operators[editing_operator_index].index);
    }
    else if (editing_all_flag == 1) {
      sprintf(display_message, "ALL");
    }

    patch.sendSysexMessage(display_message);

    // Drawing top raw
    for (char pad_index = 0; pad_index < 8; pad_index++) {
      // Draw groups
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
      // Draw "return" button
      if (pad_index == RETURN_PAD_INDEX) {
        changePadColor(pad_index, RETURN_PAD_COLOR);
      }
      else {
        changePadColor(pad_index, BLACK);
      }
    }

    // Drawing buttons
    drawPlayButtons();
  }

  void drawParamScreen(char param_group = -1) {
    active_screen_type = PARAM_SCREEN;

    if (param_group > -1) {
      active_param_group = param_group;
    }

    SERIAL_MONITOR.println("drawParamScreen_started");

    char display_message[DISPLAY_CODE_LENGTH];

    SERIAL_MONITOR.print("editing_operator_index ");
    SERIAL_MONITOR.print(editing_operator_index, DEC);
    SERIAL_MONITOR.print(" editing_all_flag ");
    SERIAL_MONITOR.print(editing_all_flag, DEC);
    SERIAL_MONITOR.println();

    if (editing_operator_index > -1) {
      sprintf(display_message, "%i %s", patch.operators[editing_operator_index].index, parameter_group_names[0][active_param_group]);
    }
    else if (editing_all_flag == 1){
      sprintf(display_message, "A %s", parameter_group_names[1][active_param_group]);
    }
    // SERIAL_MONITOR.println(display_message);

    patch.sendSysexMessage(display_message);

    // Drawing top raw
    for (char pad_index = 0; pad_index < 8; pad_index++) {
      // Draw active pads (pads that are associated with patch parameter)
      if (editing_operator_index > -1) {
        if (pad_index < parameter_group_lengths[0][active_param_group]) {
          changePadColor(pad_index, YELLOW);
        }
        else {
          changePadColor(pad_index, BLACK);
        }
      }
      else if (editing_all_flag == 1) {
        if (pad_index < parameter_group_lengths[1][active_param_group]) {
          changePadColor(pad_index, YELLOW);
        }
        else {
          changePadColor(pad_index, BLACK);
        }
      }
    }
    // Drawing top raw
    for (char pad_index = 8; pad_index < 16; pad_index++) {
      // Draw "return" button
      if (pad_index == RETURN_PAD_INDEX) {
        changePadColor(pad_index, RETURN_PAD_COLOR);
      }
      else {
        changePadColor(pad_index, BLACK);
      }
    }

    drawPlayButtons();
  }

  void drawOperatorSelectionScreen() {
    active_screen_type = OPERATOR_SELECT_SCREEN;

    // Drawing top raw
    for (char pad_index = 0; pad_index < 8; pad_index++) {
      // Pads 1-6 represent operators
      if (pad_index < 6) {
        if (pad_index == patch.operators[editing_operator_index].index - 1) {
          changePadColor(pad_index, GREEN);
        }
        else {
          changePadColor(pad_index, YELLOW);
        }
      }
      else {
        changePadColor(pad_index, BLACK);
      }
    }
    // Drawing bottom raw
    for (char pad_index = 8; pad_index < 16; pad_index++) {
      changePadColor(pad_index, BLACK);
    }

    // Drawing buttons
    drawPlayButtons();
  }

  // Auxiliary screen for displaying current algorithm

  char getCarrierPosition(char operator_index) {
    char carrier_position;

    // Check if carrier has modulator
    char carrier_edge_index = -1;
    char modulated_carriers_number = 0;
    char modulator_branch_number = 0;

    for (char edge_index = 0; edge_index < algorithm_edges_number[patch.all.parameters[ALGORITHM].current_value]; edge_index++) {
      if (algorithm_edges[patch.all.parameters[ALGORITHM].current_value][edge_index][0] == operator_index) {
        carrier_edge_index = edge_index;
        break;
      }
    }
    // Check if carrier modulator modulates other carriers
    if (carrier_edge_index > -1) {
      for (char edge_index = 0; edge_index < algorithm_edges_number[patch.all.parameters[ALGORITHM].current_value]; edge_index++) {
        if (algorithm_edges[patch.all.parameters[ALGORITHM].current_value][edge_index][1] == algorithm_edges[patch.all.parameters[ALGORITHM].current_value][carrier_edge_index][1]) {
          if (edge_index == carrier_edge_index) {
            modulator_branch_number = modulated_carriers_number;
          }
          modulated_carriers_number++;
        }
      }
    }

    // Carriers are drawn in the bottom row starting from the left
    if (last_drawn_pad_index == -1) {
      carrier_position = 8;
      last_drawn_pad_index = 8;
      return carrier_position;
    }
    else {
      // If last drawn pad was for modulator add 1 pad margin
      if (last_drawn_pad_index >= 0 and last_drawn_pad_index <= 7) {
        carrier_position = last_drawn_pad_index + 8 + 2;
        last_drawn_pad_index = last_drawn_pad_index + 8 + 2;
        return carrier_position;
      }
      else if (last_drawn_pad_index >= 8 and last_drawn_pad_index <= 15 and
          (carrier_edge_index > -1 and modulated_carriers_number == 1 or carrier_edge_index > -1 and modulated_carriers_number > 1 and modulator_branch_number == 0)
        ) {
        carrier_position = last_drawn_pad_index + 2;
        last_drawn_pad_index = last_drawn_pad_index + 2;
        return carrier_position;
      }
      // If last drawn pad was for carrier margin is not needed
      else if (last_drawn_pad_index >= 8 and last_drawn_pad_index <= 15) {
        carrier_position = last_drawn_pad_index + 1;
        last_drawn_pad_index = last_drawn_pad_index + 1;
        return carrier_position;
      }
    }
  }

  void drawCarrier(char operator_index) {
    getCarrierPosition(operator_index);

    if (feedback[patch.all.parameters[ALGORITHM].current_value][operator_index] == 1) {
      changePadColor(last_drawn_pad_index, ORANGE);
    }
    else {
      changePadColor(last_drawn_pad_index, GREEN);
    }
  }

  char getModulatorPosition(char modulator_edge_index) {
    char modulator_position;

    // Check if it's a carrier modulator
    char non_carrier_modulator = 0;
    char modulators_number = 0;

    for (char edge_index = 0; edge_index < algorithm_edges_number[patch.all.parameters[ALGORITHM].current_value]; edge_index++) {
      if (algorithm_edges[patch.all.parameters[ALGORITHM].current_value][modulator_edge_index][0] == algorithm_edges[patch.all.parameters[ALGORITHM].current_value][edge_index][1]) {
        non_carrier_modulator = 1;
        break;
      }
    }
    // Check if carrier modulator is modulated by multiple modulators (rule for algorithms 14 and 15)
    if (non_carrier_modulator == 0) {
      for (char edge_index = 0; edge_index < algorithm_edges_number[patch.all.parameters[ALGORITHM].current_value]; edge_index++) {
        if (algorithm_edges[patch.all.parameters[ALGORITHM].current_value][modulator_edge_index][1] == algorithm_edges[patch.all.parameters[ALGORITHM].current_value][edge_index][0]) {
          modulators_number++;
        }
      }
    }

    // Check if this modulator is modulating multiple carriers
    char modulated_carriers_number = 0;
    char modulator_branch_number = 0;

    for (char edge_index = 0; edge_index < algorithm_edges_number[patch.all.parameters[ALGORITHM].current_value]; edge_index++) {
      if (algorithm_edges[patch.all.parameters[ALGORITHM].current_value][edge_index][1] == algorithm_edges[patch.all.parameters[ALGORITHM].current_value][modulator_edge_index][1]) {
        if (edge_index == modulator_edge_index) {
          modulator_branch_number = modulated_carriers_number;
        }
        modulated_carriers_number++;
      }
    }

    // If it's a carrier modulator
    if (non_carrier_modulator == 0) {
      // If modulator modulates only one carrier
      if (modulated_carriers_number == 1) {
        // If it's a first modulator branch
        if (last_drawn_pad_index >= 8 and last_drawn_pad_index <= 18) {
          // Rule for algorithms 14 and 15
          if (modulators_number > 1) {
            modulator_position = last_drawn_pad_index + 1;
            last_drawn_pad_index = last_drawn_pad_index + 1;
            return modulator_position;
          }
          modulator_position = last_drawn_pad_index - 8;
          last_drawn_pad_index = last_drawn_pad_index - 8;
          return modulator_position;
        }
        // If it's not first modulator branch
        else if (last_drawn_pad_index >= 0 and last_drawn_pad_index <= 7) {
          modulator_position = last_drawn_pad_index + 2;
          last_drawn_pad_index = last_drawn_pad_index + 2;
          return modulator_position;
        }
      }
      // If modulator modulates multiple carriers
      else {
        // Draw modulator above the leftmost carrier
        if (modulator_branch_number == 0) {
          modulator_position = last_drawn_pad_index - 8;
          return modulator_position;
        }
        else {
          if (modulator_branch_number == modulated_carriers_number - 1) {
            last_drawn_pad_index = last_drawn_pad_index - 8;
          }

          // Draw nothing
          return -1;
        }
      }
    }
    // If it's modulating modulator
    else {
      if (last_drawn_pad_index >= 0 and last_drawn_pad_index <= 7) {
        modulator_position = last_drawn_pad_index + 1;
        last_drawn_pad_index = last_drawn_pad_index + 1;
        return modulator_position;
      }
      else if (last_drawn_pad_index >= 8 and last_drawn_pad_index <= 18) {
        modulator_position = last_drawn_pad_index - 8;
        last_drawn_pad_index = last_drawn_pad_index  -8;
        return modulator_position;
      }
    }
  }

  void drawModulator(char edge_index) {
    char modulator_position;

    modulator_position = getModulatorPosition(edge_index);

    if (modulator_position > -1) {
      if (feedback[patch.all.parameters[ALGORITHM].current_value][algorithm_edges[patch.all.parameters[ALGORITHM].current_value][edge_index][1]] == 1) {
        changePadColor(modulator_position, ORANGE);
      }
      else {
        changePadColor(modulator_position, YELLOW);
      }
    }

  }

  char getModulatorFor(char operator_index) {
    // Find all modulators for operator
    for (char edge_index = 0; edge_index < algorithm_edges_number[patch.all.parameters[ALGORITHM].current_value]; edge_index++) {
      // Modulator found
      if (operator_index == algorithm_edges[patch.all.parameters[ALGORITHM].current_value][edge_index][0]) {

        drawModulator(edge_index);

        // Find all modulators for modulator
        getModulatorFor(algorithm_edges[patch.all.parameters[ALGORITHM].current_value][edge_index][1]);
      }
    }
    // delay(1000);
    return -1;
  }

  void drawAlgorithm() {
    active_screen_type = ALGORITHM_DISPLAY_SCREEN;
    last_drawn_pad_index = -1;

    char modulates_something = 0;

    clearScreen();

    for (char operator_index = 0; operator_index < OPERATORS_NUMBER; operator_index++) {
      modulates_something = 0;

      for (char edge_index = 0; edge_index < algorithm_edges_number[patch.all.parameters[ALGORITHM].current_value]; edge_index++) {
        if (algorithm_edges[patch.all.parameters[ALGORITHM].current_value][edge_index][1] == operator_index) {
          modulates_something = 1;
        }
      }
      if (modulates_something == 0) {

        drawCarrier(operator_index);

        getModulatorFor(operator_index);
      }
    }
  }

  //###########################################################################
  // Controller event handling
  //##########################################################################

  void keyPressed(uint8_t key_note) {
    uint8_t msg[3] = {CHANNEL1_NOTE_ON, key_note, 127};
    // SERIAL_MONITOR.print("keyPressed ");
    // SERIAL_MONITOR.print(msg[0], DEC);
    // SERIAL_MONITOR.print(" ");
    // SERIAL_MONITOR.print(msg[1], DEC);
    // SERIAL_MONITOR.print(" ");
    // SERIAL_MONITOR.print(msg[2], DEC);
    // SERIAL_MONITOR.println();
    MIDI_SERIAL_PORT_1.write(msg, 3);
  }

  void keyReleased(uint8_t key_note) {
    uint8_t msg[3] = {CHANNEL1_NOTE_OFF, key_note, 127};
    // SERIAL_MONITOR.print("keyReleased ");
    // SERIAL_MONITOR.print(msg[0], DEC);
    // SERIAL_MONITOR.print(" ");
    // SERIAL_MONITOR.print(msg[1], DEC);
    // SERIAL_MONITOR.print(" ");
    // SERIAL_MONITOR.print(msg[2], DEC);
    // SERIAL_MONITOR.println();
    MIDI_SERIAL_PORT_1.write(msg, 3);
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
      if (editing_operator_index > -1 and pad_index < OPERATOR_GROUPS) {
        drawParamScreen(pad_index);
      }
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
            active_param_index = pad_index;
            // SERIAL_MONITOR.print("Show param ");
            // SERIAL_MONITOR.print(pad_index, DEC);
            // SERIAL_MONITOR.print(" from group ");
            // SERIAL_MONITOR.print(active_param_group, DEC);
            // SERIAL_MONITOR.print(" or ");
            // SERIAL_MONITOR.print(parameters[0][active_param_group][pad_index], DEC);
            // SERIAL_MONITOR.print(" for operator ");
            // SERIAL_MONITOR.print(editing_operator_index, DEC);
            // SERIAL_MONITOR.println();
            patch.showParameterValue(editing_operator_index, parameters[0][active_param_group][active_param_index]);
          }
        }
        else {
          if (pad_index < parameter_group_lengths[1][active_param_group]) {
            active_param_index = pad_index;
            // SERIAL_MONITOR.print("Show param ");
            // SERIAL_MONITOR.print(pad_index, DEC);
            // SERIAL_MONITOR.print(" from group ");
            // SERIAL_MONITOR.print(active_param_group, DEC);
            // SERIAL_MONITOR.print(" or ");
            // SERIAL_MONITOR.print(parameters[0][active_param_group][pad_index], DEC);
            // SERIAL_MONITOR.print(" for all ");
            // SERIAL_MONITOR.print(editing_operator_index, DEC);
            // SERIAL_MONITOR.println();
            patch.showParameterValue(editing_operator_index, parameters[1][active_param_group][active_param_index]);
            if (parameters[1][active_param_group][active_param_index] == ALGORITHM) {
              drawAlgorithm();
            }
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
    else if (active_screen_type == ALGORITHM_DISPLAY_SCREEN) {
      drawParamScreen();
    }
    else if (active_screen_type == OPERATOR_SELECT_SCREEN) {
      if (pad_index == 0) {
        if (editing_operator_index != OP1) {
          for (char param_index = 0; param_index < parameter_group_lengths[0][active_param_group]; param_index++) {
            patch.operators[OP1].parameters[parameters[0][active_param_group][param_index]].current_value = patch.operators[editing_operator_index].parameters[parameters[0][active_param_group][param_index]].current_value;

            if (active_param_group == 2) {
              patch.operators[OP1].power = patch.operators[editing_operator_index].power;
            }
          }
        }
      }
      else if (pad_index == 1) {
        if (editing_operator_index != OP2) {
          for (char param_index = 0; param_index < parameter_group_lengths[0][active_param_group]; param_index++) {
            patch.operators[OP2].parameters[parameters[0][active_param_group][param_index]].current_value = patch.operators[editing_operator_index].parameters[parameters[0][active_param_group][param_index]].current_value;
          }

          if (active_param_group == 2) {
            patch.operators[OP2].power = patch.operators[editing_operator_index].power;
          }
        }
      }
      else if (pad_index == 2) {
        if (editing_operator_index != OP3) {
          for (char param_index = 0; param_index < parameter_group_lengths[0][active_param_group]; param_index++) {
            patch.operators[OP3].parameters[parameters[0][active_param_group][param_index]].current_value = patch.operators[editing_operator_index].parameters[parameters[0][active_param_group][param_index]].current_value;
          }

          if (active_param_group == 2) {
            patch.operators[OP3].power = patch.operators[editing_operator_index].power;
          }
        }
      }
      else if (pad_index == 3) {
        if (editing_operator_index != OP4) {
          for (char param_index = 0; param_index < parameter_group_lengths[0][active_param_group]; param_index++) {
            patch.operators[OP4].parameters[parameters[0][active_param_group][param_index]].current_value = patch.operators[editing_operator_index].parameters[parameters[0][active_param_group][param_index]].current_value;
          }

          if (active_param_group == 2) {
            patch.operators[OP4].power = patch.operators[editing_operator_index].power;
          }
        }
      }
      else if (pad_index == 4) {
        if (editing_operator_index != OP5) {
          for (char param_index = 0; param_index < parameter_group_lengths[0][active_param_group]; param_index++) {
            patch.operators[OP5].parameters[parameters[0][active_param_group][param_index]].current_value = patch.operators[editing_operator_index].parameters[parameters[0][active_param_group][param_index]].current_value;
          }

          if (active_param_group == 2) {
            patch.operators[OP5].power = patch.operators[editing_operator_index].power;
          }
        }
      }
      else if (pad_index == 5) {
        if (editing_operator_index != OP6) {
          for (char param_index = 0; param_index < parameter_group_lengths[0][active_param_group]; param_index++) {
            patch.operators[OP6].parameters[parameters[0][active_param_group][param_index]].current_value = patch.operators[editing_operator_index].parameters[parameters[0][active_param_group][param_index]].current_value;
          }

          if (active_param_group == 2) {
            patch.operators[OP6].power = patch.operators[editing_operator_index].power;
          }
        }
      }

      drawParamScreen();
    }
  }

  void topPlayButtonPressed() {
    if (active_screen_type == PARAM_SCREEN or active_screen_type == OPERATOR_SELECT_SCREEN) {
      top_play_button_pressed = 1;
    }
  }

  void bottomPlayButtonPressed() {
    if (top_play_button_pressed == 1) {
      if (active_screen_type == OPERATOR_SELECT_SCREEN) {
        drawParamScreen();
      }
      else if (active_screen_type == PARAM_SCREEN) {
        if (editing_operator_index > -1) {
          drawOperatorSelectionScreen();

          char display_message[DISPLAY_CODE_LENGTH];
          sprintf(display_message, "copy %d to", patch.operators[editing_operator_index].index);

          patch.sendSysexMessage(display_message);
        }
      }
    }
  }

  void knobRotated(char knob_index, char value) {
    SERIAL_MONITOR.println("knobRotated_started");
    if (active_screen_type == PARAM_SCREEN or active_screen_type == ALGORITHM_DISPLAY_SCREEN) {
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
          if (parameters[1][active_param_group][knob_index] == ALGORITHM) {
            drawAlgorithm();
          }
          patch.sendSysexMessage();
        }
      }
    }
  }

  void trackChanged(char direction) {
    char display_message[DISPLAY_CODE_LENGTH];

    if (active_screen_type == PARAM_GROUP_SCREEN or active_screen_type == PARAM_SCREEN) {
      if (direction == 1) {
        // SERIAL_MONITOR.println("DIRECTION UP");
        if (editing_operator_index > -1 and editing_operator_index > OP6) {
          editing_operator_index--;

          if (active_screen_type == PARAM_SCREEN) {
            if (active_param_index > -1) {
              patch.showParameterValue(editing_operator_index, parameters[0][active_param_group][active_param_index]);
            }
            else {
              patch.showParameterValue(editing_operator_index, parameters[0][active_param_group][0]);
            }

          }
          else {
            sprintf(display_message, "OP %1i", patch.operators[editing_operator_index].index);
            patch.sendSysexMessage(display_message);
          }
        }
        else if (editing_operator_index > -1 and editing_operator_index == OP6) {
          editing_operator_index = -1;
          editing_all_flag = 1;

          if (active_screen_type == PARAM_SCREEN) {
            if (active_param_index > -1) {
              patch.showParameterValue(editing_operator_index, parameters[1][active_param_group][active_param_index]);
            }
            else {
              patch.showParameterValue(editing_operator_index, parameters[1][active_param_group][0]);
            }
          }
          else {
            sprintf(display_message, "ALL");
            patch.sendSysexMessage(display_message);
          }
        }

      }
      else if (direction == -1) {
        // SERIAL_MONITOR.println("DIRECTION DOWN");
        if (editing_all_flag == 1) {
          editing_all_flag = 0;
          editing_operator_index = OP6;

          if (active_screen_type == PARAM_SCREEN) {
            patch.showParameterValue(editing_operator_index, parameters[0][active_param_group][active_param_index]);
          }
          else {
            sprintf(display_message, "OP %1i", patch.operators[editing_operator_index].index);
            patch.sendSysexMessage(display_message);
          }
        }
        else if (editing_operator_index > -1 and editing_operator_index < OP1) {
          editing_operator_index++;

          if (active_screen_type == PARAM_SCREEN) {
            patch.showParameterValue(editing_operator_index, parameters[0][active_param_group][active_param_index]);
          }
          else {
            sprintf(display_message, "OP %1i", patch.operators[editing_operator_index].index);
            patch.sendSysexMessage(display_message);
          }
        }
      }
    }
  }

private:
  char active_screen_type = 0;
  char editing_operator_index = -1;
  char editing_all_flag = 0;
  char active_param_group = -1;
  char active_param_index = -1;
  char top_play_button_pressed = 0;

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

  void clearScreen() {
    for (char i = 0; i < 16; i++) {
      changePadColor(i, BLACK);
    }
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
        LFO_SYNC, LFO_WAVE, MODULATION_SENSITIVITY_PITCH, OSCILLATOR_SYNC
      },
      {
        ALGORITHM, FEEDBACK, TRANSPOSE
      }
    }
  };

  const char parameter_group_lengths[2][3] = {
    {8, 6, 7}, {8, 8, 3}
  };

  const char parameter_group_names[2][3][10] = {
    {"lvl eg", "scale", "osc"},
    {"pcht eg", "lfo", "prog"}
  };

  char last_drawn_pad_index = -1;

  const char feedback[32][6] = {
    {0, 0, 0, 0, 0, 1}, // 1
    {0, 1, 0, 0, 0, 0}, // 2
    {0, 0, 0, 0, 0, 1}, // 3
    {0, 0, 0, 1, 1, 1}, // 4
    {0, 0, 0, 0, 0, 1}, // 5

    {0, 0, 0, 0, 1, 1}, // 6
    {0, 0, 0, 0, 0, 1}, // 7
    {0, 0, 0, 1, 0, 0}, // 8
    {0, 1, 0, 0, 0, 0}, // 9
    {0, 0, 1, 0, 0, 0}, // 10

    {0, 0, 0, 0, 0, 1}, // 11
    {0, 1, 0, 0, 0, 0}, // 12
    {0, 0, 0, 0, 0, 1}, // 13
    {0, 0, 0, 0, 0, 1}, // 14
    {0, 1, 0, 0, 0, 0}, // 15

    {0, 0, 0, 0, 0, 1}, // 16
    {0, 1, 0, 0, 0, 0}, // 17
    {0, 0, 1, 0, 0, 0}, // 18
    {0, 0, 0, 0, 0, 1}, // 19
    {0, 0, 1, 0, 0, 0}, // 20

    {0, 0, 1, 0, 0, 0}, // 21
    {0, 0, 0, 0, 0, 1}, // 22
    {0, 0, 0, 0, 0, 1}, // 23
    {0, 0, 0, 0, 0, 1}, // 24
    {0, 0, 0, 0, 0, 1}, // 25

    {0, 0, 0, 0, 0, 1}, // 26
    {0, 0, 1, 0, 0, 0}, // 27
    {0, 0, 0, 0, 1, 0}, // 28
    {0, 0, 0, 0, 0, 1}, // 29
    {0, 0, 0, 0, 1, 0}, // 30

    {0, 0, 0, 0, 0, 1}, // 31
    {0, 0, 0, 0, 0, 1} // 32
  };

  const char algorithm_edges_number[32] = {
    4, 4, 4, 4, 3,
    3, 4, 4, 4, 4,
    4, 4, 4, 4, 4,
    5, 5, 5, 4, 4,
    4, 4, 3, 3, 2,
    3, 3, 3, 2, 2,
    1, 0
  };

  const char algorithm_edges[32][5][2] = {
    {{0, 1}, {2, 3}, {3, 4}, {4, 5}}, // 1
    {{0, 1}, {2, 3}, {3, 4}, {4, 5}}, // 2
    {{0, 1}, {1, 2}, {3, 4}, {4, 5}}, // 3
    {{0, 1}, {1, 2}, {3, 4}, {4, 5}}, // 4
    {{0, 1}, {2, 3}, {4, 5}}, // 5

    {{0, 1}, {2, 3}, {4, 5}}, // 6
    {{0, 1}, {2, 3}, {2, 4}, {4, 5}}, // 7
    {{0, 1}, {2, 3}, {2, 4}, {4, 5}}, // 8
    {{0, 1}, {2, 3}, {2, 4}, {4, 5}}, // 9
    {{0, 1}, {1, 2}, {3, 4}, {3, 5}}, // 10

    {{0, 1}, {1, 2}, {3, 4}, {3, 5}}, // 11
    {{0, 1}, {2, 3}, {2, 4}, {2, 5}}, // 12
    {{0, 1}, {2, 3}, {2, 4}, {2, 5}}, // 13
    {{0, 1}, {2, 3}, {3, 4}, {3, 5}}, // 14
    {{0, 1}, {2, 3}, {3, 4}, {3, 5}}, // 15

    {{0, 1}, {0, 2}, {2, 3}, {0, 4}, {4, 5}}, // 16
    {{0, 1}, {0, 2}, {2, 3}, {0, 4}, {4, 5}}, // 17
    {{0, 1}, {0, 2}, {0, 3}, {3, 4}, {4, 5}}, // 18
    {{0, 1}, {1, 2}, {3, 5}, {4, 5}}, // 19
    {{0, 2}, {1, 2}, {3, 4}, {3, 5}}, // 20

    {{0, 2}, {1, 2}, {3, 5}, {4, 5}}, // 21
    {{0, 1}, {2, 5}, {3, 5}, {4, 5}}, // 22
    {{1, 2}, {3, 5}, {4, 5}}, // 23
    {{2, 5}, {3, 5}, {4, 5}}, // 24
    {{3, 5}, {4, 5}}, // 25

    {{1, 2}, {3, 4}, {3, 5}}, // 26
    {{1, 2}, {3, 4}, {3, 5}}, // 27
    {{0, 1}, {2, 3}, {3, 4}}, // 28
    {{2, 3}, {4, 5}}, // 29
    {{2, 3}, {3, 4}}, // 30

    {{4, 5}}
  };

};

#endif // FM_HPP
