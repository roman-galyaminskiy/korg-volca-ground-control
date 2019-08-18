#define CHANNEL1_NOTE_ON 144
#define CHANNEL1_NOTE_OFF 128
#define CHANNEL1_CONTROL_CHANGE 176

#define OP1 5
#define OP2 4
#define OP3 3
#define OP4 2
#define OP5 1
#define OP6 0
#define ALL -1

#define OPERATORS_NUMBER 6

int incomingByte = 0;   // for incoming serial data
int patch_sent = -1;

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
  char last_modified_parameter_code[10];

  Param parameters[21] = {
    {"egr1", 99}, {"egr2", 99}, {"egr3", 99}, {"egr4", 99},
    {"egl1", 99}, {"egl2", 99}, {"egl3", 99}, {"egl4", 99},
    {"lsbp", 99}, {"lsld", 99}, {"lsrd", 99}, {"lslc", 3},
    {"lslr", 3}, {"krs ", 7}, {"ams ", 7}, {"kvs", 7},
    {"olvl", 99}, {"fixd", 1, 1}, {"fcrs", 31}, {"ffne", 99},
    {"detn", 14}
  };

  void getParameterValues() {
    for (char i = 0; i < 21; i++) {
      Serial2.write(parameters[i].current_value);
      // Serial.print(parameters[i].display_code);
      // Serial.print(" ");
      // Serial.print(parameters[i].current_value, DEC);
      // Serial.println();
    }
  }
};

struct All {

  char modified_parameter = 0;
  char last_modified_parameter_code[10];

  Param parameters[19] = {
    {"ptr1", 99, 99, 0}, {"ptr2", 99, 99, 0}, {"ptr3", 99, 99, 0}, {"ptr4", 99, 99, 0},
    {"ptl1", 99, 50, 0}, {"ptl2", 99, 50, 0}, {"ptl3", 99, 50, 0}, {"ptl4", 99, 50, 0},
    {"algo", 31, 0, 0}, {"feed", 7, 0, 0}, {"oks ", 1, 0, 0}, {"lfor", 99, 0, 0},
    {"lfod", 99, 0, 0}, {"lpmd", 99, 0, 0}, {"lamd", 99, 99, 0}, {"lks ", 1, 0, 0},
    {"lfow", 5, 0, 0}, {"msp ", 7, 0, 0}, {"trsp", 48, 24, 0}
  };

  void getParameterValues() {
    for (char i = 0; i < 19; i++) {
      Serial2.write(parameters[i].current_value);
      // Serial.print(parameters[i].display_code);
      // Serial.print(" ");
      // Serial.print(parameters[i].current_value, DEC);
      // Serial.println();
    }
  }
};

struct Patch {
  char patch_name[10];

  Operator operators[6] = {{6}, {5}, {4}, {3}, {2}, {1}};
  All all;

  void beginSysex() {
    uint8_t patch_start[6] = {0xf0, 0x43, 0x00, 0x00, 0x01, 0x1b};
    for (char i = 0; i < 6; i++) {
      Serial2.write(patch_start[i]);
      // Serial.print(patch_start[i], HEX);
      // Serial.println();
    }
  }

  // Send SYSEX footer
  void endSysex() {
    Serial2.write(0xf7);
    // Serial.print(0xf7, HEX);
    // Serial.println();
  }

  // Send SYSEX body
  void sendPatchData() {
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

  void sendSysexMessage() {
    Serial.println("sendSysexMessage");
    // Message header
    beginSysex();
    // Message body
    sendPatchData();
    // Message footer
    endSysex();

    /*patch_sent = 1;*/
  }
};

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

    // Serial.print(incomingByte, DEC);
    // Serial.println();
    
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
      // Serial.println("bite 2");
      if (incomingByte == 67) {
        i = 1;
      }
      else {
       i = -1; 
      }      
    }

    if (incomingByte == 240) {
      // Serial.println("bite 1");
      i = 0;
    }    
  }
  else {
    if (patch_sent == 0) {
      Serial.println("test");
      delay(100);
      patch.sendSysexMessage();       
    }
  }
}
