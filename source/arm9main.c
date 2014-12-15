/*----------------------------------------------------------------------------
 *      by Diddl 2007
 *----------------------------------------------------------------------------*/

#include "nds.h"
#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/statvfs.h>


int main_sub(void);

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------

	// install the default exception handler
	defaultExceptionHandler();

	videoSetMode(0);	//not using the main screen
	consoleDemoInit();
	

	iprintf("NDS FAT TEST V1.0\n");
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
		
		memset(buffer,0xAA,128*1024);

		cpuStartTiming(0);
		
		fwrite(buffer,1,128*1024,file);
		fclose(file);
		u32 elapsed=cpuEndTiming();
		iprintf("128k written in %d cycles.\n",elapsed);
		
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
	while(1) {
		swiWaitForVBlank();
		scanKeys();
		if (keysDown() & KEY_START) break;
	}

	return 0;
}
