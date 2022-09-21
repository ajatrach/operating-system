#include "page.h"
#include <stddef.h>

struct ppage physical_page_array[128]; // 128 pages, each 2mb in length covers 256 megs of memory
struct ppage* free_physical_pages = NULL; //initialize head
struct ppage* allocd_list=NULL;    //head of allocated list
				   
void init_pfa_list(void) {
//will initialize the linked list of free pages. It will loop through every element of your statically allocated
//physical page array and link it into the list.
//
	extern void *_end_kernel;
	int starting_address =4000; // _end_kernel&000 + 4000;
	struct ppage* temp;
	struct ppage* iterator;	
	for (int i=0; i<=128; i++){
		temp=&physical_page_array[i];           
		temp->physical_addr= starting_address+(4000*i);     //aquiring physical address
		temp->next=NULL;
		temp->prev=NULL;

		if(free_physical_pages == NULL){
			free_physical_pages=temp;
		}else{
			iterator = free_physical_pages;         //set iterator at head
        	        while(iterator->next != NULL){  //traverse list
                        iterator= iterator->next;
               		}
                iterator->next= temp;            //add temp at end
		temp->prev=iterator;

		}
	}

}


struct ppage *allocate_physical_pages(unsigned int npages) {
//allocates one or more physical pages from the list of free pages. The input to this function
//npages specifies the number of pages to allocate. The function will unlink the requested number of pages from the free list
//and create a new separate list. It will return a pointer to the new list (called allocd list in the diagram below).

	struct ppage* iterator=NULL;
	struct ppage* tempfree=NULL;	
	for(int i=0; i<npages; i++){
		tempfree = free_physical_pages;  //store head
		free_physical_pages = free_physical_pages->next; //update head

		//link it to alloc list
		if(allocd_list=NULL){                  //if allocd doesn't point to anything yet, point to first removed
			allocd_list=tempfree;
		}
		else{                                    //otherwise iterate to end and jam it on
			iterator = allocd_list;
			while(iterator->next != NULL){
				iterator=iterator->next;
			}
			iterator->next=tempfree;
			tempfree->prev=iterator;
		}		
	}
						//return the pointer to the list
	return allocd_list;
}

//creating sepperate new list 
//	struct ppage* temp = NULL;
//	struct ppage* iterator = NULL;

//	for (int i=0; i<npages; i++){
		//temp = mallocnode             //how to allocate location within array??
		//temp->physical_addr=
//		temp->next=NULL;
//		temp->prev=NULL;
//
//		if(allocd_list = NULL){         //if list is empty, establish first node
//			allocd_list=temp;
//		}else{
//		
//		iterator = allocd_list;         //set iterator at head
//		while(iterator->next != NULL){  //traverse list
//			iterator= iterator->next;
//		}
//		iterator->next= temp;            //add temp at end
	
//		}
//	}
//	return allocd_list;
//}

void free_physical_pages_function(struct ppage *ppage_list){
//returns a list of physical pages to the free list. Basically does the opposite of allocate physical pages.
	//insert this into the free list, we are inserting at begining
	struct ppage * temphead = free_physical_pages; //store head
	free_physical_pages = ppage_list;   //head now points to new element 
	ppage_list->next = temphead->next;  //new element's next points to prev head's next
	temphead->next->prev = ppage_list;  //prev head's prev points to new element
}
