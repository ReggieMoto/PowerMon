default: powermon

SOURCES := main.c user_io.c wifi_io.c
	
powermon:
	gcc $(SOURCES) -o powermon