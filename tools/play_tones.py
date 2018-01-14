import sys
import struct
import time
import math
import numpy
import pyaudio
import argparse

parser = argparse.ArgumentParser(description='Tone player for playing ccc1718 tone binary format.')
parser.add_argument('tone_file', type=str, help='file to play')
args = parser.parse_args()

tones_file = open(args.tone_file, 'rb')
tones = []

s = struct.Struct('<HH')

def sine(frequency, length, rate):
    length = int(length * rate)
    factor = float(frequency) * (math.pi * 2) / rate
    return numpy.sin(numpy.arange(length) * factor)


def play_tone(stream, frequency=440, length=1, rate=44100):
    chunks = []
    chunks.append(sine(frequency, length, rate))
    chunk = numpy.concatenate(chunks) * 0.25
    stream.write(chunk.astype(numpy.float32).tostring())

if __name__ == '__main__':
    p = pyaudio.PyAudio()
    stream = p.open(format=pyaudio.paFloat32,
                    channels=1, rate=44100, output=1)

    while True:
        elem = tones_file.read(s.size)
        if not len(elem):
            break

        freq, duration = s.unpack(elem)
        tone_duration = (1000 / duration) * 1.2
        play_tone(stream, frequency=freq, length=tone_duration/500)
        pause_between_duration = tone_duration * .8;
        time.sleep(pause_between_duration / 500)

    stream.close()
    p.terminate()
