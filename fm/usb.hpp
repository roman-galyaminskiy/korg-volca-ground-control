#ifndef USB_HPP
#define USB_HPP

#include <usbh_midi.h>
#include <usbhub.h>
#include <SPI.h>

USB Usb;
USBH_MIDI Midi(&Usb);

#endif // USB_HPP
