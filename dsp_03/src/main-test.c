#include "avi.h"
#include "avstream.h"
#include "common.h"


AVFormatContext avContext;

int main(int argc, char **argv)
{

	FILE *fp;

	fp = fopen(argv[1], "rb");
	if(fp == NULL) 
	{
		printf("error \r\n");
		return -1;
	}

	avContext.fp = fp;
	av_register_all();

	if(av_open_input_file(&avContext)	 < 0)
	{
		printf("can't detect file format \r\n");
		goto FAIL;	
	}

	printf("find file format \r\n");
  printf("fmt address : %08X \r\n", avContext.iformat);	
  
  if(av_read_header_file(&avContext) < 0)	{		
    printf("incorrect header format \r\n");		
    goto FAIL;	
  }	
  
  if(avContext.formatType == AVI_FORMAT)	{		
    int i;				
    printf("AVI FILE Format \r\n");		
    AVIStream *st = (AVIStream *)avContext.filecontain;		
    AVIStreamHeader *stheader;		
    
    char *s, *s1;		
    printf("stream count : %d, Size: %d \r\n", st->nbStreams, st->aviSize);		
    printf("MaxBytesPerSec : %d, MicroSecPerFrame : %d \r\n", st->mainHeader.dwMaxBytesPerSec, st->mainHeader.dwMicroSecPerFrame);		
    
    for(i=0; i < st->nbStreams; i++)		{			
      stheader = &(st->streamHeader[i]);			
      s = &(stheader->fccType);			
      s1 = &(stheader->fccHandler);			
      printf("fccType : %c%c%c%c, fccHandler : %c%c%c%c \r\n", s[0],s[1],s[2],s[3], s1[0],s1[1],s1[2],s1[3]);			
      printf("rate : %d, Scale: %d,  %f \r\n", stheader->dwRate , stheader->dwScale, (float)(stheader->dwRate/stheader->dwScale));		
    }			
  }	else {		
    printf("Unknown FILE Format \r\n");	
  }

FAIL:
	fclose(fp);

	return 0;
}
