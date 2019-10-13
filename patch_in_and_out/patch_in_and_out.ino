#include "Patch.hpp"

#define CHANNEL1_NOTE_ON 144
#define CHANNEL1_NOTE_OFF 128
#define CHANNEL1_CONTROL_CHANGE 176



int incomingByte = 0;   // for incoming serial data
int patch_sent = -1;

Patch patch;


void setup() {
  Serial.begin(9600);
  Serial2.begin(31250);
}

void loop() {
  char param_index = 0;
  char op_index = 0;
  char name_index = -1;

  static int i = -1;
  static int d = 0;
  static int a[170];
  int r;

  uint8_t msg[3] = {CHANNEL1_NOTE_ON, 63, 127};

  // send data only when you receive data:
  if (Serial2.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial2.read();

     Serial.print(incomingByte, DEC);
     Serial.println();

    if (i == -1) {
      for (size_t j = 0; j < 170; j++) {
        a[j] = 0;
      }
    }

    if (i == 5) {
      if (incomingByte == 247) {

        patch_sent = 0;

        Serial.print(d, DEC);
        Serial.println(" end");



        i = -1;

        for (size_t j = 0; j <= d; j++) {
          // Serial.print(a[j], DEC);
          // Serial.print(" ");
          // Serial.write(a[j]);
          // Serial.println();

          if (op_index >= 0 and op_index <= 5) {
            // Serial.print(op_index, DEC);
            // Serial.print(" ");
            // Serial.print(param_index, DEC);
            // Serial.println();
            if (patch.operators[op_index].parameters[param_index].max_value >= a[j]) {
              patch.operators[op_index].parameters[param_index].setValue(a[j]);
            }
            else {
              patch.operators[op_index].parameters[param_index].setValue(0);
            }
          }
          else {
            // Serial.print("all ");
            // Serial.print(param_index, DEC);
            // Serial.println();
            if (patch.all.parameters[param_index].max_value >= a[j]) {
              patch.all.parameters[param_index].setValue(a[j]);
            }
            else {
              patch.all.parameters[param_index].setValue(0);
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
              // Serial.println("Done");
          }
          else if (op_index == -1 and name_index > -1 and name_index <= 10) {
              patch.patch_name[name_index] = a[j];
              name_index++;
              // Serial.write(a[j]);
              // Serial.println();
          }
          else {
            // Serial.print("On/off ");
            // Serial.println();

            patch.operators[0].power = a[j] & 1;
            // Serial.print(a[j] & 1, BIN);
            // Serial.println();

            patch.operators[1].power = (a[j] & (1 << 1)) >> 1;
            // Serial.print((a[j] & (1 << 1)) >> 1, BIN);
            // Serial.println();

            patch.operators[2].power = (a[j] & (1 << 2)) >> 2;
            // Serial.print((a[j] & (1 << 2)) >> 2, BIN);
            // Serial.println();

            patch.operators[3].power = (a[j] & (1 << 3)) >> 3;
            // Serial.print((a[j] & (1 << 3)) >> 3, BIN);
            // Serial.println();

            patch.operators[4].power = (a[j] & (1 << 4)) >> 4;
            // Serial.print((a[j] & (1 << 4)) >> 4, BIN);
            // Serial.println();

            // Serial.print((a[j] & (1 << 5)) >> 5, BIN);
            // Serial.println();
            patch.operators[5].power = (a[j] & (1 << 5)) >> 5;
            break;
          }
        }

      }
      else {
        if (incomingByte < 240) {
          a[d] = incomingByte;
          d += 1;
        }

        if (d == 169) {
          i = -1;
          d = 0;
        }
      }
    }

    if (i == 4) {
      // Serial.println("bite 6");
      if (incomingByte == 27) {
        i = 5;
      }
      else {
       i = -1;
      }
    }

    if (i == 3) {
      // Serial.println("bite 5");
      if (incomingByte == 1) {
        i = 4;
      }
      else {
       i = -1;
      }
    }

    if (i == 2) {
      // Serial.println("bite 4");
      if (incomingByte == 0) {
        i = 3;
      }
      else {
       i = -1;
      }
    }

    if (i == 1) {
      // Serial.println("bite 3");
      if (incomingByte == 0) {
        i = 2;
      }
      else {
       i = -1;
      }
    }

    if (i == 0) {
      Serial.println("bite 2");
      if (incomingByte == 67) {
        i = 1;
      }
      else {
       i = -1;
      }
    }

    if (incomingByte == 240) {
      Serial.println("bite 1");
      i = 0;
    }
  }
  else {
    // if (patch_sent == 0) {
    //   Serial.println("test");
    //   delay(100);
    //   patch.sendSysexMessage();
    // }
  }
}
