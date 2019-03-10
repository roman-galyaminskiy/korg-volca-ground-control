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

#define BLACK 0
#define RED 15
#define ORANGE 19
#define YELLOW 21
#define GREEN 24

#define LEVEL 7
#define ATTACK 47
#define DECAY 48

#define CONTROLLER_REMAP_MODE_OFF -2
#define CONTROLLER_REMAP_MODE_SELECT_PAD -1

#define CONTROLLER_KEYBOARD_MODE_OFF -1
#define PAD_KEYBOARD_MODE_OFF 0
#define PAD_KEYBOARD_MODE_ON 1

#define CONTROLLER_KNOB_MODE_OFF -1
#define PAD_KNOB_MODE_OFF 0
#define PAD_KNOB_MODE_ON 1

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

  // lk specific members
  char index;
  char keyboard_mode = PAD_KEYBOARD_MODE_OFF;
  char knob_mode = PAD_KNOB_MODE_OFF;
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
  char remap_mode = CONTROLLER_REMAP_MODE_OFF;

  /* Allows playing selected sample chromatically using keyboard. When this mode is off
  lk keyboard is not responsive.
  1. To enter press bottom 'play' buttom and one of the pads pad successively
    * Selected pad turn RED
    * Flag is equal to pad note (ex: 96)
  2. To exit press bottom 'play' buttom one more time
    * Flag is equal 0
  */
  char knob_mode = CONTROLLER_KNOB_MODE_OFF;

  /* Allows editing selected sample parameter using knobs. When this mode is off
  lk knobs are not responsive.
  1. To enter press top 'play' buttom and one of the pads pad successively
    * Selected pad turn ORAN
    * Flag is equal to pad note (ex: 96)
  2. To exit press bottom 'play' buttom one more time
    * Flag is equal 0
  */
  char keyboard_mode = CONTROLLER_KEYBOARD_MODE_OFF;

  Pad pads[16] = {
    {PAD1, 7}, {PAD2, 6}, {PAD3, 1}, {PAD4, 5}, {PAD5, 5}, {PAD6, 8}, {PAD7, 6}, {PAD8, 7},
    {PAD9, 2}, {PAD10, 3}, {PAD11, 0}, {PAD12, 4}, {PAD13, 4}, {PAD14, 0}, {PAD15, 3}, {PAD16, 2}
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

  void controller_disable_keyboard_mode() {
    for (char i = 0; i < 16; i++) {
      pads[i].keyboard_mode = PAD_KEYBOARD_MODE_OFF;
    }
    keyboard_mode = CONTROLLER_KEYBOARD_MODE_OFF;
    controller_render_scene();
  }

  void controller_disable_knob_mode() {
    for (char i = 0; i < 16; i++) {
      pads[i].knob_mode = PAD_KNOB_MODE_OFF;
    }
    knob_mode = CONTROLLER_KNOB_MODE_OFF;
    controller_render_scene();
  }

  void controller_enable_keyboard_mode(char pad_index) {
    if (keyboard_mode == CONTROLLER_KEYBOARD_MODE_OFF) {
      remap_mode = CONTROLLER_REMAP_MODE_OFF;
      keyboard_mode = pad_index;
      pads[pad_index].keyboard_mode = PAD_KEYBOARD_MODE_ON;

      controller_disable_knob_mode();
    }
  }

  void controller_enable_knob_mode(char pad_index) {
    if (knob_mode == CONTROLLER_KNOB_MODE_OFF) {
      remap_mode = CONTROLLER_REMAP_MODE_OFF;
      knob_mode = pad_index;
      pads[pad_index].knob_mode = PAD_KNOB_MODE_ON;

      controller_disable_keyboard_mode();
    }
  }

  void controller_map_pad_to_voice(char voice_index) {
    if (voice_index > 9) {
      voice_index = -1;
    }
    pads[remap_mode].voice = voice_index;
    remap_mode = CONTROLLER_REMAP_MODE_OFF;
    controller_render_scene();
  }

  void controller_render_active_pads() {
    // 'note_on' message templape
    uint8_t msg[3] = {144, 0, 0};

    for (char i = 0; i < 16; i++) {
      msg[1] = pads[i].index;
      if (pads[i].voice > -1) {
        if (pads[i].knob_mode == PAD_KNOB_MODE_ON) {
          msg[2] = ORANGE;
        }
        else if (pads[i].keyboard_mode == PAD_KEYBOARD_MODE_ON) {
          msg[2] = RED;
        }
        else {
          msg[2] = YELLOW;
        }

      }
      else {
        msg[2] = BLACK;
      }
      Usb.Task();
      Midi.SendData(msg, 1);
      delay(1);
    }
  }

  void controller_render_voice_select() {
    // 'note_on' message templape
    uint8_t msg[3] = {144, 0, 0};

    for (char i = 0; i < 16; i++) {
      msg[1] = pads[i].index;
      if (pads[i].index >= PAD1 and pads[i].index <= PAD8 or pads[i].index >= PAD9 and pads[i].index <= PAD10) {
        msg[2] = GREEN;
      }
      else {
        msg[2] = BLACK;
      }
      Usb.Task();
      Midi.SendData(msg, 1);
      delay(1);
    }
  }

  void controller_render_play_buttons() {
    // 'note_on' message templape
    uint8_t top_play_msg[3] = {144, TOP_PLAY, 0};
    uint8_t bottom_play_msg[3] = {144, BOTTOM_PLAY, 0};

    char rc;

    if (knob_mode > CONTROLLER_KNOB_MODE_OFF) {
      top_play_msg[2] = RED;
    }

    if (keyboard_mode > CONTROLLER_KEYBOARD_MODE_OFF) {
      bottom_play_msg[2] = RED;
    }

    if (remap_mode > CONTROLLER_REMAP_MODE_OFF) {
      top_play_msg[2] = RED;
      bottom_play_msg[2] = RED;
    }

    if (remap_mode == CONTROLLER_REMAP_MODE_OFF and knob_mode == CONTROLLER_KNOB_MODE_OFF) {
      top_play_msg[2] = BLACK;
    }

    if (remap_mode == CONTROLLER_REMAP_MODE_OFF and keyboard_mode == CONTROLLER_KEYBOARD_MODE_OFF) {
      bottom_play_msg[2] = BLACK;
    }

    Usb.Task();
    rc = Midi.SendData(top_play_msg, 1);
    Usb.Task();
    rc = Midi.SendData(bottom_play_msg, 1);
  }

  void controller_render_scene() {
    if (remap_mode > 0) {
      controller_render_voice_select();
    }
    else {
      controller_render_active_pads();
    }

    controller_render_play_buttons();
  }
};

Controller lk;

void setup() {
  // put your setup code here, to run once:
  // Serial.begin(9600);
  _MIDI_SERIAL_PORT.begin(31250);

  // Serial.println("check1");
  if (Usb.Init() == -1) {
    while (1); //halt
  }//if (Usb.Init() == -1...
  delay( 200 );

  lk.extended_mode = 0;
}

char top_play_button_pressed = 0;
char bottom_play_button_pressed = 0;

void loop() {


  if (lk.extended_mode == 0) {
    Usb.Task();
    if ( Usb.getUsbTaskState() == USB_STATE_RUNNING ) {
      lk.controller_enable_extended_mode();
    }

    lk.controller_render_scene();
  }
  // put your main code here, to run repeatedly:
  Usb.Task();
  MIDI_poll();

  delay(10);
}

// Poll USB MIDI Controler and send to serial MIDI
void MIDI_poll()
{
  uint8_t outBuf[ 3 ];
  uint8_t size;

  char pad_index;

  extern char top_play_button_pressed;
  extern char bottom_play_button_pressed;

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
        if (outBuf[1] >= PAD1 and outBuf[1] <= PAD8) {
            pad_index = outBuf[1] - PAD1;
        }
        else if (outBuf[1] >= PAD9 and outBuf[1] <= PAD16){
            pad_index = 8 + outBuf[1] - PAD9;
        }
        else {
            pad_index = -1;
        }

        if (pad_index > -1) {
          // Serial.print("lk.remap_mode ");
          // Serial.print(lk.remap_mode, DEC);
          // Serial.print(", top_play_button_pressed ");
          // Serial.print(top_play_button_pressed, DEC);
          // Serial.print(", bottom_play_button_pressed ");
          // Serial.print(bottom_play_button_pressed, DEC);
          // Serial.println();
          if (lk.remap_mode == CONTROLLER_REMAP_MODE_SELECT_PAD) {
            lk.remap_mode = pad_index;
          }
          else if (lk.remap_mode > CONTROLLER_REMAP_MODE_SELECT_PAD) {
            lk.controller_map_pad_to_voice(pad_index);
          }
          else if (top_play_button_pressed == 1) {
            lk.controller_enable_knob_mode(pad_index);
          }
          else if (bottom_play_button_pressed == 1) {
            lk.controller_enable_keyboard_mode(pad_index);
          }
          else {
            if (lk.pads[pad_index].voice > -1) {
              outBuf[0] += lk.pads[pad_index].voice;
              _MIDI_SERIAL_PORT.write(outBuf, size);
            }
          }
        }

        // Identify button
        else if (outBuf[1] == TOP_PLAY or outBuf[1] == BOTTOM_PLAY) {
          if (outBuf[1] == TOP_PLAY) {
            if (lk.knob_mode > CONTROLLER_KNOB_MODE_OFF) {
              lk.controller_disable_knob_mode();
            }
            else {
              top_play_button_pressed = 1;
            }
          }

          else if (outBuf[1] == BOTTOM_PLAY) {
            if (lk.keyboard_mode > CONTROLLER_KEYBOARD_MODE_OFF) {
              lk.controller_disable_keyboard_mode();
            }
            else {
              if (top_play_button_pressed == 1) {
                if (lk.remap_mode == CONTROLLER_REMAP_MODE_OFF) {
                  lk.remap_mode = CONTROLLER_REMAP_MODE_SELECT_PAD;
                }
                else {
                  lk.remap_mode = CONTROLLER_REMAP_MODE_OFF;
                }
                top_play_button_pressed = 0;
                bottom_play_button_pressed = 0;
              }
              else {
                bottom_play_button_pressed = 1;
              }
            }
          }
        }

      }

      else if (outBuf[0] == 128) {
        if (outBuf[1] == TOP_PLAY) {
          top_play_button_pressed = 0;
        }
        else if (outBuf[1] == BOTTOM_PLAY) {
          bottom_play_button_pressed = 0;
        }
      }

      lk.controller_render_scene();
    }

  } while (size > 0);
}
