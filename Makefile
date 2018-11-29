CFLAGS?=$(shell pkg-config --cflags libsysfs) -Wall -std=c99
LDFLAGS?=$(shell pkg-config --libs libsysfs)
PREFIX ?= /usr/local

all: sysbacklight

sysbacklight: sysbacklight.o
	$(CC) -o $@ $< $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

install:
	install -m 4755 sysbacklight $(DESTDIR)$(PREFIX)/bin 



clean:
	rm -f sysbacklight sysbacklight.o

