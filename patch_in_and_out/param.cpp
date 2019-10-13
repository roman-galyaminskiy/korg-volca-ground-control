#include "Param.hpp"

void Param::setValue(char value) {
  previous_value = current_value;
  current_value = value;
}

void Param::restorePreviousValue() {
  char buffer = current_value;
  current_value = previous_value;
  previous_value = buffer;
}
