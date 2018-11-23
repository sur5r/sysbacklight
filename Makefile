CFLAGS?=$(shell pkg-config --cflags libsysfs) -Wall -std=c99
LDFLAGS?=$(shell pkg-config --libs libsysfs)

all: sysbacklight

sysbacklight: sysbacklight.o
	$(CC) -o $@ $< $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

install:
	install -m 4755 sysbacklight /usr/bin/

clean:
	rm -f sysbacklight sysbacklight.o

