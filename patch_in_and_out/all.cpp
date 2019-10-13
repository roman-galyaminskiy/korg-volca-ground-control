#include <Arduino.h>
#include "All.hpp"

void All::getParameterValues() {
  for (char i = 0; i < 19; i++) {
    Serial2.write(parameters[i].current_value);
    // Serial.print(parameters[i].display_code);
    // Serial.print(" ");
    // Serial.print(parameters[i].current_value, DEC);
    // Serial.println();
  }
}
