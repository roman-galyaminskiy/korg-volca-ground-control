#ifndef PATCH_H
#define PATCH_H

#define OP1 5
#define OP2 4
#define OP3 3
#define OP4 2
#define OP5 1
#define OP6 0
#define ALL -1

#define OPERATORS_NUMBER 6

#define DISPLAY_CODE_LENGTH 10


struct Patch {
  char patch_name[10];

  Operator operators[6] = {{6}, {5}, {4}, {3}, {2}, {1}};
  All all;

  void beginSysex();
  void endSysex();
  void sendPatchData();
  void sendSysexMessage();
  void receiveSysex(byte* sysex_array, unsigned sysex_array_length);
};

#endif
