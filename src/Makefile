.PHONY: all clean

all: bin_dir test ledfill

CROSS_TOOLS:=OFF
SOCK_UDP:=OFF
CCFLAGS=-std=gnu99
BIN_DIR:=../bin
RPI_WS281X_DEP=$(addprefix ../rpi_ws281x/,mailbox.c ws2811.c pwm.c pcm.c dma.c rpihw.c)

clean:
	rm -frv $(BIN_DIR)/*

ifeq ($(CROSS_TOOLS), ON)
	CC=../tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
endif

ifeq ($(SOCK_UDP), ON)
	CCFLAGS+=-DSOCK_UDP
endif

bin_dir:
	mkdir -pv $(BIN_DIR)

test: main.c $(RPI_WS281X_DEP)
	$(CC) $(CCFLAGS) -o $(BIN_DIR)/$@ -I ../rpi_ws281x -pthread $^ -lm

ledfill: test_client.c
	$(CC) $(CCFLAGS) -o $(BIN_DIR)/$@ -I ../rpi_ws281x $^

ledfill_remote: test_client.c
	gcc -std=gnu99 -DSOCK_UDP -o $(BIN_DIR)/$@ -I ../rpi_ws281x $^
