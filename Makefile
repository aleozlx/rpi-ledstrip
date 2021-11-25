.PHONY: all clean

all: dirs test ledfill

CROSS_TOOLS:=OFF
SOCK_UDP:=OFF
CCFLAGS=-std=gnu99
BIN_DIR=bin
SRC_DIR=src
INCLUDE_DIR=src rpi_ws281x

ifeq ($(CROSS_TOOLS), ON)
	CC=tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
	BIN_DIR=bin_dev
endif

ifeq ($(SOCK_UDP), ON)
	CCFLAGS+=-DSOCK_UDP
endif

clean:
	rm -frv $(BIN_DIR)/*

dirs:
	mkdir -pv $(BIN_DIR)

RPI_WS281X_DEP=$(addprefix rpi_ws281x/,mailbox.c ws2811.c pwm.c pcm.c dma.c rpihw.c)
INTERNAL_DEP=$(addprefix $(BIN_DIR)/,protocol.o)
INCLUDE_FLAGS=$(addprefix -I ,$(INCLUDE_DIR))

test: $(SRC_DIR)/main.c $(INTERNAL_DEP) $(RPI_WS281X_DEP)
	$(CC) $(CCFLAGS) -o $(BIN_DIR)/$@ $(INCLUDE_FLAGS) -pthread $^ -lm

ledfill: $(SRC_DIR)/test_client.c $(INTERNAL_DEP)
	$(CC) $(CCFLAGS) -o $(BIN_DIR)/$@ $(INCLUDE_FLAGS) $^

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CCFLAGS) -c -o $@ $(INCLUDE_FLAGS) -pthread $<
