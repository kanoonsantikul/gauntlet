TARGET=usb-ghost
MCU=atmega168
F_CPU=16000000
CFLAGS=-mmcu=$(MCU) -Os -DF_CPU=$(F_CPU) -Iusbdrv -I.
OBJS=usbHID.o timer.o usbdrv/usbdrv.o usbdrv/usbdrvasm.o

.SECONDARY:

all: $(TARGET).hex

flash: $(TARGET).flash

%.flash: %.hex
	avrdude -p $(MCU) -c usbasp -u -U flash:w:$<

%.hex: %.elf
	avr-objcopy -j .text -j .data -O ihex $< $@

%.elf: %.o $(OBJS)
	avr-gcc $(CFLAGS) -o $@ $^

%.o: %.c
	avr-gcc $(CFLAGS) -c -o $@ $<

%.i: %.c
	avr-gcc $(CFLAGS) -E -o $@ $<

%.o: %.S
	avr-gcc $(CFLAGS) -x assembler-with-cpp -c -o $@ $<

clean:
	rm -f *~
	rm -f *.o
	rm -f *.i
	rm -f *.elf
	rm -f *.hex
	rm -f $(OBJS)
