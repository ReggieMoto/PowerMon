default: powermon

CC := gcc
SOURCES := main.c user_io.c wifi_io.c data_store.c
OBJECTS := main.o user_io.o wifi_io.o data_store.o

powermon: $(OBJECTS)
	$(CC) $^ -o $@ -lpthread
