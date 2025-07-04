BOARD?=esp32:esp32:esp32da
PORT?= $(shell ls /dev/ttyUSB*)

.PHONY: default lint all flash clean

default: all flash clean

all:
	arduino-cli compile --fqbn $(BOARD) ./

flash:
	arduino-cli upload -p $(PORT) --fqbn $(BOARD)

clean:
	rm -f build

