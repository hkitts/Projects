/* Hunter Kitts 
 * CS494/CS594 -- Lab 2: Implementing a File Allocation Table
 */
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <string>
#include <cmath>

//holds 1024 bytes sector in each if not NULL
//flag is set if it needs to be changed
class Sector
{
	public:
		unsigned short * buf = NULL;
		bool flag;
};


//takes the value at a link and finds the block it links to
unsigned short Read_Link(int X, size_t D, size_t S)
{
	unsigned short r;
	r = S + X -1;

	return r;
}

//writes the links for the FAT sectors if any changes where made
void Write_Link(Sector * sectors, unsigned short index, unsigned short bufi, unsigned short val, size_t S)
{
	val = val - S + 1;
	unsigned short current;
	current = sectors[index].buf[bufi];

	if(current != val){
		sectors[index].flag = 1;
		sectors[index].buf[bufi] = val;
	}
}

//reads the jdisk file and stores it in the buffer
int jdisk_read(FILE *jd, unsigned int lba, unsigned short *buf)
{
	fseek(jd,  (1024*lba), SEEK_SET);
	fread(buf, 2, 512, jd);

	return 1;
}

//same as jdisk_write
//not sure why I wrote this??
int jdisk_write_s(FILE *jd, unsigned int lba, unsigned short * buf)
{
	fseek(jd,  (1024*lba), SEEK_SET);
	fwrite(buf, sizeof(char), 1024, jd);
	if(ferror(jd)){
		printf("error writing jd\n");
		return 0;
	}
	return 1;

}
//writes the buffer to the jdisk at the lba
int jdisk_write(FILE *jd, unsigned int lba, char buf[])
{
	size_t i;
	
	fseek(jd,  (1024*lba), SEEK_SET);
	fwrite(buf, sizeof(char), 1024, jd);
	if(ferror(jd)){
		printf("error writing jd\n");
		return 0;
	}

	return 1;
}


//given the jdisk determins the D and S size
//this does not work correctly I have written it several times
//D and S are wrong usually off by 1
void Fat_Values(FILE *jd, size_t *D, size_t *S)
{
	size_t flen, total_sectors;
	float temp = 0.0;

	fseek(jd, 0, SEEK_END);
	flen = ftell(jd);
	total_sectors = flen / 1024;
	temp = (((float)total_sectors)*2)/1024;
	*S = temp + 0.5f;
	if(*S == 0) *S = 1;
	*D = total_sectors - *S;
	//printf("flen: %d\n", flen);
	//printf("Size(sectors): %d  FAT-Sectors: %d   Data-Sectors: %d\n", total_sectors, *S, *D);

}

//imports a file and writes it to the jdisk
void Fat_Import(FILE * jd, FILE * in_fp, size_t D, size_t S, char **argv)
{
	size_t flen, space, sectors_needed, i;
	size_t num_free = 0;
	unsigned short * buf;
	unsigned short link, index, bufi, find_link;
	int reads = 0;
	int writes = 0;
	Sector* sectors;

	sectors = (Sector *) malloc(sizeof(Sector)*S);
	if(sectors == NULL){ fprintf(stderr, "Memory error\n"); exit(1); }

	//read file to be imorted
	fseek(in_fp, 0, SEEK_END);
	flen = ftell(in_fp);
	sectors_needed = ceil(flen/1024.0);
	unsigned short free_links[sectors_needed];
	unsigned short changes[sectors_needed];
	fseek(in_fp, 0, SEEK_SET);	

	//set up to read jdisk
	fseek(jd,  0, SEEK_SET);
	buf = (unsigned short*)  malloc(sizeof(unsigned short)*512);

	//read sector 0 to get start of free list
	if(sectors[0].buf == NULL){
		reads += jdisk_read(jd, 0, buf);
		sectors[0].buf = (unsigned short*)  malloc(sizeof(unsigned short)*512);
		memcpy(sectors[0].buf, buf, sizeof(unsigned short)*512);
	}
	link = Read_Link((int)sectors[0].buf[0], D, S);

	//save a list of free sectors on the jdisk as they are read
	if(link != 0){
		free_links[num_free] = link;
		num_free += 1;
	}

	//if the file would not fit on the jdisk
	if(flen > (D*1024)){
		fprintf(stderr, "%s is too big for the disk (%d vs %d)\n", argv[3], flen, (D*1024));
		free (buf);
		free (sectors[0].buf);
		free (sectors);
		exit(1);
	}

	//find large enough space
	while(num_free < sectors_needed && link != 0){
		find_link = link -S +1;
		index = find_link/512;
		
		//look if stector has already been read
		if(sectors[index].buf == NULL){
			reads += jdisk_read(jd, index, buf);
			sectors[index].buf = (unsigned short*)  malloc(sizeof(unsigned short)*512);
			memcpy(sectors[index].buf, buf, sizeof(unsigned short)*512);
		} 
		//it has already been read
		bufi = (find_link - (index * 512));
		link = Read_Link((int)sectors[index].buf[bufi], D, S);
		if(link != 0){
			free_links[num_free] = link;
			num_free += 1;
		}

	}

	//if there is enough space 
	//the last link will be to the last sector that the file will 
	if(num_free == sectors_needed){

		//get next free sector so can write a link to it
		find_link = link -S +1;
		index = find_link/512;
		if(sectors[index].buf == NULL){
			reads += jdisk_read(jd, index, buf);
			sectors[index].buf = (unsigned short*)  malloc(sizeof(unsigned short)*512);
			memcpy(sectors[index].buf, buf, sizeof(unsigned short)*512);
		}
		bufi = (find_link - (index * 512));
		link = Read_Link((int)sectors[index].buf[bufi], D, S);

		int total_bytes = flen;
		//read the file by 1024 bytes
		for(i = 0; i < sectors_needed; i++){
			changes[i] = free_links[i] - S + 1;
			index = changes[i]/512;
			bufi = changes[i] - (index * 512);

			//write the links in the S sectors if needed
			if(i != sectors_needed - 1){
				Write_Link(sectors, index, bufi, free_links[i+1], S);
			} else{
				Write_Link(sectors, index, bufi, free_links[i], S);
			}

			//read the file
			char buffer[1024];
			fread(buffer, 1, 1024, in_fp);

			//keep a running total of bytes left to right
			//if the last sector to write is 1023 bytes
			if(total_bytes == 1023){
				//set last byte
				buffer[1023] = 0xff;
			}
			//if the last sectors is less than 1023 bytes
			else if(total_bytes < 1023){
				//set last 2 byte
				int x, y;
				x = total_bytes/256;
				y = total_bytes - (x*256);
				buffer[1022] = y;
				buffer[1023] = x;
			}

			else{ total_bytes -= 1024; }
			//if therer are no bytes left write the link as 0
			if(total_bytes == 0){
				Write_Link(sectors, index, bufi, (S-1), S);
			}

			//if this is not the last sector write to the output file
			writes += jdisk_write(jd, (int)free_links[i], buffer);
		}
	} else {
		printf("Not enough free sectors (%d)  for %s, which needs %d.\n", num_free, argv[3], sectors_needed);
		return;
	}

	//find sectors links that need to be changed
	//first link will always be changed
	Write_Link(sectors, 0, 0, link, S);

	for(size_t n =0; n < S; n++){
		if(sectors[n].flag == 1){
			writes += jdisk_write_s(jd, n, sectors[n].buf);
		}
	}


	printf("New file starts at sector %d\n", free_links[0]);
	printf("Reads: %d\n", reads);
	printf("Writes: %d\n", writes);


	free (buf);
	for(size_t n =0; n < S; n++){
		if(sectors[n].buf != NULL) free(sectors[n].buf);
	}
	free(sectors);


}

//finds the file in the jdisk, starting a the starting_block and output it to a file
void Fat_Export(FILE * jd, FILE * out_fp, size_t D, size_t S,int starting_block, char **argv)
{
	Sector* sectors;
	unsigned short lba, temp, index, bufi;
	unsigned short * buf;
	unsigned short num_sec = 0;
	unsigned long  block[D];
	unsigned long link;
	unsigned short link_val[D];
	bool last_block = 0;
	bool entire_block = 0;
	short prev = -1;
	short cur;
	long file_size = 0;
	size_t reads = 0;
	size_t writes = 0;
	size_t i;

	//printf("starting_block %d\n", starting_block);
	if(starting_block < 0){
		fprintf(stderr, "Starting block cannot be less than 0\n");
		exit(1);
	}
	if(starting_block > (D+S)){
		fprintf(stderr, "Starting block cannot be greater than D\n");
		exit(1);
	}	

	//find the link for the starting block

	sectors = (Sector *) malloc(sizeof(Sector)*S);
	if(sectors == NULL){ fprintf(stderr, "Memory error\n"); exit(1); }
	buf = (unsigned short*)  malloc(sizeof(unsigned short)*512);


	//get starting blocks link field
	//printf("link: %d\n", link);
	block[0] = starting_block;
	link = starting_block;
	link = link - S + 1;
	index = link/512;
	bufi = link - (512 * index);
	//printf("link:   %d\n", link);
	//printf("sector: %d\n", index);

	//printf("block: %d\n", block[0]);
	if(sectors[index].buf == NULL){
		reads += jdisk_read(jd, index, buf);
		sectors[index].buf = (unsigned short*)  malloc(sizeof(unsigned short)*512);
		memcpy(sectors[index].buf, buf, sizeof(unsigned short)*512);
	}
	link_val[0] = sectors[index].buf[bufi];

	//if it is the last block of the file set the flags to skip the while loop
	if((block[0] - S + 1) == link_val[0]){
		last_block = 1;
	}
	if(link_val[0] == 0){ 
		entire_block = 1;
	}
	num_sec += 1;

	//if the flags have not been set 
	if(entire_block != 1){
		if(last_block != 1){

			//loop throug and find the sectors that contain the file
			while(1){
				link = Read_Link((int)sectors[index].buf[bufi], D, S);
				block[num_sec] = link;

				link = link - S + 1;
				index = link/512;
				bufi = link - (512 * index);

				if(sectors[index].buf == NULL){
					reads += jdisk_read(jd, index, buf);
					sectors[index].buf = (unsigned short*)  malloc(sizeof(unsigned short)*512);
					memcpy(sectors[index].buf, buf, sizeof(unsigned short)*512);
				}
				link_val[num_sec] = sectors[index].buf[bufi];

				//if the last block is less than 1024 bytes
				if((block[num_sec] - S + 1) == link_val[num_sec]){
					last_block = 1;
					num_sec += 1;
					break;
				}
				//if the last block is 1024 bytes
				if(link_val[num_sec] == 0){
					entire_block = 1;
					num_sec += 1;
					break;
				}
				num_sec += 1;
			}
		}
	}

	/*
	printf("\nblocks that contain file\n");
	for(i = 0; i < num_sec; i++){
		printf("%d\n", block[i]);
	}
	*/

	//if the last block was < 1024 bytes determine the size
	if(last_block){
		fseek(out_fp, 0, SEEK_SET);

		//read each block and from the jdisk
		//write it to the file until the last block
		for(i = 0; i < num_sec - 1; i++){
			reads += jdisk_read(jd, block[i], buf);
			fwrite(buf, 2, 512, out_fp);
		}

		//read last block
		reads += jdisk_read(jd, block[i], buf);

		//figure out last 2 bytes of last file block
		int last, x, y;
		last = buf[511];
		x = last/256;
		y = last - (x*256);
		
		//1023 bytes belong to the file
		if(x == 255){
			fwrite(buf, 1, 1023, out_fp);
		} else{
			fwrite(buf, 1, last, out_fp);
		}

	//if the last block is 1024 bytes just write all the blocks
	} else if(entire_block){
		fseek(out_fp, 0, SEEK_SET);
		for(i = 0; i < num_sec; i++){
			reads += jdisk_read(jd, block[i], buf);
			fwrite(buf, 2, 512, out_fp);
		}

	}

	free(buf);
	for(i = 0; i < S; i++){
		if(sectors[i].buf != NULL) free(sectors[i].buf);
	}
	free(sectors);

	printf("Reads: %d\n", reads);
	printf("Writes: %d\n", writes);


}

void usage()
{
	fprintf(stderr, "FATRW diskfile import input-file\n");
	fprintf(stderr, "FATRW diskfile export starting-block output-file\n");
	return;
}

int main(int argc, char **argv)
{
	std::string disk, action;
	FILE * in_fp;
	FILE * out_fp;
	FILE * jdisk;
	size_t D, S;
	size_t * f;
	int sb;

	//open jdisk
	disk = (std::string)argv[1];
	if(disk.substr(disk.find_last_of(".") + 1) == "jdisk"){
		jdisk = fopen(argv[1], "r+");
		if(jdisk != NULL){
			//printf("jdisk open\n");

			Fat_Values(jdisk, &D, &S);
			f = (size_t*) malloc(S*sizeof(size_t));

			//if import option
			if((std::string)argv[2] == "import"){
				if(argc != 4){ fclose(jdisk); usage(); return 1;}

				//open input-file
				in_fp = fopen(argv[3], "r");
				if(in_fp != NULL){
					//f
					//printf("%d\n", S);
					Fat_Import(jdisk, in_fp, D, S, argv);
					fclose(in_fp);
				} else{
					fclose(jdisk);
					fprintf(stderr, "Unable to open %s\n", argv[3]);
					return 1;
				}
			}

			//if export option
			else if((std::string)argv[2] == "export"){
				if(argc != 5){ fclose(jdisk); usage(); return 1; }



				//open output-file
				out_fp = fopen(argv[4], "w");
				if(out_fp != NULL){
					sb = atoi(argv[3]);
					Fat_Export(jdisk, out_fp, D, S, sb, argv);
					fclose(out_fp);
				} else{
					fclose(jdisk);
					fprintf(stderr, "Unable to open %s\n", argv[4]);
					return 1;
				}
			} else{
				fclose(jdisk);
				fprintf(stderr, "argument: 3 incorrect\n");
				usage();
				return 1;
			}

			//close the jdisk
			fclose(jdisk);
		} else {
			fprintf(stderr, "Unable to open %s\n", argv[1]);
			return 1;
		}

		//not a jdisk file
	} else {
		fprintf(stderr, "%s not a jdisk\n", argv[1]);
		usage();
		return 1;
	}	

	return 0;
}
