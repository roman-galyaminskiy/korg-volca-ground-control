#include <Arduino.h>
#include "Patch.hpp"

void Patch::beginSysex() {
  uint8_t patch_start[6] = {0xf0, 0x43, 0x00, 0x00, 0x01, 0x1b};
  for (char i = 0; i < 6; i++) {
    Serial2.write(patch_start[i]);
    // Serial.print(patch_start[i], HEX);
    // Serial.println();
  }
}

// Send SYSEX footer
void Patch::endSysex() {
  Serial2.write(0xf7);
  // Serial.print(0xf7, HEX);
  // Serial.println();
}

// Send SYSEX body
void Patch::sendPatchData() {
  char local_patch_name[10] = "test1234";

  // Collect subentities data
  for (char i = 0; i < 6; i++) {
    operators[i].getParameterValues();
  }

  all.getParameterValues();

  for (char i = 0; i < 10; i++) {
    Serial2.write(local_patch_name[i]);
    // Serial.print(local_patch_name[i]);
    // Serial.println();
  }

  // Collecting operator ON/OFF status
  char operators_power_status = operators[OP1].power + 2 * operators[OP2].power +
    4 * operators[OP3].power + 8 * operators[OP4].power + 16 * operators[OP5].power +
    32 * operators[OP6].power;

  Serial2.write(operators_power_status);
  // Serial.print(operators_power_status, DEC);
  // Serial.println();
}

void Patch::sendSysexMessage() {
  Serial.println("sendSysexMessage");
  // Message header
  beginSysex();
  // Message body
  sendPatchData();
  // Message footer
  endSysex();

  /*patch_sent = 1;*/
}

//SysEx:
void handle_sysex(byte* a, unsigned sizeofsysex){

  Serial.println(sizeofsysex,DEC);
 //Print Sysex on Serial0 on PC:
 for(int n=0;n<sizeofsysex;n++){
  Serial.print(a[n]);
  Serial.print("  ");
  }
 Serial.print('\n');
}

void Patch::receiveSysex(byte* sysex_array, unsigned sysex_array_length) {
  Serial.println("receiveSysex");
  // SERIAL_MONITOR.println("receiveSysex");

  char patch_name[DISPLAY_CODE_LENGTH];
  char param_index = 0;
  char op_index = 0;
  char name_index = -1;

  if (sysex_array_length == 263) {
    // Parse SysEx omitting first 5 bytes of header
    for (size_t sysex_index = 6; sysex_index < sysex_array_length - 1; sysex_index++) {

      if (op_index >= 0 and op_index <= 5) {
        // SERIAL_MONITOR.print(op_index, DEC);
        // SERIAL_MONITOR.print(" ");
        // SERIAL_MONITOR.print(param_index, DEC);
        // SERIAL_MONITOR.println();
        if (operators[op_index].parameters[param_index].max_value >= sysex_array[sysex_index]) {
          operators[op_index].parameters[param_index].setValue(sysex_array[sysex_index]);
        }
        else {
          operators[op_index].parameters[param_index].setValue(0);
        }
      }
      else {
        // SERIAL_MONITOR.print("all ");
        // SERIAL_MONITOR.print(param_index, DEC);
        // SERIAL_MONITOR.println();
        if (all.parameters[param_index].max_value >= sysex_array[sysex_index]) {
          all.parameters[param_index].setValue(sysex_array[sysex_index]);
        }
        else {
          all.parameters[param_index].setValue(0);
        }
      }

      if (op_index >= 0 and op_index <= 5 and param_index < 20) {
        param_index++;
      }
      else if (op_index >= 0 and op_index < 5 and param_index == 20) {
          op_index++;
          param_index = 0;
      }
      else if (op_index >= 0 and op_index == 5 and param_index == 20) {
          op_index = -1;
          param_index = 0;
      }
      else if (op_index == -1 and param_index < 18) {
          param_index++;
      }
      else if (op_index == -1 and param_index == 18) {
          param_index++;
          name_index = 0;
          // SERIAL_MONITOR.println("Done");
      }
      else if (op_index == -1 and name_index > -1 and name_index <= 10) {
          name_index++;
          patch_name[name_index] = sysex_array[sysex_index];
          // SERIAL_MONITOR.write(a[j]);
          // SERIAL_MONITOR.println();
      }
    }
  }
}
