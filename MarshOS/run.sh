#assemble boot.s file
as --32 source/boot.asm -o build/boot.o

#compile kernel.c file
gcc -m32 -Iinclude -c source/kernel.c -o build/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
gcc -m32 -Iinclude -c source/char.c -o build/char.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
gcc -m32 -Iinclude -c source/utils.c -o build/utils.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
gcc -m32 -Iinclude -c source/calculator.c -o build/calculator.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

#linking the kernel with kernel.o and boot.o files
ld -m elf_i386 -T config/linker.ld build/kernel.o build/char.o build/utils.o build/calculator.o build/boot.o -o kernel.bin -nostdlib

#check kernel.bin file is x86 multiboot file or not
grub-file --is-x86-multiboot kernel.bin

#building the iso file
mkdir -p iso/boot/grub
cp kernel.bin iso/boot/kernel.bin
cp grub.cfg iso/boot/grub/grub.cfg
grub-mkrescue -o MarshOS.iso iso

#run it in qemu
qemu-system-x86_64 -cdrom MarshOS.iso -machine q35,acpi=on