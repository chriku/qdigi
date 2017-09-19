all:
	avr-gcc -Wall -mmcu=attiny2313 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Os -o avr.elf avr.c
	avr-objcopy -j .text --dump-section .text=avr.bin avr.elf
	avr-objdump -d avr.elf > ref.txt
