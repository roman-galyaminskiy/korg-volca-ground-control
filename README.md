korg-volca-ground-control

Editing patch with just one knob is a very tedious task on Korg Volca FM. I wanted to create a more user friendly workflow, that will not require a PC. At this time I had USB controller with 8 knobs and 16 pads, but without MIDI out port, so I decided to use it together with Arduino to implement a kind of USB-MIDI conveter, that will allow me to dive in FM patch with much ease.

Using 16 5-color backlight pads to show and navigate a menu and 8 knobs to change patch parameters you could reach any part of patch with just a few taps. Menu has 3 "screens":

1. Patch subentity selection screen (OP/ALL, also allows you to turn each OP on and off completely)
2. Parameter groupp selection screen (Divided OP/ALL parameters in groups of atmost 8 params)
3. Parameter edit screen
