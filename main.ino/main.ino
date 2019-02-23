#include <usbh_midi.h>
#include <usbhub.h>
#include <SPI.h>
USB Usb;
USBHub Hub(&Usb);
USBH_MIDI  Midi(&Usb);

// #ifdef USBCON
// #define _MIDI_SERIAL_PORT Serial1
// #else
// #define _MIDI_SERIAL_PORT Serial
// #endif
//////////////////////////
// MIDI Pin assign
// 2 : GND
// 4 : +5V(Vcc) with 220ohm
// 5 : TX
//////////////////////////

void MIDI_poll();

struct Option {
  const char* name;
  char color;
  int (*handler)(); // pointer to function

  void callHandler() {
    int rc;
    rc = handler();
    Serial.println(rc, DEC);
  }
};

int one() {
  return 1;
}

int two() {
  return 2;
}

int three() {
  return 3;
}

int four() {
  return 4;
}

struct Pad {
  char index;
  char option_index;
  Option* options_array;
  char options_array_len;

  void SelectNextOption() {
    if (options_array_len == 0) {
      return;
    }
    if (option_index + 1 == options_array_len) {
      option_index = 0;
    }
    else {
      option_index++;
    }
    options_array[option_index].callHandler();
  }
};

struct Scene {
  char display_only_flg;

  Option power_options[2] = {{"off", 15, two}, {"on", 24, one}};
  Option eg_options[4] = {{"bass", 15, one}, {"piano", 19, two}, {"yellow", 21, three}, {"strings", 24, four}};

//  char pad_indexes[16] = {96, 97, 98, 99, 100, 101, 102, 103, 112, 113, 114, 115, 116, 117, 118, 119};

  Pad pads[16] = {
    {96, 0, power_options, 2},
    {97, 0, eg_options, 4},
    {98, 0, power_options, 2},
    {99, 0, eg_options, 4},
    {100, 0, power_options, 2},
    {101, 0, eg_options, 4},
    {102, 0, power_options, 2},
    {103, 0, eg_options, 4},
    {112, 0, 0, 0},
    {113, 0, 0, 0},
    {114, 0, 0, 0},
    {115, 0, 0, 0},
    {116, 0, 0, 0},
    {117, 0, 0, 0},
    {118, 0, 0, 0},
    {119, 0, 0, 0}
  };

  void Render() {
    uint8_t msg[3] = {144, 0, 0};

   for (int i=0; i <= 16; i++){
      if (pads[i].options_array != 0) {
        msg[1] = pads[i].index;
        msg[2] = pads[i].options_array[pads[i].option_index].color;
        Midi.SendData(msg, 1);
        delay(1);
      }
   }
  }
};

struct Controller {
  char extended_mode_flg;
  Scene scenes[1];

  void EnableExtendedMode() {
    uint8_t msg[3] = {144, 12, 0};

    // Serial.println("Enable extended mode");

    // TODO: Status check
    /* uint8_t msg[3] = {144, 11, 0};
    Midi.SendData(msg, 1);
    delay(1);
    MIDI_poll(); */

    // Off
    Midi.SendData(msg, 1);
    // On
    delay(100);
    msg[2] = 127;
    Midi.SendData(msg, 1);

    extended_mode_flg = 1;
  }
};

Controller controller;
Scene scene1;

void setup()
{
  Serial.begin(9600);
  // _MIDI_SERIAL_PORT.begin(31250);

  // Serial.println("check1");
  if (Usb.Init() == -1) {
    while (1); //halt
  }//if (Usb.Init() == -1...
  delay( 200 );

  // Serial.println("check2");
  controller.extended_mode_flg = 0;
}

void loop() {
  if (controller.extended_mode_flg == 0) {
    Usb.Task();
    if ( Usb.getUsbTaskState() == USB_STATE_RUNNING ) {
      controller.EnableExtendedMode();
    }

    // scene1.Render();
  }

  Usb.Task();
  MIDI_poll();

  Usb.Task();
  scene1.Render();
  delay(100);
}

// Poll USB MIDI Controler and send to serial MIDI
void MIDI_poll()
{
  uint8_t outBuf[ 3 ];
  uint8_t size;

  char pad_index;

  do {
    pad_index = -1;

    if ( (size = Midi.RecvData(outBuf)) > 0 ) {
      /* Serial.print(outBuf[0]);
      Serial.print(" ");
      Serial.print(outBuf[1]);
      Serial.print(" ");
      Serial.println(outBuf[2]); */
      if (outBuf[1] >= 96 && outBuf[1] <= 103 && outBuf[0] == 144) {
        // Serial.println("Got top pad event");
        pad_index = outBuf[1] - 96;
      }
      else if (outBuf[1] >= 112 && outBuf[1] <= 119 && outBuf[0] == 144) {
        // Serial.println("Got bottom pad event");
        pad_index = outBuf[1] - 112;
      }
      else {
        // _MIDI_SERIAL_PORT.write(outBuf, size);
      }
    }

    if (pad_index > -1) {
      // Serial.println("Passing event to scene");
      scene1.pads[pad_index].SelectNextOption();
      scene1.Render();
    }
  } while (size > 0);
}
