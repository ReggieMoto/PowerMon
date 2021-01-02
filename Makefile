default: PowerMon

CC = gcc
CFLAGS = -g -Wall -Og
INCS = -Iinclude -I../avahi

SRCS = source/main.c
SRCS += source/powermon.c
SRCS += source/powermon_logger.c
SRCS += source/powermon_time.c
SRCS += source/powermon_calc.c
SRCS += source/powermon_socket.c
SRCS += source/powermon_fileio.c
SRCS += source/powermon_svc.c
SRCS += source/console_io.c
SRCS += source/device_io.c
SRCS += source/device_io_data.c
SRCS += source/user_io.c
SRCS += source/user_io_fsm.c
SRCS += source/xconsole_io.c
SRCS += source/msg_queues.c
SRCS += source/data_store.c
SRCS += source/sha3.c
SRCS += source/screens/config.c
SRCS += source/screens/login.c
SRCS += source/screens/report.c
SRCS += source/screens/top_menu.c
SRCS += source/screens/user.c

OBJS = $(SRCS:.c=.o)

LIBS = -pthread -L/usr/lib/x86_64-linux-gnu -lX11 -lrt -lavahi-core -lavahi-client -lavahi-common
LFLAGS = 

MAIN = PowerMon

.PHONY: depend clean

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCS) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCS) -c $<  -o $@

clean:
	$(RM) source/*.o $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
