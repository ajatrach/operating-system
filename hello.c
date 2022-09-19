
#include <stdint.h>

#define MULTIBOOT2_HEADER_MAGIC         0xe85250d6

const unsigned int multiboot_header[]  = {MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16+MULTIBOOT2_HEADER_MAGIC), 0, 12};   //hard-coded array  grub is the bootloader,
														   //multibootheader tells grub where to put the OS. 
														   //Grub likes it at address 0

int offset=0;
void putc(char type){
	unsigned short *vram = (unsigned short*)0xb8000;   //base address of video mem
	const unsigned char color = 7; //color is 7
	unsigned short data = (color << 8) + type;
	vram[offset]= data;
	offset= offset+1;
	if(offset==2000){
		for(int i = 0; i<2000; i++){
			vram[i]=vram[i+80];
		}	
		offset=1920;
	}
}

uint8_t inb (uint16_t _port) {
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void main() {

	for (int i = 0; i<100; i++){
	esp_printf(putc, "Hello World! hello hi abcdefghijklmnopqrstuvwxyzlalalalalalalalalalalalalalaalalalalallalala");
	}

    while(1) {
        uint8_t status = inb(0x64);

        if(status & 1) {
            uint8_t scancode = inb(0x60);
        }
    }
}
