.PHONY: all clean

all: test

clean:
	rm test

CROSS_TOOLS:=OFF

ifeq ($(CROSS_TOOLS), ON)
	CC=../tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
endif

RPI_WS281X_DEP=$(addprefix ../rpi_ws281x/,mailbox.c ws2811.c pwm.c pcm.c dma.c rpihw.c)

test: main.c $(RPI_WS281X_DEP)
	$(CC) -o $@ $^ -I ../rpi_ws281x -lm
