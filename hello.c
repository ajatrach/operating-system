#include <stdint.h>
#include <string.h>
#include "page.h"

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

void main() {

//Initialization of the page directory and page table (use a for() loop in your main function):
//(a) Zero out every element of the page directory. You could use memset for this.
	memset(pd,0,sizeof(pd));
//(b) Set the present and rw bits to 1 for page directory element 0.
	pd[0].present=1;
	pd[0].rw=1;

//(c) Point the page directory element 0 to the page table your allocated by setting the frame field.
	pd[0].frame=pt>>12; //element 0 of page directory needs to point to second level of page table

//(d) Set up identity mapping in the page table by setting the frame field in the page table entries equal to the index
//	of the entry. For example, page table[0].frame = 0 page table[1].frame = 1, and so on. Do this for every
//	element of the page table. Also set the present and rw bits to 1 for each element of the page table.
	for(int i =0; i<=1024;i++){
		pt[i] =i; //physical address
		pt[i].present =1;
		pt[i].rw=1;
	}


//4. Load the CR3 register with the address of the page directory. This can be done in C in the following way:
// Put the address of page_directory[] into CR3
	asm("mov %0,%%cr3"
	:
	: "r"(pd)
	:);
//5. Enable paging by setting bits 0 and 31 in the CR0 register:
	asm("mov %cr0, %eax\n"
	"or $0x80000001,%eax\n"
	"mov %eax,%cr0");


	//testing if allocating pages works
	init_pfa_list();
	struct ppage *thing = allocate_physical_pages(2);
	free_physical_pages_function(thing);	
    
	
	//while loop to print to screen
	while(1) {
        uint8_t status = inb(0x64);

        if(status&1 == 1) {
            uint8_t scancode = inb(0x60);
	  //  esp_printf(putc,("scancode = %d\n"),scancode);  for printing scancode   IF LARGER THAN 127 then don't prin
	    if(scancode<127){
	    esp_printf(putc, "%c", keyboard_map[scancode]);
	    }    
        }
    }
}
