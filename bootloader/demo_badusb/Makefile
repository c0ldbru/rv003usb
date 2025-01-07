all : demo_badusb.bin busbflash

TARGET:=demo_badusb
CH32V003FUN:=../../ch32v003fun/ch32v003fun
MINICHLINK?=../../ch32v003fun/minichlink

ADDITIONAL_C_FILES+=../../rv003usb/rv003usb.S ../../rv003usb/rv003usb.c
SYSTEM_C:=
EXTRA_CFLAGS:=-I../rv003usb -I../lib

busbflash : busbflash.c
	gcc -o $@ $^ -ludev -I../../ch32v003fun/minichlink

LINKER_SCRIPT:=scratchpad.ld
WRITE_SECTION:=bootloader
FLASH_COMMAND:=./busbflash

include ../../ch32v003fun/ch32v003fun/ch32v003fun.mk

flash : busbflash cv_flash
clean : cv_clean
	rm -rf busbflash

