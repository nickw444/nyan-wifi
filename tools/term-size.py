"""
A tool written to test VT100 escape codes for determining terminal size.
"""

import sys
import os

stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)
stdout.write('\033[999;999H\033[6n')

stdin = os.fdopen(sys.stdin.fileno(), 'r', 0)
data = stdin.read(1)
print("data", data)
