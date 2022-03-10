all: main

main: tempsensor.o
	avr-gcc -mmcu=atmega32u4 -I lufa/VirtualSerial -I lufa/lufa-LUFA-140928 -L lufa/VirtualSerial_lib -DF_USB=16000000 -Os -Wall -o main tempsensor.o -lVirtualSerial

tempsensor.o: tempsensor.c
	avr-gcc -mmcu=atmega32u4 -I lufa/VirtualSerial -I lufa/lufa-LUFA-140928 -L lufa/VirtualSerial_lib -DF_USB=16000000 -Os -Wall -c tempsensor.c -lVirtualSerial

clean:
	rm *.o main

flash: main
	avrdude -c avr109 -b57600 -D -p atmega32u4 -P /dev/ttyACM0 -e -U flash:w:main.out
