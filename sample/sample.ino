#include <usbh_midi.h>
#include <usbhub.h>
#include <SPI.h>

USB Usb;
USBH_MIDI  Midi(&Usb);

#ifdef USBCON
#define _MIDI_SERIAL_PORT Serial1
#else
#define _MIDI_SERIAL_PORT Serial
#endif
//////////////////////////
// MIDI Pin assign
// 2 : GND
// 4 : +5V(Vcc) with 220ohm
// 5 : TX
//////////////////////////

#define BLACK 0
#define RED 15
#define ORANGE 19
#define YELLOW 21
#define GREEN 24

#define LEVEL 7
#define ATTACK 47
#define DECAY 48

void MIDI_poll();

struct Param {
  char index;
  char value;
};

struct Pad {
  /* Pad is mapped to one of the voices. Two different pads could be
  mapped to the same voice, so each pad stores not only voice index,
  but also parameters for the mapped voice. Each time pad triggers sampler
  to produce sound, 'control_change' maessage for every parameter are send
  prior to 'note_on' message. */

  // controller specific members
  char index;
  char keyboard_mode = 0;
  char knob_mode = 0;
  char velocity_sensivity = 0; // 0 - 127

  //  sampler specific members
  char voice; // 0-9
  Param params[3] = {
      {LEVEL, 127},
      {ATTACK, 0},
      {DECAY, 127}
  };

  Pad (char a, char b) {
    index = a;
    voice = b;
  }

};

struct Controller {
  // (aka 'InControl') Allows customizing pads behaviour
  char extended_mode = 0;

  /* Allows changing voice mapping for each pad
  1. To enter press top 'play' button and bottom 'play' button successively.
    * Both buttom will turn RED
    * Flag is equal 1
  2. Select a pad you want to change mapping for by pressing
    * Flag is equal to pad note (ex: 96)
    * Pad 1-10 will turn green inviting you to select a voice you want to
      map selected pad to
  3. Select pad 1-10 to map voice to pad and exit or select black pad
     to exit without remapping
    * Flag is equal 0

  You could exit mode anytime by pressing top 'play' button and bottom play' button successively
  one more time.

  You could you use knob and keyboard modes simultaneously, but when you enter remap mode,
  you exit knob and keyboard modes automaticaly */
  char remap_mode = 0;

  /* Allows playing selected sample chromatically using keyboard. When this mode is off
  controller keyboard is not responsive.
  1. To enter press bottom 'play' buttom and one of the pads pad successively
    * Selected pad turn RED
    * Flag is equal to pad note (ex: 96)
  2. To exit press bottom 'play' buttom one more time
    * Flag is equal 0
  */
  char knob_mode = 0;

  /* Allows editing selected sample parameter using knobs. When this mode is off
  controller knobs are not responsive.
  1. To enter press top 'play' buttom and one of the pads pad successively
    * Selected pad turn ORAN
    * Flag is equal to pad note (ex: 96)
  2. To exit press bottom 'play' buttom one more time
    * Flag is equal 0
  */
  char keyboard_mode = 0;

  Pad pads[16] = {
    {96, 7}, {97, 6}, {98, 1}, {99, 5}, {100, 5}, {101, 8}, {102, 6}, {103, 7},
    {112, 2}, {113, 3}, {114, 0}, {115, 4}, {116, 4}, {117, 0}, {118, 3}, {119, 2}
  };

  void controller_enable_extended_mode() {
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

    extended_mode = 1;
  }

  void controller_enable_keyborad_mode(Pad pad) {
    remap_mode = 0;
    knob_mode = 0;
    keyboard_mode = pad.index;
    pad.keyboard_mode = 1;
  }

  void controller_disable_keyborad_mode(Pad pad) {
    keyboard_mode = 0;
    pad.keyboard_mode = 0;
  }

  void controller_enable_knob_mode(Pad pad) {
    remap_mode = 0;
    knob_mode = pad.index;
    keyboard_mode = 0;
    pad.knob_mode = 1;
  }

  void controller_disable_knob_mode(Pad pad) {
    knob_mode = 0;
    pad.knob_mode = 0;
  }
};

Controller controller;

void setup() {
  // put your setup code here, to run once:
  // Serial.begin(9600);
  _MIDI_SERIAL_PORT.begin(31250);

  // Serial.println("check1");
  if (Usb.Init() == -1) {
    while (1); //halt
  }//if (Usb.Init() == -1...
  delay( 200 );

  controller.extended_mode = 0;
}

void loop() {
  if (controller.extended_mode == 0) {
    Usb.Task();
    if ( Usb.getUsbTaskState() == USB_STATE_RUNNING ) {
      controller.controller_enable_extended_mode();
    }

    // scene1.Render();
  }
  // put your main code here, to run repeatedly:
  Usb.Task();
  MIDI_poll();
}

// Poll USB MIDI Controler and send to serial MIDI
void MIDI_poll()
{
  uint8_t outBuf[ 3 ];
  uint8_t size;

  char pad_index;

  do {
    if ( (size = Midi.RecvData(outBuf)) > 0 ) {
      // Serial.print(outBuf[0]);
      // Serial.print(" ");
      // Serial.print(outBuf[1]);
      // Serial.print(" ");
      // Serial.println(outBuf[2]);

      // channel 1 note_on
      if (outBuf[0] == 144) {
        // Identify pad
        if (outBuf[1] >= 96 and outBuf[1] < 104) {
            pad_index = outBuf[1] - 96;
        }
        else if (outBuf[1] >= 112 and outBuf[1] < 120){
            pad_index = 8 + outBuf[1] - 112;
        }
        else {
            pad_index = -1;
        }

        if (pad_index > -1) {
          outBuf[0] += controller.pads[pad_index].voice;
          // Serial.println(outBuf[0], DEC);
          _MIDI_SERIAL_PORT.write(outBuf, size);
        }

      }
    }
  } while (size > 0);
}
