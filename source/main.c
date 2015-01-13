/*----------------------------------------------------------------------------
 *      by Diddl 2007
 *----------------------------------------------------------------------------*/

#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

void initialise();
void waitForInput();
void beginTiming();
uint32_t endTiming();

int main_sub(void);

int timed_read(char *name, void *buffer, int size, bool buffered) {

		beginTiming();

		FILE *file = fopen(name,"rb");
		if (!buffered) setvbuf(file, NULL , _IONBF, 0);

		fread(buffer,1,size,file);
		fclose(file);

//		return cpuEndTiming();

		return endTiming();
}

int timed_write(char *name, void *buffer, int size, bool buffered) {

		beginTiming();

		FILE *file = fopen(name,"wb");
		if (!buffered) setvbuf(file, NULL , _IONBF, 0);

		fwrite(buffer,1,size,file);
		fclose(file);

		return endTiming();

		return 0;
}


//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------

	initialise();

	iprintf("libfat test V1.1\n");
	iprintf("fatInit()..");
	if (fatInitDefault()) {
		iprintf("\tSuccess\n");
		main_sub();


		char *buffer = malloc(256*1024);
		if (buffer == NULL ) {
			iprintf("Out of Memory!\n");
			return 0;
		}
		char *buffer2 = buffer + 128 * 1024;
		FILE *file = fopen("128k.tst","wb");

		if (file == NULL) {
			iprintf("File Create Error!\n");
			return 0;
		}

		memset(buffer,0xAA,256*1024);

		iprintf("write tests\n");

		u32 elapsed=timed_write("128k.tst",buffer,128*1024,true);
		iprintf("buffered   128k %"PRIi32".\n",elapsed);

		elapsed=timed_write("128k.tst",buffer,128*1024,false);
		iprintf("unbuffered 128k %"PRIi32".\n",elapsed);

		elapsed=timed_write("256k.tst",buffer,256*1024,true);
		iprintf("buffered   256k %"PRIi32".\n",elapsed);

		elapsed=timed_write("256k.tst",buffer,256*1024,false);
		iprintf("unbuffered 256k %"PRIi32".\n",elapsed);

		iprintf("read tests\n");

		elapsed=timed_read("128k.tst",buffer,128*1024,true);
		iprintf("buffered   128k %"PRIi32".\n",elapsed);

		elapsed=timed_read("128k.tst",buffer,128*1024,false);
		iprintf("unbuffered 128k %"PRIi32".\n",elapsed);

		elapsed=timed_read("256k.tst",buffer,256*1024,true);
		iprintf("buffered   256k %"PRIi32".\n",elapsed);

		elapsed=timed_read("256k.tst",buffer,256*1024,false);
		iprintf("unbuffered 256k %"PRIi32".\n",elapsed);

		file = fopen("128k.tst","rb+");

		if (file == NULL) {
			iprintf("File Open Error!\n");
			return 0;
		}

		fseek(file,6*512,SEEK_SET);
		fread(buffer,1,1024,file);
		fseek(file,42*512,SEEK_SET);
		fread(buffer,1,1024,file);
		fseek(file,0,SEEK_SET);

		memset(buffer,0x55,128*1024);

		fwrite(buffer,1,128*1024,file);
		fclose(file);

		file = fopen("128k.tst","rb");
		fread(buffer2,1,128*1024,file);
		fclose(file);

		if (memcmp(buffer,buffer2,128*1024)) {
			iprintf("write failed\n");
		}
		iprintf("done!\n");

		file = fopen("attrib_test","wb");
		fclose(file);

		int attr = FAT_getAttr("attrib_test");
		iprintf("initial attributes = %02x\n",attr);

		int new_attr = ATTR_ARCHIVE;

		FAT_setAttr("attrib_test",new_attr);
		attr = FAT_getAttr("attrib_test");
		iprintf("attributes set to %02x\n",attr);

		new_attr |= ATTR_SYSTEM;
		FAT_setAttr("attrib_test",new_attr);
		attr = FAT_getAttr("attrib_test");
		iprintf("attributes set to %02x\n",attr);

		new_attr |= ATTR_HIDDEN;
		FAT_setAttr("attrib_test",new_attr);
		attr = FAT_getAttr("attrib_test");
		iprintf("attributes set to %02x\n",attr);

		new_attr |= ATTR_READONLY;
		FAT_setAttr("attrib_test",new_attr);
		attr = FAT_getAttr("attrib_test");
		iprintf("attributes set to %02x\n",attr);

	} else {
		iprintf("\tFailure\n");
	}

	printf("Test Complete!\n");

	waitForInput();

	return 0;
}
