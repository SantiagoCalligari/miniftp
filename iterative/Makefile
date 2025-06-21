CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11 -g
LDFLAGS =

SRCS = main.c arguments.c server.c utils.c pi.c dtp.c handlers.c signals.c session.c
OBJS = $(SRCS:.c=.o)
TARGET = miniftp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

