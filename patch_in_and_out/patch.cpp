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
