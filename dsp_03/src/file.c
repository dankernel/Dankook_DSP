#include "common.h"


int fileRead(FILE *fp, char *buf, int sz)
{
	return fread(buf, 1, sz, fp); 
}

int fileSeek(FILE *fp, int offset, int origin)
{
	return fseek(fp, offset, origin);
}

int fileFtell(FILE *fp)
{
	return ftell(fp);
}

int fileSkip(FILE *fp, int sz)
{
	return fseek(fp, sz, SEEK_CUR);
}

char fileGetByte(FILE *fp)
{
	return getc(fp);
}
