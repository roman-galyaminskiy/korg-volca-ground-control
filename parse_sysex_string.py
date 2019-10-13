# s_sysex = input("Paste SysEx string: ")
s_sysex = '240  67  0  0  1  27  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  29  19  12  77  99  99  0  0  0  0  0  0  0  0  3  0  73  0  2  0  7  18  7  24  31  88  99  66  31  0  0  0  0  0  0  0  0  71  0  0  41  14  39  99  99  37  99  99  99  0  0  0  0  0  0  0  0  0  95  0  0  0  0  99  99  99  99  50  50  50  50  0  0  0  7  0  0  59  1  4  0  24  49  32  111  108  118  108  32  57  53  0  56  247'
a_sysex = s_sysex.strip().split()

# for index in range(len(a_sysex)):
#     a_sysex[index] = int(a_sysex[index])

def validate_patch(_a_sysex):
    sysex_begin = ['240', '67', '0', '0', '1', '27']
    sysex_stop = '247'

    if (len(_a_sysex)):
        print("Length OK")
    else:
        pass

    if (_a_sysex[:6] == sysex_begin):
        print("Begin OK")
    else:
        pass

    if (_a_sysex[-1] == sysex_stop):
        print("End OK")
    else:
        pass

operators_data = [[], [], [], [], [], []]
all_data = []
operators_power_status = ""
patch_name = ""

def parse_patch(a_sysex):
    global operators_data
    operators_data = [[], [], [], [], [], []]

    global all_data
    all_data = []

    global operators_power_status
    global patch_name

    # (\|\s+[0-9][0-9]?\s+\|\sOperator\s[1-6]\s)([a-zA-Z0-9 ]+)(.+)
    global operators_number
    operators_number = 6

    global operator_parameters
    operator_parameters = [
        "EG Rate 1", "EG Rate 2", "EG Rate 3", "EG Rate 4",
        "EG Level 1", "EG Level 2", "EG Level 3", "EG Level 4",
        "Keyboard Level Scale Break Point", "Keyboard Level Scale Left Depth",
        "Keyboard Level Scale Right Depth", "Keyboard Level Scale Left Curve",
        "Keyboard Level Scale Right Curve", "Keyboard Rate Scaling",
        "Modulation Sensitivity Amplitude", "Operator Key Velocity Sensitivity",
        "Operator Output Level", "Oscillator Mode", "Oscillator Frequency Coarse",
        "Oscillator Frequency Fine", "Detune"
    ]

    global all_parameters
    all_parameters = [
        "Pitch EG Rate 1", "Pitch EG Rate 2", "Pitch EG Rate 3",
        "Pitch EG Rate 4", "Pitch EG Level 1", "Pitch EG Level 2",
        "Pitch EG Level 3", "Pitch EG Level 4", "Algorithm", "Feedback",
        "Oscillator Sync", "LFO Speed", "LFO Delay", "LFO Pitch Modulation Depth",
        "LFO Amplitude Modulation Depth", "LFO Sync", "LFO Wave",
        "Modulation Sensitivity Pitch", "Transpose"
    ]

    # print(len(operator_parameters))
    # print(len(all_parameters))

    sysex_data = a_sysex[6:-1]

    i = 0
    for operator_index in range(operators_number):
        operators_data[5 - operator_index] = sysex_data[i: i + len(operator_parameters)]
        i = i + len(operator_parameters)

    all_data = sysex_data[i: i + len(all_parameters)]

    operators_power_status = format(int(sysex_data[-1]), "06b")

    patch_name = ''.join(chr(int(i)) for i in sysex_data[145:154])

    # print(operators_power_status)

def print_patch():
    print(patch_name)
    print()

    for operator_index in range(operators_number):
        # Print operator number and status
        separator = '#'
        print()
        print(separator * 15 + " Operator " + str(operator_index + 1) + " " +
            ("ON" if operators_power_status[operator_index] == '1' else "OFF")
            + " " + separator * 15)
        print()
        for parameter_index in range(len(operator_parameters)):
            print(operator_parameters[parameter_index] + ": " +
                operators_data[operator_index][parameter_index])

# validate(a_sysex)
parse_patch(a_sysex)
print_patch()
# print(len(a_sysex))


# | 145~154     | Voice Name 1~10                              | ASCII              |
#
# | 155   B5    | Operator 1 On/Off                            | 0~1                |
# |      -------+----------------------------------------------+--------------------+
# |       B4    | Operator 2 On/Off                            | 0~1                |
# |      -------+----------------------------------------------+--------------------+
# |       B3    | Operator 3 On/Off                            | 0~1                |
# |      -------+----------------------------------------------+--------------------+
# |       B2    | Operator 4 On/Off                            | 0~1                |
# |      -------+----------------------------------------------+--------------------+
# |       B1    | Operator 5 On/Off                            | 0~1                |
# |      -------+----------------------------------------------+--------------------+
# |       B0    | Operator 6 On/Off                            | 0~1                |
