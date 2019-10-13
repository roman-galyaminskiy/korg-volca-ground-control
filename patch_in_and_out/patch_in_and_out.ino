#include <MIDI.h>

struct MySettings : public midi::DefaultSettings
 {
    static const unsigned SysExMaxSize = 1024; // Accept SysEx messages up to 1024 bytes long.
 };

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial2, MIDI, MySettings);

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


void setup() {
  MIDI.begin(1);
  MIDI.setHandleSystemExclusive(handle_sysex);

  Serial.begin(115200);
}

void loop() {
  MIDI.read();
  //  if (MIDI.read()) {
  //     byte type = MIDI.getType();
  //     byte channel = MIDI.getChannel();
  //     byte number = MIDI.getData1();
  //
  //     if (type != 254 and type != 248) {
  //       Serial.print(type, DEC);
  //       Serial.print(" ");
  //       Serial.print(channel, DEC);
  //       Serial.print(" ");
  //       Serial.print(number, DEC);
  //       Serial.println();
  //     }
  // }
  // else {
  // }
}
