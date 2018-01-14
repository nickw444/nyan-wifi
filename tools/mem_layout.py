"""
A python script to determine optimal memory layout for
flashing a number of assets.
"""
import argparse

parser = argparse.ArgumentParser(description='Determine optimal memory layout.')
parser.add_argument('firmware', type=str, help='Base firmware file flashed at 0x00')
parser.add_argument('data', type=str, nargs='+', help='Binary files to determine offset')
args = parser.parse_args()

firmware_size = len(open(args.firmware, 'rb').read())
sizes = list(map(lambda x: (x, len(open(x, 'rb').read())), args.data))

def print_header():
    print('{:<50}\tOffset\tSize'.format('Filename'))

def print_asset(name, offset, size):
    print('{:<50}\t0x{:04x}\t0x{:04x} ({} bytes)'.format(name, offset, size, size))


print_header()
print_asset(args.firmware, 0x00, firmware_size)

offset = firmware_size
for f, size in sizes:
    base = (offset + 0x10000) & ~0xFFFF
    print_asset(f, base, size)
    offset += base + size
