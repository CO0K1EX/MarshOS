#Assemble boot.s file
as --32 source/boot.asm -o build/boot.o

#Compile kernel.c file
gcc -m32 -Iinclude -c source/kernel.c -o build/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
gcc -m32 -Iinclude -c source/char.c -o build/char.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
gcc -m32 -Iinclude -c source/utils.c -o build/utils.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
gcc -m32 -Iinclude -c source/vga.c -o build/vga.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
gcc -m32 -Iinclude -c source/console.c -o build/console.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
gcc -m32 -Iinclude -c source/filemanager.c -o build/filemanager.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
gcc -m32 -Iinclude -c source/calculator.c -o build/calculator.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

#Linking the kernel with kernel.o and boot.o files
ld -m elf_i386 -T config/linker.ld build/kernel.o build/char.o build/utils.o build/boot.o build/vga.o build/console.o build/filemanager.o build/calculator.o -o kernel.bin -nostdlib

#Check MarshOS.bin file is x86 multiboot file or not
grub-file --is-x86-multiboot kernel.bin

#Building the iso file
mkdir -p iso/boot/grub
cp kernel.bin iso/boot/kernel.bin
cp grub.cfg iso/boot/grub/grub.cfg
grub-mkrescue -o MarshOS.iso iso

#Run it in qemu
qemu-system-x86_64 -cdrom MarshOS.iso -no-reboot -machine q35,acpi=on
