/****************************************************************************
 *                                                                          *
 * File    : test_fat.c                                                     *
 *                                                                          *
 * Purpose : Console mode program.  				                        *
 *                                                                          *
 * History : Date      Reason                                               *
 *           00/00/00  Created                                              *
 *                                                                          *
 ****************************************************************************/

#include <fat.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>


#define FILE_DIR		"/"
#define FILE_WRT1		FILE_DIR"test_wrt1"
#define FILE_WRT2		FILE_DIR"test_wrt2"
#define FILE_WRT3		FILE_DIR"test_wrt3"
#define FILE_RD1		FILE_DIR"test_rd"

#define TEST_STRING		"I love devkitPro ........"

#define BUF_SIZE		256

#define ANZ_BLOCKS		259
#define FILE_SIZE		(ANZ_BLOCKS * BUF_SIZE + sizeof(TEST_STRING) -1)

#define FILE_CREATE		"wb"
#define FILE_WRITE		"wb+"
#define FILE_READ		"rb+"




int SetBuffer(char *buf, int size, int type)
{
	unsigned char c;
	int	i;
	char *tst = "Das ist ein Teststring für fatlib";
	char *p;

	if(size <0 || size > BUF_SIZE)
		size = BUF_SIZE;

	switch(type)
	{
	  case 256:
		for(i = 0; i < size; ++i)
			*(buf++) = i;
		break;
	  case 257:
		for(i = 0; i < size; ++i)
			*(buf++) = BUF_SIZE - i;
		break;
	  case 258:
		for(i = 0, p=tst; i < size; ++i)
		{
			if(*p == 0)
				p = tst;
			*(buf++) = *(p++);
		}
		break;
	  default:
		//ss
		c = (unsigned char)type;
		for(i = 0; i < size; ++i)
			*(buf++) = c;
		break;
	}
	return 0;
}

int SeekBlock(FILE *fh, int blk)
{
	long pos;
	int	 fE;

	fE = 0;

	if(blk <= ANZ_BLOCKS)
	{
		pos = BUF_SIZE * blk;
		if(fseek(fh, pos, SEEK_SET))
		{
			fE = 1;
		}
	}
	else
	{
		fE = 2;
	}

	return fE;
}

int WriteBlock(FILE *fh, int blk)
{
	char buf[BUF_SIZE];
	long size;
	int	 fE;

	fE = 0;

	if(blk < ANZ_BLOCKS)
	{
		size = sizeof(buf);
		SetBuffer(buf, size, blk);
		if(fwrite(buf, size, 1, fh) != 1)
		{
			fE = 2;
		}
	}
	else
	{
		if(fputs(TEST_STRING, fh) == EOF)
			fE = 2;
	}

	return fE;
}

int WriteLinear(char *fnam)
{
	FILE *fh;
	int	i, fE;

	fE = 0;

	if((fh = fopen(fnam, FILE_CREATE)) != NULL)
	{
		for(i = 0; i <= ANZ_BLOCKS; ++i)
		{
			if(WriteBlock(fh, i))
			{
				printf("\n* write error");
				fE = 2;
				break;
			}
		}
		fclose(fh);
	}
	else
	{
		printf("\n* open error");
		fE = 1;
	}
	return fE;
}

int WriteConfuse(char *fnam)
{
	FILE *fh;
	int	i, blk, fE;

	fE = 0;
	//FILE *log = fopen("confuse.log", FILE_CREATE);

	if((fh = fopen(fnam, FILE_CREATE)) != NULL)
	{
		for(i = 0; i <= ANZ_BLOCKS; ++i)
		//for(i = ANZ_BLOCKS; i >= 0; --i)
		{
			if(i < 256)		blk = ( (i&0x5f) | ((i>>2)&0x20) | ((i<<2)&0x80) ) ^ 0x5a; // i ^ 0x5a;
			else				blk = i;
			//fprintf(log,"\n- blk=%x (%d)", blk, i);

			if(SeekBlock(fh, blk))
			{
				printf("\n* seek error");
				fE = 3;
				break;
			}
			if(WriteBlock(fh, blk))
			{
				printf("\n* write error");
				fE = 2;
				break;
			}
		}
		fclose(fh);
	}
	else
	{
		printf("\n* open error");
		fE = 1;
	}
	//fclose(log);
	return fE;
}


int CmpBlock(FILE *fh, int blk)
{
	char buf1[BUF_SIZE];
	char buf2[BUF_SIZE];
	long size;
	int	 fE;

	fE = 0;

	if(blk < ANZ_BLOCKS)
	{
		size = BUF_SIZE;
		SetBuffer(buf1, size, blk);
		if(fread(buf2, sizeof(buf2), 1, fh) != 1)
		{
			fE = 2;
		}
		else if(memcmp(buf1, buf2, BUF_SIZE) != 0)
		{
			fE = 3;
		}
	}
	else
	{
		fgets(buf2, sizeof(TEST_STRING), fh);
		if(strcmp(TEST_STRING, buf2) != 0)
		{
			fE = 3;
		}
	}

	return fE;
}

int ReadLinear(char *fnam)
{
	FILE *fh;
	int	i, fE;

	fE = 0;

	if((fh = fopen(fnam, FILE_READ)) != NULL)
	{
		for(i = 0; i < ANZ_BLOCKS; ++i)
		{
			if(CmpBlock(fh, i))
			{
				printf("\n* read error");
				fE = 2;
				break;
			}
		}
		fclose(fh);
	}
	else
	{
		printf("\n* open error");
		fE = 1;
	}


	return fE;
}

int ReadBackward(char *fnam)
{
	FILE *fh;
	int	i, blk, fE;

	fE = 0;

	if((fh = fopen(fnam, FILE_READ)) != NULL)
	{
		for(i = ANZ_BLOCKS; i >= 0; --i)
		{
			blk = i;
			if(SeekBlock(fh, blk))
			{
				printf("\n* seek error");
				fE = 3;
				break;
			}
			if(CmpBlock(fh, blk))
			{
				printf("\n* read error blk=%d",i);
				fE = 2;
				break;
			}
		}
		fclose(fh);
	}
	else
	{
		printf("\n* open error");
		fE = 1;
	}


	return fE;
}

int FileSize(char *fnam)
{
	FILE *fh;
	long  size;

	if((fh = fopen(fnam, FILE_READ)) != NULL)
	{
		fseek(fh, 0, SEEK_END);
		size = ftell(fh);
		fclose(fh);
	}
	else
		size = -1;

	return size;
}

int ChkFilesize(char *fnam1, char *fnam2)
{
	int	fE;
	long  size1, size2;

	fE = 0;

	size1 = FileSize(fnam1);
	if(size1 != FILE_SIZE)
	{
		printf("\n* filesize error:%ld", size1);
		fE = 1;
	}

	if(fnam2 != NULL)
	{
		size2 = FileSize(fnam2);
		if(size2 != FILE_SIZE)
		{
			printf("\n* filesize error(2):%ld", size2);
			fE = 2;
		}
	}

	if(fE) 		printf("\n* filesize should be %ld", (long)FILE_SIZE);

	return fE;
}

int FileCompare(char *fnam1, char *fnam2)
{
	FILE *fh1, *fh2;
	char buf1[BUF_SIZE];
	char buf2[BUF_SIZE];
	int	fE;
	long  size1, size2;

	fE = 0;

	size1 = FileSize(fnam1);
	size2 = FileSize(fnam2);
	if(size1 != size2)
	{
		printf("\n* filesize different");
		fE = 3;
	}

	if(fE == 0)
	{
		if((fh1 = fopen(fnam1, FILE_READ)) != NULL)
		{
			if((fh2 = fopen(fnam2, FILE_READ)) != NULL)
			{
				size2 = sizeof(buf2);

				while(size1 > 0)
				{
					if(size1 < size2)	size2 = size1;

					if(fread(buf1, size2, 1, fh1) != 1)
					{
						printf("\n* read error (1)");
						fE = 1;
						break;
					}
					if(fread(buf2, size2, 1, fh2) != 1)
					{
						printf("\n* read error (2)");
						fE = 2;
						break;
					}
					if(memcmp(buf1, buf2, size2) != 0)
					{
						printf("\n* files different");
						fE = 4;
						break;
					}

					size1 -= size2;
				}

				fclose(fh2);
			}
			else
			{
				printf("\n* open error (2)");
				fE = 2;
			}
			fclose(fh1);
		}
		else
		{
			printf("\n* open error (1)");
			fE = 1;
		}
	}

	if(!fE)
	{
	}


	return fE;
}

int RenameFile()
{
	FILE *fh;

	struct stat st;

	if((fh = fopen("test.txt", FILE_CREATE)) != NULL)
	{
		fputs(TEST_STRING, fh);
		fclose(fh);

		remove("renamed.txt");
		if ( rename("test.txt", "renamed.txt") != 0 ) return -1;
		if (stat ("renamed.txt", &st) < 0) return -1;
	}

	return 0;
}
/****************************************************************************
 *                                                                          *
 * Function: main                                                           *
 *                                                                          *
 * Purpose : Main entry point.                                              *
 *                                                                          *
 * History : Date      Reason                                               *
 *           00/00/00  Created                                              *
 *                                                                          *
 ****************************************************************************/

int main_sub(void)
{
	int fEW = 0;
	int fER = 0;

	printf("\nwrite test linear ... ");
	if(WriteLinear(FILE_WRT1))	fEW = 1;
	else printf("ok");

	if(fEW == 0)
	{
		printf("\nread test linear ... ");
		if(ReadLinear(FILE_WRT1))					fER = 1;
		else printf("ok");

		printf("\nwrite test confuse ... ");
		if(WriteConfuse(FILE_WRT2))				fEW = 1;
		else printf("ok");
/*
		printf("\nwrite test 2 linear ... ");
		if(WriteLinear(FILE_WRT3))	fEW = 1;
		else printf("ok");
*/
		printf("\nread test backward ... ");
		if(ReadBackward(FILE_WRT2))				fER = 1;
		else printf("ok");

		printf("\ncompare files ... ");
		if(FileCompare(FILE_WRT1, FILE_WRT2))		fER = 1;
		else printf("ok");

		printf("\ncheck filesizes ... ");
		if(ChkFilesize(FILE_WRT1, FILE_WRT2))				fER = 1;
		else printf("ok");
	}
	else
	{
		fEW = 1;

		printf("\nread test linear ... ");
		if(ReadLinear(FILE_RD1))				fER = 1;
		else printf("ok");

		printf("\nread test backward ... ");
		if(ReadBackward(FILE_RD1))			fER = 1;
		else printf("ok");

		printf("\ncheck filesize ... ");
		if(ChkFilesize(FILE_RD1, NULL))				fER = 1;
		else printf("ok");
	}

	printf("\nRename test ... ");
	if(RenameFile()) fEW=1;
	else printf("ok");

	if(fEW == 0 && fER == 0)
		printf("\nall tests ok!\n");
	else if(fER == 0)
		printf("\nbad write support!\n");
	else
		printf("\nbad read support\n");


	return 0;
}

