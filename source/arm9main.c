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
	

	printf("NDS FAT TEST V1.0\n");
	printf("fatInit()..");
	if (fatInitDefault()) {
		printf("\tSuccess\n");
		main_sub();

		
		char *buffer = malloc(256*1024);
		if (buffer == NULL ) {
			printf("Out of Memory!\n");
			return 0;
		}
		char *buffer2 = buffer + 128 * 1024;
		FILE *file = fopen("128k.tst","wb");

		
		if (file == NULL) {
			printf("File Create Error!\n");
			return 0;
		}
		
		memset(buffer,0xAA,128*1024);

		cpuStartTiming(0);
		
		fwrite(buffer,1,128*1024,file);
		fclose(file);
		u32 elapsed=cpuEndTiming();
		printf("128k written in %d cycles.\n",elapsed);
		
		file = fopen("128k.tst","rb+");

		if (file == NULL) {
			printf("File Open Error!\n");
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
			printf("write failed\n");
		}
		printf("done!\n");		
	} else {
		printf("\tFailure\n");
	}
	printf("Test Complete!\n");
	while(1) {
		swiWaitForVBlank();
		scanKeys();
		if (keysDown() & KEY_START) break;
	}

	return 0;
}
