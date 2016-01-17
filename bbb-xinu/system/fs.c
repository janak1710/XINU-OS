#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#if FS
#include <fs.h>

static struct fsystem fsd;

int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

int file_counter = 0;
int inode_counter = 0;

#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2
#define NUM_FD 16

struct filetable oft[NUM_FD];
int next_open_fd = 0;


#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (( (fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2


int fs_fileblock_to_diskblock(int dev, int fd, int fileblock);

int fs_open(char *filename, int flags)
{

 int flag = 0, fd =0,i;

// printf("\nNumber of files in oft %d",next_open_fd);
//Check if file is already opened for read/write
 for (i=0; i< next_open_fd; i++)
 {
 	if (strcmp(oft[i].fname,filename)== 0)
	{
		flag = 1;
		if((flags == 1 || flags == 2) && (oft[i].state == 1 || oft[i].state == 2)) 
		{
			printf("Error Opening File. File already opened for writing\n");
			return SYSERR;
		}
		else
		{
			//return file pointer
			fd = oft[i].fileptr;
		}
	}
 }

//Open the file if its not already open
 if (flag == 0)
 {
 	for (i=0; i<fsd.root_dir.numentries; i++)
        {
                if (strcmp(fsd.root_dir.entry[i].name,filename) == 0)
                {
			oft[next_open_fd].fname = filename;
			oft[next_open_fd].state = flags;
			oft[next_open_fd].de = i;
			oft[next_open_fd].in = fsd.root_dir.entry[i].inode_num;
			oft[next_open_fd].fileptr = 0;
			fd = oft[next_open_fd].fileptr;	                                 
                }
        }
//adjust file open table counter
	next_open_fd = next_open_fd + 1;
  
 }	

// printf("\nNumber of files in oft at end %d",next_open_fd);
 return fd;

}

int fs_close(char* fname,int fd)
{
	int pivot = -1,i,j;

//remove the file from open file table 
//locate file in open file table
	for (i=0;i<next_open_fd;i++)
	{
		if (strcmp(oft[i].fname,fname) == 0)
		{
			pivot = i;
			break;
		}
	}
//if file is not opened then raise an error
	if(pivot == -1)
	{
		printf("Failed, File is not open !\n");
		return SYSERR;
	}
	else
	{
//delete the file from open file table and adjust counter
		for(j=i;j<next_open_fd;j++)
		{
			oft[j].fname = oft[j+1].fname;
	                oft[j].state = oft[j+1].state;
        	        oft[j].de =  oft[j+1].de;
                	oft[j].in = oft[j+1].de;
		}
		next_open_fd--;
	}

	return OK;
}

//Create file 
int fs_create(char *filename, int mode)
{
  int flag = 0,i;
  struct inode in;

//create root directory
  if (mode == O_CREAT)
  {
//check if file name is already present in system
	for (i=0; i<fsd.root_dir.numentries; i++)
	{
		if (strcmp(fsd.root_dir.entry[i].name,filename) == 0)
		{
			printf("File creation failed, file name already exists!\n");
			flag = 1;
			return SYSERR;
		}
	}
  }

  if (flag == 0)
  {
//create inode for directory entry for input file
  	fsd.root_dir.numentries = fsd.root_dir.numentries + 1;
//create directory entry
	fsd.root_dir.entry[file_counter].inode_num = inode_counter;
	fsd.root_dir.entry[file_counter].name = filename;	 
	file_counter++;
	inode_counter++;
//create inode
	in.id = 0;
	in.type = 1;
	in.device = 0;
	in.size = 0;
//save inode in block storage
	fs_put_inode_by_num(0, 0, &in);	
  }
  return 0;
}

//change file pointer value
int fs_seek(int fd, int offset)
{ 
//adjust file pointer
	fd = fd + offset;
	return fd;
}

int fs_read(char* fname,int fd, char *buf, int nbytes)
{	
        int inode,k;
        struct inode in;
	char data[MDEV_BLOCK_SIZE];
	char buf1[1200];
	int fptr,j,i,counter;

//get inode details for given file
        for (i=0;i<next_open_fd;i++)
        {
          if (strcmp(oft[i].fname,fname) == 0)
          {
                  inode = oft[i].in;
		  break;
	  }
	}	

	fs_get_inode_by_num(0, inode, &in);
	
	fptr = fd;
        j = 0;
	counter = nbytes;

	while(nbytes > 0)
	{
		for(i=0;i<in.size;i++)
		{
//skip the blocks untill file pointer position block is reached
			if (fptr > MDEV_BLOCK_SIZE)
			{
				fptr = fptr - MDEV_BLOCK_SIZE;
				continue;
			} 
			else
			{
//read the block
       				//fs_bread(0, in.blocks[i], 0, data, MDEV_BLOCK_SIZE);
				for(k=0;k<MDEV_BLOCK_SIZE;k++)
				{
//skip the bytes before file pointer block
					if (fptr > 0)
					{
						fptr--;
						continue;
					}
					else
					{
//write data to output buffer
						buf[j] = data[k];
						j++;
						nbytes--;
					}
//stop writing ones required bytes are read
					if (nbytes == 0)
						break;
				}				

            }
			if (nbytes == 0)
                    break;
		}
	}  	 
	
	fd = fd + counter;
	return fd;
}        

int fs_write(char* fname,int fd, char buf[1200], int nbytes)
{
	int inode;
	struct inode in;
	char data[MDEV_BLOCK_SIZE];
	int block_number = 0;
	int counter = 0,i;
	int size = 0,flag = 0;

//get inode details for given file
	for (i=0;i<next_open_fd;i++)
        {
                if (strcmp(oft[i].fname,fname) == 0)
                {                    
                        inode = oft[i].in;
			break;
                }
        }

//read inode data
	fs_get_inode_by_num(0, inode, &in);

	int j = 0,k;

	while(nbytes > 0)
	{
		size = 0;
//copy buffer data to block size buffer
		for(i=0;i<MDEV_BLOCK_SIZE;i++)
		{
			data[i] = buf[j];
			j++;
			nbytes--;
			size++;
//come out if last buffer size is less than 512
			if(nbytes == 0)
			{
				flag = 1;
				break;
			}
		}
	
//calculate the block number
		block_number = INODEBLOCKS + counter;
 	 
// write the free block bitmask in block_number, mark block used 
		fs_setmaskbit(block_number);
		//fs_bwrite(0, block_number, 0, data, size);
//adjust inode table
		in.size++;
		in.blocks[counter] = block_number;
//adjust file pointer
		fd = fd + size;
		counter++;
	}

  fs_put_inode_by_num(0, inode, &in); 	

  return fd;
}

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock) {
	int diskblock;
struct inode in;

  if (fileblock >= INODEBLOCKS - 2)
  {
    printf("No indirect block support\n");
    return SYSERR;
  }

  diskblock = oft[fd].in; //get the logical block address

  fs_get_inode_by_num(0, diskblock, &in);

  diskblock = in.blocks[fileblock];

  return diskblock;
}

/* read in an inode and fill in the pointer */
int fs_get_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;
  int inode_off;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  inode_off = inn * sizeof(struct inode);

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  printf("inn*sizeof(struct inode): %d\n", inode_off);
  */

  bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
  memcpy(in, &block_cache[inode_off], sizeof(struct inode));

  return OK;

}

int fs_put_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  */

  bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
  memcpy(&block_cache[(inn*sizeof(struct inode))], in, sizeof(struct inode));
  bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);

  return OK;
}
     
int fs_mkfs(int dev, int num_inodes) {
  int i;
  
  if (dev == 0) {
    fsd.nblocks = dev0_numblocks;
    fsd.blocksz = dev0_blocksize;
  }
  else {
    printf("Unsupported device\n");
    return SYSERR;
  }

  if (num_inodes < 1) {
    fsd.ninodes = DEFAULT_NUM_INODES;
  }
  else {
    fsd.ninodes = num_inodes;
  }

  i = fsd.nblocks;
  while ( (i % 8) != 0) {i++;}
  fsd.freemaskbytes = i / 8; 
  
  if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR) {
    printf("fs_mkfs memget failed.\n");
    return SYSERR;
  }
  
  /* zero the free mask */
  for(i=0;i<fsd.freemaskbytes;i++) {
    fsd.freemask[i] = '\0';
  }
  
  fsd.inodes_used = 0;
  
  /* write the fsystem block to SB_BLK, mark block used */
  fs_setmaskbit(SB_BLK);
  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));
  
  /* write the free block bitmask in BM_BLK, mark block used */
  fs_setmaskbit(BM_BLK);
  bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);

  return 1;
}

void
fs_print_fsd(void) {

  printf("fsd.ninodes: %d\n", fsd.ninodes);
  printf("sizeof(struct inode): %d\n", sizeof(struct inode));
  printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
  printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  fsd.freemask[mbyte] |= (0x80 >> mbit);
  return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  return( ( (fsd.freemask[mbyte] << mbit) & 0x80 ) >> 7);
  return OK;

}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b) {
  int mbyte, mbit, invb;
  mbyte = b / 8;
  mbit = b % 8;

  invb = ~(0x80 >> mbit);
  invb &= 0xFF;

  fsd.freemask[mbyte] &= invb;
  return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
   block is indicated in the high-order bit.  Shift the byte by j
   positions to make the match in bit7 (the 8th bit) and then shift
   that value 7 times to the low-order bit to print.  Yes, it could be
   the other way...  */
void fs_printfreemask(void) {
  int i,j;

  for (i=0; i < fsd.freemaskbytes; i++) {
    for (j=0; j < 8; j++) {
      printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
    }
    if ( (i % 8) == 7) {
      printf("\n");
    }
  }
  printf("\n");
}

#endif /* FS */
