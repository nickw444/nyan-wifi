# export PATH=~/.platformio/penv/bin:$PATH

CAT_ADDR=0x50000
MELODY_ADDR=0xa0000

ESPPORT ?= /dev/cu.usbmodem1411
ESPBAUD ?= 115200

FIRMWARE_BIN=.pioenvs/esp01_1m/firmware.bin

default: upload

clean:
	[ -d target ] && rm -r target/ || true
	platformio run --target clean

build: $(FIRMWARE_BIN)

$(FIRMWARE_BIN): src/* platformio.ini
	platformio run

update:
	platformio update

upload_resources: flash_melody flash_cat

upload: $(FIRMWARE_BIN)
	esptool.py -p $(ESPPORT) --baud $(ESPBAUD) write_flash 0x00 $(FIRMWARE_BIN)

upload_all: upload upload_resources

erase:
	esptool.py -p $(ESPPORT) --baud $(ESPBAUD) erase_flash

target/melody.bin: resources/audio/freq_defs.txt resources/audio/melody.txt
	[ -d target ] || mkdir target
	python tools/make_tone_data.py resources/audio/freq_defs.txt resources/audio/melody.txt target/melody.bin

flash_melody: target/melody.bin
	esptool.py -p $(ESPPORT) --baud $(ESPBAUD) write_flash $(MELODY_ADDR) target/melody.bin

target/cat_frames.bin: resources/cat_frames/*.txt
	[ -d target ] || mkdir target
	python tools/make_cat_data.py resources/cat_frames/*.txt target/cat_frames.bin

flash_cat: target/cat_frames.bin
	esptool.py -p $(ESPPORT) --baud $(ESPBAUD) write_flash $(CAT_ADDR) target/cat_frames.bin

monitor:
	 miniterm.py --raw --eol LF $(ESPPORT) $(ESPBAUD)

mem_layout: $(FIRMWARE_BIN) target/cat_frames.bin target/melody.bin
	python tools/mem_layout.py $(FIRMWARE_BIN) target/cat_frames.bin target/melody.bin
