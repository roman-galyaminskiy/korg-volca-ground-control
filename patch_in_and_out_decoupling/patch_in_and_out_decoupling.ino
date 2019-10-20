#include <MIDI.h>

typedef midi::MidiInterface<HardwareSerial> MidiInterface;

class Mediator {
public:
  MidiInterface* MIDI = new MidiInterface(Serial2);
  // MidiInterface MIDI(Serial2); error: 'Serial2' is not a type

  void start_midi() {
    MIDI->begin();
  }

  void test_func() {
//    Serial.println("test_func");
    if (MIDI->read()) {
       byte type = MIDI->getType();
       byte channel = MIDI->getChannel();
       byte number = MIDI->getData1();

       if (type < 248) {
         Serial.print(type, DEC);
         Serial.print(" ");
         Serial.print(channel, DEC);
         Serial.print(" ");
         Serial.print(number, DEC);
         Serial.println();
       }
     }
  }
};

Mediator m;

void setup() { 
   Serial.begin(115200);
     
   m.start_midi();
   
   Serial.println("test");
}

void loop() {
//    Serial.println("test");
    m.test_func();
}
