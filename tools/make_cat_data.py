import sys
import re
import argparse


parser = argparse.ArgumentParser(description='Cat binary frame format generator.')
parser.add_argument('frames', nargs='+', type=str, help='Frame input files')
parser.add_argument('output', type=str, help='Output file')
args = parser.parse_args()

out = b""

colors = {}
colors[',']  = "\033[48;5;17m"
colors['^']  = "\033[48;5;231m"
colors['\''] = "\033[48;5;16m"
colors['@']  = "\033[48;5;230m"
colors['$']  = "\033[48;5;175m"
colors['-']  = "\033[48;5;162m"
colors['>']  = "\033[48;5;196m"
colors['&']  = "\033[48;5;214m"
colors['+']  = "\033[48;5;226m"
colors['#']  = "\033[48;5;118m"
colors['=']  = "\033[48;5;33m"
colors[';']  = "\033[48;5;19m"
colors['*']  = "\033[48;5;240m"
colors['%']  = "\033[48;5;175m"

lines = []
line_count = None

for f_name in args.frames:
    with open(f_name) as f:
        file_parts = []

        file_lines = 0
        for i, line in enumerate(f.read().split('\n')[23:43]):
            if len(line.strip()) == 0:
                continue

            file_lines += 1
            line = re.sub(r'[#;=&>+.]', ',', line)
            line = line[17:49]
            curr = None
            line_data = ''

            for j, ch in enumerate(line):
                if curr != ch and (j > 0):
                    # Color changed
                    # print(colors[ch], end="")
                    line_data += colors[ch]
                curr = ch
                # print("  ", end="")
                line_data += '  '
            # print("\033[0m")
            lines.append(line_data)


        if line_count is None:
            line_count = file_lines
        elif line_count != file_lines:
            raise Exception('Line count mismatch in file {}'.format(f_name))


lines = list(map(lambda x: x.encode('utf-8'), lines))
pad_l = max(map(lambda x: len(x), lines)) + 1


for i, line in enumerate(lines):
    print(u'L {} @ 0x{:x} - Padded: {:03d} - \u001b[41m {}'.format(i, i * pad_l, pad_l - len(line), line.decode('utf-8')))
    row = line + (pad_l - len(line)) * b'\x00'
    out += row

with open(args.output, 'wb') as fh:
    fh.write(out)

print("\033[0m")
print("Num lines (CAT_ROWS): {}".format(line_count))
print("Row Length (DCAT_ROW_SIZE): {} ".format(pad_l))
print("Num Frames (CAT_FRAMES): {} ".format(len(args.frames)))
