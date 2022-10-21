#include <stdint.h>
#include <string.h>
#include "page.h"
#include "pgtbl.h"
#include "fat.h"
#define MULTIBOOT2_HEADER_MAGIC         0xe85250d6

const unsigned int multiboot_header[]__attribute__((section(".multiboot"))) = {MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16+MULTIBOOT2_HEADER_MAGIC), 0, 12};
   //hard-coded array  grub is the bootloader,
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

/*
* inb
*
* Reads from I/O port _port, and returns a one byte value
*/
uint8_t inb (uint16_t _port) {
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}
/*
* outb
*
* Writes val to I/O port _port
*/
void outb (uint16_t _port, uint8_t val) {
__asm__ __volatile__ ("outb %0, %1" : : "a" (val), "dN" (_port) );
}


unsigned char keyboard_map[128]=  //needs fixing
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',     /* 9 */
  '9', '0', '-', '=', '\b',     /* Backspace */
  '\t',                 /* Tab */
  'q', 'w', 'e', 'r',   /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
    0,                  /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',     /* 39 */
 '\'', '`',   0,                /* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',                    /* 49 */
  'm', ',', '.', '/',   0,                              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0, 
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0, 
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0, 
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

char buf[4096];
void main() {

	map_pages_init();

	//testing if allocating pages works
	init_pfa_list();
	struct ppage *thing = allocate_physical_pages(2);

	//testing if mapping pages works
	map_pages(0x4000000, thing, 1);
			
	free_physical_pages_function(thing);	
   	//hexdump(sectorbuf, 1); 
	fatInit();
	fatRead(fatOpen("/IDE"),buf, 4096);
	
	//while loop to print to screen
	while(1) {
        uint8_t status = inb(0x64);

       	 if((status&1) == 1) {
            uint8_t scancode = inb(0x60);
	 //  esp_printf(putc,("scancode = %d\n"),scancode);  for printing scancode   IF LARGER THAN 127 then don't prin
	    if(scancode<127){
		    esp_printf(putc, "%c", keyboard_map[scancode]);
	    }    
        }
    }
}
