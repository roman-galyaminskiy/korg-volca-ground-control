#include "usb.hpp"
#include "controller.hpp"

//////////////////////////
// MIDI Pin assign
// 2 : GND
// 4 : +5V(Vcc) with 220ohm
// 5 : TX
//////////////////////////

#define SERIAL_MONITOR Serial

// controller specific macro
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

#define BLACK 0
#define RED 15
#define ORANGE 19
#define YELLOW 21
#define GREEN 24

#define TRACK_LEFT 106
#define TRACK_RIGHT 107

#define OPERATORS_NUMBER 6

#define CHANNEL1_CONTROL_CHANGE 176

Controller controller;

void setup() {
  SERIAL_MONITOR.begin(9600);
  SERIAL_MONITOR.println("Checking USB...");
  if (Usb.Init() == -1) {
    while (1); //halt
  }//if (Usb.Init() == -1...
  delay( 200 );

  SERIAL_MONITOR.println("SUCCESS");
}

struct Screen {
  const uint8_t pad_notes[16] = {
    96, 97, 98, 99, 100, 101, 102, 103,
    112, 113, 114, 115, 116, 117, 118, 119
  };

  char algorithm_index = 14;

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

  char last_drawn_pad_index = -1;

  void changePadColor(uint8_t pad_index, uint8_t color_code) {
    uint8_t msg[3] = {CHANNEL1_NOTE_ON, pad_notes[pad_index], color_code};

    Usb.Task();
    Midi.SendData(msg, 1);
    delay(1);
  }

  void clearScreen() {
    for (char i = 0; i < 16; i++) {
      changePadColor(i, BLACK);
      // SERIAL_MONITOR.println("Screen cleared");
    }
  }

  char getCarrierPosition(char operator_index) {
    char carrier_position;

    // Check if carrier has modulator
    char carrier_edge_index = -1;
    char modulated_carriers_number = 0;
    char modulator_branch_number = 0;

    for (char edge_index = 0; edge_index < algorithm_edges_number[algorithm_index]; edge_index++) {
      if (algorithm_edges[algorithm_index][edge_index][0] == operator_index) {
        carrier_edge_index = edge_index;
        break;
      }
    }
    // Check if carrier modulator modulates other carriers
    if (carrier_edge_index > -1) {
      for (char edge_index = 0; edge_index < algorithm_edges_number[algorithm_index]; edge_index++) {
        if (algorithm_edges[algorithm_index][edge_index][1] == algorithm_edges[algorithm_index][carrier_edge_index][1]) {
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
    // SERIAL_MONITOR.print("Drawing carrier ");
    // SERIAL_MONITOR.print(operator_index + 1, DEC);
    // SERIAL_MONITOR.print(" on pad ");

    getCarrierPosition(operator_index);

    // SERIAL_MONITOR.print(last_drawn_pad_index + 1, DEC);
    // SERIAL_MONITOR.println();

    if (feedback[algorithm_index][operator_index] == 1) {
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

    for (char edge_index = 0; edge_index < algorithm_edges_number[algorithm_index]; edge_index++) {
      if (algorithm_edges[algorithm_index][modulator_edge_index][0] == algorithm_edges[algorithm_index][edge_index][1]) {
        non_carrier_modulator = 1;
        break;
      }
    }
    // Check if carrier modulator is modulated by multiple modulators (rule for algorithms 14 and 15)
    if (non_carrier_modulator == 0) {
      for (char edge_index = 0; edge_index < algorithm_edges_number[algorithm_index]; edge_index++) {
        if (algorithm_edges[algorithm_index][modulator_edge_index][1] == algorithm_edges[algorithm_index][edge_index][0]) {
          modulators_number++;
        }
      }
    }

    // Check if this modulator is modulating multiple carriers
    char modulated_carriers_number = 0;
    char modulator_branch_number = 0;

    // SERIAL_MONITOR.print("\tCounting number of modulated carriers for operator: ");
    // SERIAL_MONITOR.print(algorithm_edges[algorithm_index][modulator_edge_index][1] + 1, DEC);
    // SERIAL_MONITOR.print(" ... ");
    for (char edge_index = 0; edge_index < algorithm_edges_number[algorithm_index]; edge_index++) {
      if (algorithm_edges[algorithm_index][edge_index][1] == algorithm_edges[algorithm_index][modulator_edge_index][1]) {
        if (edge_index == modulator_edge_index) {
          modulator_branch_number = modulated_carriers_number;
        }
        modulated_carriers_number++;
      }
    }

    // SERIAL_MONITOR.print(modulated_carriers_number, DEC);
    // SERIAL_MONITOR.println();

    // If it's a carrier modulator
    if (non_carrier_modulator == 0) {
      // SERIAL_MONITOR.println("\tCarrier modulator");

      // If modulator modulates only one carrier
      if (modulated_carriers_number == 1) {
        // If it's a first modulator branch
        if (last_drawn_pad_index >= 8 and last_drawn_pad_index <= 18) {
          // Rule for algorithms 14 and 15
          if (modulators_number > 1) {
            // SERIAL_MONITOR.println("\tException for rules 14 and 15");
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
        // SERIAL_MONITOR.println("\tModulator modulates multiple carriers");
        // SERIAL_MONITOR.print("\tNumber of branches: ");
        // SERIAL_MONITOR.print(modulated_carriers_number, DEC);
        // SERIAL_MONITOR.println();
        // SERIAL_MONITOR.print("\tBranch number: ");
        // SERIAL_MONITOR.print(modulator_branch_number, DEC);
        // SERIAL_MONITOR.println();

        // Draw modulator above the leftmost carrier
        if (modulator_branch_number == 0) {
          // SERIAL_MONITOR.println("\tDraw modulator above the leftmost carrier");

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
      if (feedback[algorithm_index][algorithm_edges[algorithm_index][edge_index][1]] == 1) {
        changePadColor(modulator_position, ORANGE);
      }
      else {
        changePadColor(modulator_position, YELLOW);
      }

      // SERIAL_MONITOR.print("Drawing modulator ");
      // SERIAL_MONITOR.print(algorithm_edges[algorithm_index][edge_index][1] + 1, DEC);
      // SERIAL_MONITOR.print(" on pad ");
      // SERIAL_MONITOR.print(last_drawn_pad_index + 1, DEC);
      // SERIAL_MONITOR.println();
    }

  }

  char getModulatorFor(char operator_index) {
    // Find all modulators for operator
    for (char edge_index = 0; edge_index < algorithm_edges_number[algorithm_index]; edge_index++) {
      // Modulator found
      if (operator_index == algorithm_edges[algorithm_index][edge_index][0]) {
        // DEBUG:
        // SERIAL_MONITOR.print("Found modulator ");
        // SERIAL_MONITOR.print(algorithm_edges[algorithm_index][edge_index][1], DEC);
        // SERIAL_MONITOR.print(" for ");
        // SERIAL_MONITOR.print(algorithm_edges[algorithm_index][edge_index][0], DEC);
        // SERIAL_MONITOR.println();

        drawModulator(edge_index);

        // Find all modulators for modulator
        getModulatorFor(algorithm_edges[algorithm_index][edge_index][1]);
      }
    }
    // delay(1000);
    return -1;
  }

  void drawAlgorithm() {
    // SERIAL_MONITOR.println();
    // SERIAL_MONITOR.print("###########################");
    // SERIAL_MONITOR.println();

    last_drawn_pad_index = -1;

    char modulates_something = 0;

    clearScreen();

    for (char operator_index = 0; operator_index < OPERATORS_NUMBER; operator_index++) {
      modulates_something = 0;

      // SERIAL_MONITOR.print("Checking operator ");
      // SERIAL_MONITOR.print(operator_index + 1, DEC);
      // SERIAL_MONITOR.println();
      /* code */
      for (char edge_index = 0; edge_index < algorithm_edges_number[algorithm_index]; edge_index++) {
        if (algorithm_edges[algorithm_index][edge_index][1] == operator_index) {
          // SERIAL_MONITOR.print("Found modulator ");
          // SERIAL_MONITOR.print(algorithm_edges[algorithm_index][edge_index][1] + 1, DEC);
          // SERIAL_MONITOR.print(" that modulates ");
          // SERIAL_MONITOR.print(algorithm_edges[algorithm_index][edge_index][0] + 1, DEC);
          // SERIAL_MONITOR.println();
          modulates_something = 1;
        }
      }
      if (modulates_something == 0) {
        drawCarrier(operator_index);
        // SERIAL_MONITOR.print("Found carrier ");
        // SERIAL_MONITOR.print(operator_index + 1, DEC);
        // SERIAL_MONITOR.println();
        getModulatorFor(operator_index);
      }

    }
  }
};

Screen screen;

void loop() {
  uint8_t msg[3];
  char size;
  char screen_initialized = 0;

  // Enable 'inContorol' mode in order to light up pads
  if (controller.getExtenderModeStatus() == 0) {
    Usb.Task();
    if ( Usb.getUsbTaskState() == USB_STATE_RUNNING ) {
      controller.enableExtendedMode();
      SERIAL_MONITOR.println("Controller is ready to use");

      screen.clearScreen();
      screen.drawAlgorithm();
      screen_initialized = 1;
    }
  }
  else {
    Usb.Task();

    if ( (size = Midi.RecvData(msg)) > 0 ) {
      SERIAL_MONITOR.print(msg[0], DEC);
      SERIAL_MONITOR.print(" ");
      SERIAL_MONITOR.print(msg[1], DEC);
      SERIAL_MONITOR.print(" ");
      SERIAL_MONITOR.print(msg[2], DEC);
      SERIAL_MONITOR.println();

      if (msg[0] == CHANNEL1_CONTROL_CHANGE and msg[1] == TRACK_LEFT and msg[2] == 127) {
        screen.algorithm_index--;
        screen.drawAlgorithm();
      }

      else if (msg[0] == CHANNEL1_CONTROL_CHANGE and msg[1] == TRACK_RIGHT and msg[2] == 127) {
        screen.algorithm_index++;
        screen.drawAlgorithm();
      }

    }
  }
}
