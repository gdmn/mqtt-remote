TARGET = mqtt_dev_listener
LIBS = -lm
CC = cc
CFLAGS = -g -Wall
LDFLAGS=/usr/lib/libmosquitto.so.1 /lib/arm-linux-gnueabihf/libpthread.so.0
#LDFLAGS=/usr/lib/libmosquitto.so.1 /usr/lib/libpthread.so.0

.PHONY: all default clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@ $(LDFLAGS)

clean:
	-rm -f *.o
	-rm -f *~
	-rm -f $(TARGET)
