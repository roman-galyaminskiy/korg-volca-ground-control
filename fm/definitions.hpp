#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP
#define SERIAL_MONITOR Serial
#define MIDI_SERIAL_PORT_1 Serial2
// #define MIDI_SERIAL_PORT_2 Serial2
//////////////////////////
// MIDI Pin assign
// 2 : GND
// 4 : +5V(Vcc) with 220ohm
// 5 : TX
//////////////////////////

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

#define TOP_PLAY 104
#define BOTTOM_PLAY 120
#define TRACK_LEFT 106
#define TRACK_RIGHT 107
#define SCENE_UP 104
#define SCENE_DOWN 105

#define KNOB1 21
#define KNOB2 22
#define KNOB3 23
#define KNOB4 24
#define KNOB5 25
#define KNOB6 26
#define KNOB7 27
#define KNOB8 28

#define BLACK 0
#define RED 15
#define ORANGE 19
#define YELLOW 21
#define GREEN 24

#define CHANNEL1_NOTE_ON 144
#define CHANNEL1_NOTE_OFF 128
#define CHANNEL1_CONTROL_CHANGE 176

// fm specific macro
#define DISPLAY_CODE_LENGTH 10

#define OP1 5
#define OP2 4
#define OP3 3
#define OP4 2
#define OP5 1
#define OP6 0
#define ALL -1

#define OPERATORS_NUMBER 6

#define EG_RATE_1 0
#define EG_RATE_2 1
#define EG_RATE_3 2
#define EG_RATE_4 3
#define EG_LEVEL_1 4
#define EG_LEVEL_2 5
#define EG_LEVEL_3 6
#define EG_LEVEL_4 7
#define KEYBOARD_LEVEL_SCALE_BREAK_POINT 8
#define KEYBOARD_LEVEL_SCALE_LEFT_DEPTH 9
#define KEYBOARD_LEVEL_SCALE_RIGHT_DEPTH 10
#define KEYBOARD_LEVEL_SCALE_LEFT_CURVE 11
#define KEYBOARD_LEVEL_SCALE_RIGHT_CURVE 12
#define KEYBOARD_RATE_SCALING 13
#define MODULATION_SENSITIVITY_AMPLITUDE 14
#define KEY_VELOCITY_SENSITIVITY 15
#define OUTPUT_LEVEL 16
#define OSCILLATOR_MODE 17
#define OSCILLATOR_FREQUENCY_COARSE 18
#define OSCILLATOR_FREQUENCY_FINE 19
#define DETUNE 20

#define PITCH_EG_RATE_1 0
#define PITCH_EG_RATE_2 1
#define PITCH_EG_RATE_3 2
#define PITCH_EG_RATE_4 3
#define PITCH_EG_LEVEL_1 4
#define PITCH_EG_LEVEL_2 5
#define PITCH_EG_LEVEL_3 6
#define PITCH_EG_LEVEL_4 7
#define ALGORITHM 8
#define FEEDBACK 9
#define OSCILLATOR_SYNC 10
#define LFO_SPEED 11
#define LFO_DELAY 12
#define LFO_PITCH_MODULATION_DEPTH 13
#define LFO_AMPLITUDE_MODULATION_DEPTH 14
#define LFO_SYNC 15
#define LFO_WAVE 16
#define MODULATION_SENSITIVITY_PITCH 17
#define TRANSPOSE 18

#endif // DEFINITIONS_HPP
