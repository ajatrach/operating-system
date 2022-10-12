#include <stdint.h>
#include <string.h>
#include "pgtbl.h"

struct page_directory_entry pd[1024] __attribute__((aligned(4096)));   //allocating 4 kilobyte block mem for the page directory. least 12 sig bits are 0

struct page pt[1024] __attribute__((aligned(4096)));  //allocate 4 kb mem for page table
struct page another_pt[1024] __attribute__((aligned(4096))); //another pt for allocation func

//maps a list of physical pages to a specified virtual address
// - void *vaddr: The virtual address to map your pages
// - struct physical_page *ppage: a linked list of physical page data structures returned by your page frame allocator
// - struct page_directory_entry *pd: the root of your page table in memory  is pd
// call apge frame alloc choose random va to map it to call map pages 
void *map_pages(void *vaddr, struct page *ppage, struct page_directory_entry *pd) {
   	uint32_t vpn = (long int)vaddr>>12; //aquire virtual page number
	uint32_t directory_number = vpn>>10; //aquire page directory number/entry/location
	uint32_t page_table_number = vpn&0x3FF; //aquire page table number/entry/location

	//initialize the pd at location given
	pd[directory_number].rw=1;
	pd[directory_number].present=1;
	pd[directory_number].frame=(long int)another_pt>>12;
	
	//assign physical address
	another_pt[page_table_number].frame= (int)ppage>>12;
	another_pt[page_table_number].rw=1;
	another_pt[page_table_number].present=1;
	
	return 0;
}



void map_pages_init(){
//Initialization of the page directory and page table (use a for() loop in your main function):
//(a) Zero out every element of the page directory. You could use memset for this.
       
	for(int i=0;i<=1023; i++){     //SHOULD IT BE 1023 or 1024
		pd[i].present=0;
		pd[i].rw=0;
		pd[i].user=0;
		pd[i].writethru=0;
		pd[i].cachedisabled=0;
		pd[i].accessed=0;
		pd[i].pagesize=0;
		pd[i].ignored=0;
		pd[i].os_specific=0;
		pd[i].frame=0;
	}
//(b) Set the present and rw bits to 1 for page directory element 0.
        pd[0].present=1;
        pd[0].rw=1;

//(c) Point the page directory element 0 to the page table your allocated by setting the frame field.
        pd[0].frame=(long int)pt>>12; //element 0 of page directory needs to point to second level of page table

//(d) Set up identity mapping in the page table by setting the frame field in the page table entries equal to the index
//      of the entry. For example, page table[0].frame = 0 page table[1].frame = 1, and so on. Do this for every
//      element of the page table. Also set the present and rw bits to 1 for each element of the page table.
        for(int i =0; i<=1023;i++){
                pt[i].frame =i; //physical address
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
        "mov %eax,%cr0");         //doesn't crash is fine

}
