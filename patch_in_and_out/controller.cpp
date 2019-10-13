#include "controller.hpp"

USB Usb;
USBH_MIDI Midi(&Usb);

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

char Controller::getExtenderModeStatus() { return extended_mode; }

void Controller::enableExtendedMode() {
  uint8_t msg[3] = {CHANNEL1_NOTE_ON, 12, 0};

  // TODO: Status check
  // uint8_t msg[3] = {CHANNEL1_NOTE_ON, 11, 0};
  // Midi.SendData(msg, 1);
  // delay(1);
  // MIDI_poll();

  // Off
  Usb.Task();
  Midi.SendData(msg, 1);
  delay(1);

  // On
  msg[2] = 127;
  Usb.Task();
  Midi.SendData(msg, 1);
  delay(1);

  extended_mode = 1;
  // SERIAL_MONITOR.println("Extended mode enabled");
}

void Controller::listen() {
  uint8_t msg[3];
  char size;

  Usb.Task();
  if ( (size = Midi.RecvData(msg)) > 0 ) {
    SERIAL_MONITOR.print(msg[0], DEC);
    SERIAL_MONITOR.print(" ");
    SERIAL_MONITOR.print(msg[1], DEC);
    SERIAL_MONITOR.print(" ");
    SERIAL_MONITOR.print(msg[2], DEC);
    SERIAL_MONITOR.println();

    // We pass "note on" and "note off" as they come, but "control change"
    // event from knobs require additional processing. If we pass them as they
    // come 0 to 127, for exmaple, all 127 messages will be sent to synth,
    // and not very fast at reading them. That why we sent knob data only step
    // after the last received "control change" messsage

    if (msg[0] == CHANNEL1_NOTE_ON) {
      if (msg[1] >= PAD1 and msg[1] <= PAD8 or msg[1] >= PAD9 and msg[1] <= PAD16
        or msg[1] == TOP_PLAY or msg[1] == BOTTOM_PLAY) {
      }
      // Keyboard
      else {
        SERIAL_MONITOR.println("sendNoteOn");
        MIDI.sendNoteOn(msg[1], msg[2], 1);

        // SERIAL_MONITOR.print(msg[0], DEC);
        // SERIAL_MONITOR.print(" ");
        // SERIAL_MONITOR.print(msg[1], DEC);
        // SERIAL_MONITOR.print(" ");
        // SERIAL_MONITOR.print(msg[2], DEC);
        // SERIAL_MONITOR.println();
      }
    }
    else if (msg[0] == CHANNEL1_NOTE_OFF) {
      if (msg[1] >= PAD1 and msg[1] <= PAD8 or msg[1] >= PAD9 and msg[1] <= PAD16
        or msg[1] == TOP_PLAY or msg[1] == BOTTOM_PLAY) {
      }
      // Keyboard
      else {
        SERIAL_MONITOR.println("sendNoteOff");
        MIDI.sendNoteOff(msg[1], 0, 1);
      }
    }
  }
  delay(1);
}

void Controller::setup() {
  MIDI.begin(1);
  MIDI.setHandleSystemExclusive(handle_sysex);

  if (Usb.Init() == -1) {
    while (1); //halt
  }//if (Usb.Init() == -1...
  delay( 200 );
  Serial.println("USB initialized successfully");
}

void Controller::loop() {
  // MIDI.read();

   if (MIDI.read()) {
      byte type = MIDI.getType();
      byte channel = MIDI.getChannel();
      byte number = MIDI.getData1();

      if (type < 248) {
        Serial.print(type, DEC);
        Serial.print(" ");
        Serial.print(channel, DEC);
        Serial.print(" ");
        Serial.print(number, DEC);
        Serial.println();
      }
      else {
        listen();
      }
  }


  // if (getExtenderModeStatus() == 0) {
  //   Usb.Task();
  //   if ( Usb.getUsbTaskState() == USB_STATE_RUNNING ) {
  //     enableExtendedMode();
  //     Serial.println("Controller is ready to use");
  //   }
  // }
  // else {
  //   listen();
  // }
}
