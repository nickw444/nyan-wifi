import struct
import argparse

parser = argparse.ArgumentParser(description='Binary tone format generator.')
parser.add_argument('freq_defs', type=str, help='Frequency definition file')
parser.add_argument('tone_file', type=str, help='Tone file to generate binary for')
parser.add_argument('output', type=str, help='Output file')
args = parser.parse_args()

tones_data = open(args.freq_defs).read().split('\n')
tones = {}

for line in tones_data:
    if len(line) == 0:
        continue
    (name, freq) = line.split(' ')
    tones[name] = int(freq)

def process_tones():
    length = 0
    input_data = open(args.tone_file).read().split('\n')
    output_data = b''
    for line in input_data:
        if len(line) == 0:
            continue
        (note_name, duration) = line.split(' ')
        output_data += struct.pack('<HH', tones[note_name], int(duration))
        length += 1

    with open(args.output, 'wb') as f:
        f.write(output_data)

    print("Wrote {} with length (MELODY_LENGTH): {} ({} bytes)".format(args.output, length, len(output_data)))

process_tones()
