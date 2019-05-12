#ifndef USB_HPP
#define USB_HPP

#include <usbh_midi.h>
#include <usbhub.h>
#include <SPI.h>

//////////////////////////
// MIDI Pin assign
// 2 : GND
// 4 : +5V(Vcc) with 220ohm
// 5 : TX
//////////////////////////

USB Usb;
USBH_MIDI Midi(&Usb);

#endif // USB_HPP
