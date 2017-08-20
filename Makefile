default: PowerMon

CC = gcc
CFLAGS = -g -Wall -Og
INCS = 

SRCS = main.c user_io.c wifi_io.c data_store.c
OBJS = $(SRCS:.c=.o)

LIBS = -rt -pthread
LFLAGS = 

MAIN = PowerMon

.PHONY: depend clean

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCS) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCS) -c $<  -o Debug/$@

clean:
	$(RM) Debug/*.o Debug/$(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
