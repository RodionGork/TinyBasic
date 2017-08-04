arm-linux-gnueabi-gcc -Wall -Wno-write-strings -Wno-char-subscripts -fno-stack-protector -DNO_STDLIB=1 -O3 -mcpu=cortex-m3 -mthumb -c *.c ../core/*.c
arm-linux-gnueabi-ld -T stm32f103-64k.ld -nostdlib -Map=test.map -o test.elf *.o
arm-linux-gnueabi-objcopy test.elf -O ihex test.hex
arm-linux-gnueabi-size test.elf
rm *.o
rm *.map
rm *.elf
