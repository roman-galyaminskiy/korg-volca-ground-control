.PHONY: clean flash

flash: main.hex
	avrdude -v -c arduino -P COM13 -p atmega328p -U flash:w:main.hex

main.hex: main.elf
	avr-objcopy -O ihex main.elf main.hex

main.elf: main.o
	avr-gcc -DF_CPU=16000000 -mmcu=atmega328p -o main.elf main.o

main.o: main.cpp
	avr-gcc -c main.cpp -Os -mmcu=atmega328p -DF_CPU=16000000UL

clean:
	rm main.hex main.elf main.o
