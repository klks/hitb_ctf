CC=gcc
CFLAGS=-O0 -m32 -lpthread
STRIP=strip
OUTPUT_BIN=daemon_klks.ctf
HEADERS=vm_processor.h vm_opcodes.h
OBJECTS=daemon.o vm_processor.o

all: $(OBJECTS) $(HEADERS)
	$(CC) -o $(OUTPUT_BIN) $(OBJECTS) $(CFLAGS)
	$(STRIP) -s $(OUTPUT_BIN)

source:
	$(CC) -S vm_processor.c

clean:
	rm daemon.o $(OUTPUT_BIN)
