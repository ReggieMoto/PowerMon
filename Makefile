default: powermon

SOURCES := main.c
	
powermon:
	gcc $(SOURCES) -o powermon