#include "common.h"




/*#define GET_FCC(fp) (getc(fp) << 0 | getc(fp) << 8 | getc(fp) << 16 | getc(fp) << 24) */
/*#define GET_TCC(fp) (getc(fp) << 0 | getc(fp) << 8)*/

#define PUT_FCC(ch4, fp) putc(ch4[0],fp); putc(ch4[1],fp); putc(ch4[2],fp); putc(ch4[3],fp)
#define PUT_FCCN(num, fp) putc((num>>0)&0377,fp); putc((num>>8)&0377,fp); putc((num>>16)&0377,fp); putc((num>>24)&0377,fp)
#define PUT_TCC(ch2, fp) putc(ch2[0],fp); putc(ch2[1],fp)

static char DEBUG_FCC[4];

unsigned int GET_FCC (FILE *fp) {
	unsigned char tmp[4];

	tmp[0] = getc(fp);
	tmp[1] = getc(fp);
	tmp[2] = getc(fp);
	tmp[3] = getc(fp);

	return FCC (tmp);
}

unsigned int GET_FCC1 (FILE *fp) {
	char tmp[4];

	tmp[0] = getc(fp);
	if(tmp[0] < 0) perror("test");
	tmp[1] = getc(fp);
	tmp[2] = getc(fp);
	tmp[3] = getc(fp);

	return FCC (tmp);
}

unsigned int GET_TCC (FILE *fp) {
	char tmp[5];

	tmp[0] = getc(fp);
	tmp[1] = getc(fp);
	tmp[2] = 0;
	tmp[3] = 0;

	return FCC (tmp);
}

char *fcc_to_char (unsigned int fcc) {
	DEBUG_FCC[0]= (fcc)&0177;
	DEBUG_FCC[1]= (fcc>>8)&0177;
	DEBUG_FCC[2]= (fcc>>16)&0177;
	DEBUG_FCC[3]= (fcc>>24)&0177;

	return DEBUG_FCC;	
}

char *tcc_to_char (unsigned int tcc) {
	DEBUG_FCC[0]= (tcc)&0177;
	DEBUG_FCC[1]= (tcc>>8)&0177;
	DEBUG_FCC[2]= 0;
	DEBUG_FCC[3]= 0;

	return DEBUG_FCC;	
}
