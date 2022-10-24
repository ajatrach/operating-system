#include "ide.h"
#include "fat.h"

struct boot_sector *bs;  //pointer
unsigned char buff[512];         //superblock location holds one sector half a kilobyte
unsigned char fatbuff[8192];     //where fat is going 16 sectors
 
char *rdbuff[512*4];    //buffer for the rde values (directory names)
			
int fatInit(){
	ata_lba_read(2048, buff, 1);           //write bootloader to superblock skip to 2048
	bs = buff;                             //point to superblock bs-> etc points to element
        
	if(bs->boot_signature != 0xaa55){    //this means is not a valid bootloader	if false
		return -1;
   	 }
                                         //validate if fat type is FAT16
	if(bs->fs_type[4] != '6'){       //check if last ascii is 6
		return -1;
	}

	//find number rese2-rved sectors
	//read past reserved sectors
	//location of fat is 2048+ #number of reserved sectors
	
	int fatstart = 2048+ bs->num_reserved_sectors;  //find start of FAT
	ata_lba_read(fatstart, fatbuff, 16);            //read FAT to memory (in the buff fatbuff) reads 16 sectors

	//read rde region into buff
        int root_sector = 2048+ bs->num_fat_tables*bs->num_sectors_per_fat + bs->num_reserved_sectors + bs->num_hidden_sectors;
 	ata_lba_read(root_sector, rdbuff, 4);
	
	return 1;
}


int fatOpen(char* path){     //open fat file system on disk   path = ex. USR/LOCAL/BIN/JUNK
	//input is a path look though output is the rde structure of the file you want to open
	struct root_directory_entry *r= rdbuff; //set size of pointer to rde element
	//parse path
	char dn[10]; //directory name
	int rdenum;
	int j=0;
	int q;
	
	for (int i =1; ; i++){   //does not have end condition
		if(path[i]=='/' || path[i] == '\0'){
			break;
		}
		dn[j]=path[i];
		j++;
		q=i;
	}	
	dn[j] = ' ';
	for (rdenum=0;;rdenum++){
		for(int k=0; k<8 ; k++){
			if( r[rdenum].file_name[j] == dn[j]){
				if(r[rdenum].file_name[j]==' '){
					goto done;
				}
				continue;
			}else{
				break;
			}
		}	
	
	}
	done:
	path = &path[q];      //do outer forloop to traverse sub-directories
	
	return rdenum;	
}

//reads data from a file into a buffer
//input to fatread is the rde structure
void fatRead(int rdenum, char* databuff, int sizedatabuff){
	//rde is index of rde table where you can find info about file
	struct root_directory_entry *r = rdbuff;
	unsigned char bbuff[512]; 
       //get from r[denum].cluster to the location in disk
	int clusternum= r[rdenum].cluster;   //clusters per fat?
	int datalocation= 2048 + bs->num_fat_tables*bs->num_sectors_per_fat+bs->num_reserved_sectors+((bs->num_root_dir_entries*sizeof(struct root_directory_entry))/512) 
		+(clusternum-2)*bs->num_sectors_per_cluster;    //is number of sectors   
								
	//ata_lba_read(datalocation, bbuff, 1);   //ata read only reads 512 at time int is # of clusters
	int offset=0;					//
	for(int i=0;i<sizedatabuff; i++){   //if there is still data left
		ata_lba_read(datalocation+i, bbuff, 1);
		for (int j=0; j<512; j++){
			databuff[j+offset]=bbuff[j];
		}
		offset=offset+512;
		
	}

}
	
