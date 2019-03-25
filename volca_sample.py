import mido
import time

pad_indexes = list(range(96, 104)) + list(range(112, 120))
pad_colors = {'black': 0, 'red': 15, 'orange': 19, 'yellow': 21, 'green': 24}

usb2midi = mido.open_output('USB2.0-MIDI 3')

################################################################################

class Param():
    def __init__(self, index, name, value=127):
        self.index = index
        self.name = name
        self.value = value


class Pad:
    def __init__(self, index, part, keyboard_mode=0, knob_mode=0):
        # controller specific attributes
        self.index = index
        self.keyboard_mode = keyboard_mode
        self.knob_mode = knob_mode
        self.velocity_sensivity = 0 # 0 - 1

        # synth specific attributes
        self.part = part

        self.params = [
            Param(7, 'level'),
            Param(47, 'attack', 0),
            Param(48, 'decay'),
            Param(43, 'speed', 63),
            Param(10, 'pan', 63),
            Param(40, 'start_point', 0),
            Param(41, 'length'),
            Param(42, 'high_cut')
        ]

    def update_parameter(self, param_index, value):
        self.params[param_index].value = value
        usb2midi.send(mido.Message('control_change', channel=self.part, control=self.params[param_index].index, value=self.params[param_index].value))

class Controller:
    def __init__(self, name):
        self.name = name
        self.input_ports = []
        self.output_ports = []
        ports_initialized = self.get_ports()
        if ports_initialized:
            print(f'Controller "{self.name}" was successfully initialized.')

    def get_ports(self):
        input_ports = None
        output_ports = None

        try:
            input_ports = mido.get_input_names()
            output_ports = mido.get_output_names()
        except:
            print('Error reading ports')
            return 0

        for port in input_ports:
            if self.name in port:
                self.input_ports.append(mido.open_input(port))

        if not(self.input_ports):
            print(f'No input ports found for "{self.name}"')

        for port in output_ports:
            if self.name in port:
                self.output_ports.append(mido.open_output(port))

        if not(self.output_ports):
            print(f'No output ports found for "{self.name}"')

        return 1

class Launchkey(Controller):
        def __init__(self):
            Controller.__init__(self, name='Launchkey Mini')

            self.extended_mode = 0
            self.remap_mode = 0
            self.keyboard_mode = 0
            self.knob_mode = 0

            self.pads = [
                Pad(96, 7), Pad(97, 6), Pad(98, 1),Pad(99, 5), Pad(100, 5), Pad(101, 8), Pad(102, 6), Pad(103, 7),
                Pad(112, 2), Pad(113, 3), Pad(114, 0), Pad(115, 4), Pad(116, 4), Pad(117, 0), Pad(118, 3), Pad(119, 2)
            ]

            self.enable_extended_mode()

        def enable_extended_mode(self):
            # Request InContol
            msg = mido.Message('note_on', channel=0, note=12, velocity=127)
            self.output_ports[1].send(msg)

            # Check status
            msg = mido.Message('note_on', channel=0, note=11, velocity=0)
            self.output_ports[1].send(msg)

            status = None
            status_request_time = time.time()
            while (status is None):
                current_time = time.time()
                if (current_time - status_request_time > 1):
                    sys.exit('ERROR: Could  not enable InControl mode!')
                status = self.input_ports[1].poll()

            self.extended_mode = 1
            # print('INFO: InControl mode is enabled')

        def enable_keyboard_mode(self, pad):
            # print('enable_keyboard_mode')
            self.remap_mode = 0
            self.knob_mode = 0
            pad.keyboard_mode = 1
            self.keyboard_mode = pad.index

        def disable_keyboard_mode(self, pad):
            # print('disable_keyboard_mode')
            pad.keyboard_mode = 0
            self.keyboard_mode = 0

        def enable_knob_mode(self, pad):
            # print('enable_knob_mode')
            self.remap_mode = 0
            pad.knob_mode = 1
            self.keyboard_mode = 0
            self.knob_mode = pad.index

        def disable_knob_mode(self, pad):
            # print('disable_knob_mode')
            pad.knob_mode = 0
            self.knob_mode = 0

        def send_pad_message(self, pad, velocity):
            # usb2midi.send(mido.Message('control_change', channel=pad.part, control=7, value=velocity))
            # usb2midi.send(mido.Message('control_change', channel=pad.part, control=7, value=velocity))

            # for param in pad.params:
            #     if param.name == 'level':
            #         level = int(param.value * (1 - pad.velocity_sensivity) + param.value * pad.velocity_sensivity * velocity / 127.0)
            #         # print(pad.velocity_sensivity, velocity, level)
            #         usb2midi.send(mido.Message('control_change', channel=pad.part, control=param.index, value=level))
            #     else:
            #         usb2midi.send(mido.Message('control_change', channel=pad.part, control=param.index, value=param.value))

            # print(pad.part)
            usb2midi.send(mido.Message('note_on', channel=pad.part, note=60, velocity=127))

        def render(self):
            if self.remap_mode > 1:
                for i in list(range(96, 104)):
                    self.output_ports[1].send(mido.Message('note_on', note=i, velocity=pad_colors['green']))
                for i in list(range(112, 114)):
                    self.output_ports[1].send(mido.Message('note_on', note=i, velocity=pad_colors['green']))
                for i in list(range(114, 120)):
                    self.output_ports[1].send(mido.Message('note_on', note=i, velocity=pad_colors['black']))
            else:
                for pad in self.pads:
                    if (pad.index == self.keyboard_mode):
                        self.output_ports[1].send(mido.Message('note_on', note=pad.index, velocity=pad_colors['red']))
                    elif (pad.index == self.knob_mode):
                        self.output_ports[1].send(mido.Message('note_on', note=pad.index, velocity=pad_colors['orange']))
                    else:
                        self.output_ports[1].send(mido.Message('note_on', note=pad.index, velocity=pad_colors['yellow']))

            if self.knob_mode:
                self.output_ports[1].send(mido.Message('note_on', note=104, velocity=pad_colors['red']))

            if self.keyboard_mode:
                self.output_ports[1].send(mido.Message('note_on', note=120, velocity=pad_colors['red']))

            if self.remap_mode:
                self.output_ports[1].send(mido.Message('note_on', note=104, velocity=pad_colors['red']))
                self.output_ports[1].send(mido.Message('note_on', note=120, velocity=pad_colors['red']))

            if (self.remap_mode == 0 and self.knob_mode == 0 and self.keyboard_mode == 0):
                self.output_ports[1].send(mido.Message('note_on', note=104, velocity=pad_colors['black']))
                self.output_ports[1].send(mido.Message('note_on', note=120, velocity=pad_colors['black']))

            if (self.remap_mode == 0 and self.knob_mode == 0):
                self.output_ports[1].send(mido.Message('note_on', note=104, velocity=pad_colors['black']))

            if (self.remap_mode == 0 and self.keyboard_mode == 0):
                self.output_ports[1].send(mido.Message('note_on', note=120, velocity=pad_colors['black']))

        def listen(self):
            while True:
                keyboard_msg = self.input_ports[0].poll()
                if keyboard_msg:
                    if (keyboard_msg.type == 'note_on'):
                        if self.keyboard_mode:
                            if (self.keyboard_mode >= 96 and self.keyboard_mode <= 104):
                                pad_index = self.keyboard_mode - 96
                            elif (self.keyboard_mode >= 112 and self.keyboard_mode <= 120):
                                pad_index = 8 + self.keyboard_mode - 112
                            else:
                                pad_index = None

                            if pad_index is not None:
                                keyboard_msg.channel = self.pads[pad_index].part
                                usb2midi.send(keyboard_msg)

                desk_msg = self.input_ports[1].poll()
                if desk_msg is not None:
                    # If buttom or pad were pressed
                    if (desk_msg.type == 'note_on'):
                        # Identify pad
                        if (desk_msg.note >= 96 and desk_msg.note < 104):
                            pad_index = desk_msg.note - 96
                        elif (desk_msg.note >= 112 and desk_msg.note < 120):
                            pad_index = 8 + desk_msg.note - 112
                        else:
                            pad_index = None

                        # If pad was pressed
                        if pad_index is not None:
                            #  If 'remap_mode' is not on, trigger sampler
                            if self.remap_mode == 0:
                                self.send_pad_message(self.pads[pad_index], desk_msg.velocity)

                            # remap_mode == 1 means that we've entered 'remap_mode', and we should
                            # select a pad
                            elif self.remap_mode == 1:
                                    self.remap_mode = desk_msg.note
                                    # Render part selection scene and quit the current loop
                                    self.render()
                                    continue

                            # 'remap_mode' is not and we should select a part we want to to the pad
                            # selected on the previous step
                            else:
                                if pad_index < 10:
                                    if (self.remap_mode >= 96 and self.remap_mode < 104):
                                        edit_pad_index = self.remap_mode - 96
                                    elif (self.remap_mode >= 112 and self.remap_mode < 120):
                                        edit_pad_index = 8 + self.remap_mode - 112
                                    else:
                                        edit_pad_index = None

                                    if edit_pad_index is not None:
                                        self.pads[edit_pad_index].part = pad_index

                                self.remap_mode = 0

                        # If button was pressed
                        else:
                            # To quit loop press "InControl" button
                            if (desk_msg.channel == 0 and desk_msg.note == 10 and desk_msg.velocity == 0):
                                return

                            # Handling play buttons presses

                            # 1. Pressing top 'play' button and bottom button successively you enters]/exit 'remap_mode'
                            # 2. Pressing top 'play' button and pad successively you enter knob mode. Presss top 'play' button again to exit.
                            # 3. Pressing bottom 'play' button and pad successively you enter knob mode. Presss bottom 'play' button again to exit.
                            if (desk_msg.channel == 0 and desk_msg.note == 104 and desk_msg.velocity == 127):
                                # Exit knob mode
                                if self.knob_mode:
                                    if (self.knob_mode >= 96 and self.knob_mode < 104):
                                        pad_index = self.knob_mode - 96
                                    elif (self.knob_mode >= 112 and self.knob_mode < 120):
                                        pad_index = 8 + self.knob_mode - 112
                                    else:
                                        pad_index = None

                                    if pad_index is not None:
                                        self.disable_knob_mode(self.pads[pad_index])


                                else:
                                    # Enter/exit 'remap_mode'
                                    desk_msg = self.input_ports[1].receive()
                                    if (desk_msg.channel == 0 and desk_msg.note == 120 and desk_msg.velocity == 127):
                                        if (self.remap_mode == 0):
                                            self.remap_mode = 1
                                            self.knob_mode = 0
                                            self.keyboard_mode = 0
                                        else:
                                            self.remap_mode = 0

                                    # Enter 'knob_mode'
                                    if (desk_msg.note >= 96 and desk_msg.note < 104):
                                        pad_index = desk_msg.note - 96
                                    elif (desk_msg.note >= 112 and desk_msg.note < 120):
                                        pad_index = 8 + desk_msg.note - 112
                                    else:
                                        pad_index = None

                                    if pad_index is not None:
                                        self.enable_knob_mode(self.pads[pad_index])

                            if (desk_msg.channel == 0 and desk_msg.note == 120 and desk_msg.velocity == 127):
                                # Exit 'keyboard_mode'
                                if self.keyboard_mode:
                                    if (self.keyboard_mode >= 96 and self.keyboard_mode < 104):
                                        pad_index = self.keyboard_mode - 96
                                    elif (self.keyboard_mode >= 112 and self.keyboard_mode < 120):
                                        pad_index = 8 + self.keyboard_mode - 112
                                    else:
                                        pad_index = None

                                    if pad_index is not None:
                                        self.disable_keyboard_mode(self.pads[pad_index])

                                else:
                                    desk_msg = self.input_ports[1].receive()

                                    # Enter 'keyboard_mode'
                                    if (desk_msg.note >= 96 and desk_msg.note < 104):
                                        pad_index = desk_msg.note - 96
                                    elif (desk_msg.note >= 112 and desk_msg.note < 120):
                                        pad_index = 8 + desk_msg.note - 112
                                    else:
                                        pad_index = None

                                    if pad_index is not None:
                                        self.enable_keyboard_mode(self.pads[pad_index])

                    # Knob values are passed to the sampler only in 'knob_mode'
                    if (desk_msg.type == 'control_change' and self.knob_mode):
                        if (self.knob_mode >= 96 and self.knob_mode <= 104):
                            pad_index = self.knob_mode - 96
                        elif (self.knob_mode >= 112 and self.knob_mode <= 120):
                            pad_index = 8 + self.knob_mode - 112
                        else:
                            pad_index = None

                        if pad_index is not None:
                            if (desk_msg.control == 21):
                                self.pads[pad_index].velocity_sensivity = desk_msg.value / 127.0
                            else:
                                self.pads[pad_index].update_parameter(desk_msg.control - 22, desk_msg.value)

                lk.render()

lk = Launchkey()
lk.listen()
