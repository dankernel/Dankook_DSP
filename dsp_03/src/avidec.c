
#include "common.h"
#include "avi.h"
#include "avstream.h"

#define MKTAG(a,b,c,d) (a | (b << 8) | (c << 16) | (d << 24))
#define MKBETAG(a,b,c,d) (d | (c << 8) | (b << 16) | (a << 24))

static const char avi_headers[][8] = {
    { 'R', 'I', 'F', 'F',    'A', 'V', 'I', ' ' },
    { 'R', 'I', 'F', 'F',    'A', 'V', 'I', 'X' },
    { 'R', 'I', 'F', 'F',    'A', 'V', 'I', 0x19},
    { 'O', 'N', '2', ' ',    'O', 'N', '2', 'f' },
    { 'R', 'I', 'F', 'F',    'A', 'M', 'V', ' ' },
    { 0 }
};


static int avi_read_idx1(AVFormatContext *s, int size)
{
	unsigned int index, tag, flags, pos, len;
	int nb_index_entries, i;
	unsigned int pos1;
	
	AVIStream *st = (AVIStream *)s->filecontain;
	AVIStreamHeader *stheader;
	AVIINDEXENTRY *idxentry;
	char *s3;
	
	nb_index_entries = size / 16;

	if (nb_index_entries <= 0)
		return -1;

	printf("index count : %d \r\n", nb_index_entries);
	for (i=0; i < nb_index_entries; i++)
	{
		tag = GET_FCC(s->fp);
		flags = GET_FCC(s->fp);
		pos = GET_FCC(s->fp);
		len = GET_FCC(s->fp);

		s3 = (char *)&tag;
	//	printf("tag : %c%c%c%c, flags: %08X, pos: %08X, len: %08X st->moviList : %d \r\n", s3[0], s3[1], s3[2], s3[3], flags, pos, len, st->moviList);
		pos1 = pos;
		if (i==0 && pos > st->moviList)
			st->moviList = 0;
		pos += st->moviList;

		index = ((tag & 0xff) - '0') * 10;
		index += ((tag >> 8) & 0xff) - '0';

	//	printf("index : %d \r\n", index);


		if (index >= st->nbStreams)
			continue;

		if(feof(s->fp)) return -1;


		stheader = &(st->streamHeader[index]);
		stheader->nb_index_entries++;

		stheader->aviIndexEntry = realloc(stheader->aviIndexEntry, sizeof(AVIINDEXENTRY) * (stheader->nb_index_entries));

		idxentry = &(stheader->aviIndexEntry[stheader->nb_index_entries - 1]);

		idxentry->ckid = tag;
		idxentry->dwChunkLength = len;
		idxentry->dwChunkOffset = pos;
		idxentry->dwFlags = (flags&AVIIF_INDEX) ? AVINDEX_KEYFRAME : 0;
		idxentry->timestamp = stheader->cum_len;

	//	printf("idxentry timestamp : %d \r\n", idxentry->timestamp);
//		printf("tag : %c%c%c%c, flags: %08X, pos1: %d, pos: %d, len: %d , timestamp : %d \r\n", s3[0], s3[1], s3[2], s3[3], flags, pos1, pos, len, idxentry->timestamp);

		if(stheader->dwSampleSize)
			stheader->cum_len += len;
		else
			stheader->cum_len++;
		
	}


	for(i=0; i < st->nbStreams; i++)
		printf("index entry : %d \r\n",st->streamHeader[i].nb_index_entries);
	return 0;
}

static int avi_load_index(struct AVFormatContext *s)
{
	uint32_t tag, size;
	int64_t pos= fileFtell(s->fp);
	AVIINDEXENTRY *entry;
	AVIStream *st = (AVIStream *)s->filecontain;
	int ret= 0;
	char *s2;
	
    if (fileSeek(s->fp, st->moviEnd, SEEK_SET) < 0) {
	 ret = -1;
        goto the_end; // maybe truncated file
    }

	for(;;) {
		
		tag = GET_FCC(s->fp);
		size = GET_FCC(s->fp);

		if(feof(s->fp))
			goto the_end;

		s2 = (char *)&tag;
		printf("[avi_load_index] tag : %c%c%c%c, size : %d , pos : %d\r\n",s2[0], s2[1], s2[2], s2[3], size, fileFtell(s->fp));


		switch(tag) {
			case MKTAG('i','d','x','1') :
				if(avi_read_idx1(s, size) < 0)
				{
					ret = -1;
					goto the_end;
				}
				break;
			default :
				skip:
				size += (size & 1);
				if (fileSkip(s->fp, size) < 0)
				{
					ret = -1;
					goto the_end;
				}

				break;
		}
		
	}

the_end:
    fileSeek(s->fp, pos, SEEK_SET);

   return ret;

}

static int avi_read_header(struct AVFormatContext *s)
{
	AVIStream *st;
	unsigned int tag, size, tag1, handler;
    	uint64_t list_end = 0;
	int stream_idx = 0;
	AVIStreamHeader *stHeader;

	char *s1;
	int k=0;
	
	st = malloc(sizeof(AVIStream));
	memset(st, 0x00, sizeof(AVIStream));

	s->formatType = AVI_FORMAT;

	printf("avi_read_header \r\n");

	GET_FCC(s->fp); // RIFF skip
	if((st->aviSize = GET_FCC(s->fp)) == 0) // avi file size
	{
		printf("invaild AVI File Size \r\n");
		return -1;
	}
	
	GET_FCC(s->fp); // AVI type skip
	while(!feof(s->fp)) 
	{
		tag = GET_FCC(s->fp);
		size = GET_FCC(s->fp);

		if(feof(s->fp))
			break;
		
		s1 = (char *)&tag;
		//k++;
		if(tag == MKTAG('L','I','S','T'))
			printf("############## tag : %c%c%c%c, size : %d \r\n",s1[0], s1[1], s1[2], s1[3], size);
		else
			printf("		tag : %c%c%c%c, size : %d \r\n",s1[0], s1[1], s1[2], s1[3], size);

		//if( k == 10) return -1;
		
		switch(tag) {
			case MKTAG('L','I','S','T'):
				list_end = fileFtell(s->fp) + size;
				tag1 = GET_FCC(s->fp);

				s1 = (char *)&tag1;
				printf("		LIST TYPE : %c%c%c%c \r\n",s1[0], s1[1], s1[2], s1[3]);

				if (tag1 == MKTAG('m','o','v','i')) {
					st->moviList = fileFtell(s->fp) - 4;
					printf("		movi , pos : %d\r\n", st->moviList);
					
					if(size) st->moviEnd = st->moviList + size + (size & 1);
					else st->moviEnd = st->aviSize + 8;
					goto END_OF_HEADER;
				}
				break;

			case MKTAG('d','m','l','h') :
				fileSkip(s->fp, size + (size & 1));
				break;
				
			case MKTAG('a','v','i','h') :
				st->mainHeader.dwMicroSecPerFrame = GET_FCC(s->fp);
				st->mainHeader.dwMaxBytesPerSec = GET_FCC(s->fp);
				st->mainHeader.dwPaddingGranularity = GET_FCC(s->fp);
				st->mainHeader.dwFlags = GET_FCC(s->fp);
				st->mainHeader.dwTotalFrames = GET_FCC(s->fp);
				st->mainHeader.dwInitialFrames = GET_FCC(s->fp);
				st->mainHeader.dwStreams = GET_FCC(s->fp);
				st->mainHeader.dwSuggestedBufferSize = GET_FCC(s->fp);
				st->mainHeader.dwWidth = GET_FCC(s->fp);
				st->mainHeader.dwHeight = GET_FCC(s->fp);
				fileSkip(s->fp, size - (10*4));
				printf("			dwMicroSecPerFrame : 	%d \r\n",st->mainHeader.dwMicroSecPerFrame);
				printf("			dwMaxBytesPerSec : 	%d \r\n",st->mainHeader.dwMaxBytesPerSec);
				printf("			dwFlags : 			0x%08X \r\n",st->mainHeader.dwFlags);
				printf("			dwStreams : 			%d \r\n",st->mainHeader.dwStreams);
				printf("			dwWidth : 			%d \r\n",st->mainHeader.dwWidth);
				printf("			dwHeight : 			%d \r\n",st->mainHeader.dwHeight);
				
				
				break;

			case MKTAG('s','t','r','h') :
				tag1 = GET_FCC(s->fp);
				handler = GET_FCC(s->fp);

				s1 = (char *)&tag1;

				printf("			fccType : %c%c%c%c \r\n", s1[0], s1[1], s1[2], s1[3]);

				s1 = (char *)&handler;
				printf("			handler : %c%c%c%c \r\n", s1[0], s1[1], s1[2], s1[3]);
				
				
				if(tag1 == MKTAG('p','a','d','s')) {
					printf("pads skip \r\n");
					fileSkip(s->fp, size - 8);
				}
				else {
					
					stream_idx = st->nbStreams++;
					stHeader = &(st->streamHeader[stream_idx]);
					stHeader->fccType = tag1;
					stHeader->fccHandler = handler;
					stHeader->dwFlags = GET_FCC(s->fp);
					printf("			Flags : %08X \r\n", stHeader->dwFlags);
					
					stHeader->wPriority = GET_TCC(s->fp);
					stHeader->wLanguage = GET_TCC(s->fp);

					printf("			Priority : %d , Language : %d \r\n", stHeader->wPriority, stHeader->wLanguage);
					stHeader->dwInitialFrames = GET_FCC(s->fp);
					stHeader->dwScale = GET_FCC(s->fp);
					stHeader->dwRate = GET_FCC(s->fp);

					printf("			Scale : %d, Rate : %d \r\n",stHeader->dwScale , stHeader->dwRate);

					if( !(stHeader->dwScale && stHeader->dwRate)) {
						if(st->mainHeader.dwMicroSecPerFrame) {
							stHeader->dwScale = st->mainHeader.dwMicroSecPerFrame;
							stHeader->dwRate = 1000000;
						} else {
							stHeader->dwRate = 25;
							stHeader->dwScale = 1;
						}
					}
					
					stHeader->dwStart = GET_FCC(s->fp);
					stHeader->dwLength = GET_FCC(s->fp); //nb_frames
					stHeader->dwSuggestedBufferSize = GET_FCC(s->fp);
					stHeader->dwQuality = GET_FCC(s->fp);
					stHeader->dwSampleSize = GET_FCC(s->fp);
					stHeader->left  = GET_TCC(s->fp);
					stHeader->top = GET_TCC(s->fp);
					stHeader->right = GET_TCC(s->fp);
					stHeader->bottom = GET_TCC(s->fp);
					stHeader->dwStart *= FFMAX(1, stHeader->dwSampleSize);
					stHeader->cum_len = stHeader->dwStart;

					printf("			dwStart		: %d \r\n", stHeader->dwStart);
					printf("			dwLength 	: %d \r\n", stHeader->dwLength);
					printf("			dwSampleSize: %d \r\n", stHeader->dwSampleSize);
					switch(tag1) {
						case MKTAG('v','i','d','s') :
							stHeader->codectype = CODEC_TYPE_VIDEO;
							stHeader->dwSampleSize = 0;
							break;
						case MKTAG('a','u','d','s') :
							stHeader->codectype = CODEC_TYPE_AUDIO;
							break;
						case MKTAG('t','x','t','s') :
							stHeader->codectype = CODEC_TYPE_SUBTITLE;
							break;
						case MKTAG('d','a','t','s') :
							stHeader->codectype = CODEC_TYPE_DATA;
							break;
						default:
							printf("unknow stream type %X \r\n", tag1);
							goto fail;
					}
					fileSkip(s->fp, size - (14 * 4));

				}
				break;
			case MKTAG('s','t','r','f') :
				if(stream_idx >= st->nbStreams) {
					fileSkip(s->fp, size);
				}
				else {
					uint64_t cur_pos = fileFtell(s->fp);
					char *s12;
					
					if (cur_pos < list_end)
						size = FFMIN(size, list_end - cur_pos);
					stHeader = &(st->streamHeader[stream_idx]);

					printf("			strf size : %d \r\n", size);
					
					switch(stHeader->codectype) 
					{
						case CODEC_TYPE_VIDEO :
							GET_FCC(s->fp); // size
							stHeader->codecwidth = GET_FCC(s->fp); //width
							stHeader->codecheight = GET_FCC(s->fp); //height
							GET_TCC(s->fp); // panes
							stHeader->bits_per_coded_sample = GET_TCC(s->fp);
							tag1 = GET_FCC(s->fp); /* Compression */
							GET_FCC(s->fp);  /* ImageSize */
							GET_FCC(s->fp); /* XPelsPerMeter */
							GET_FCC(s->fp); /* YPelsPerMeter */
							GET_FCC(s->fp); /* ClrUsed */
							GET_FCC(s->fp); /* ClrImportant */
#if 1
							s12 = (char *)&tag1;
							printf("			Width : %d \r\n", stHeader->codecwidth);
							printf("			Height : %d \r\n", stHeader->codecheight);
							printf("			BitCount : %d \r\n", stHeader->bits_per_coded_sample);
							printf("			Compression : %c%c%c%c \r\n", s12[0], s12[1], s12[2], s12[3]);
#endif							
							if(size > 10*4 && size<(1<<30)) {
								int extradata_size= size - 10*4;

								fileSkip(s->fp, extradata_size);

								if(extradata_size & 1)
								{
									fileGetByte(s->fp);
								}
							}
							break;
							
						case CODEC_TYPE_AUDIO :
							fileSkip(s->fp, size);
							break;
						default :
							printf("unkown format \r\n");
							fileSkip(s->fp, size);
							break;
					}
				}
				break;
			default:
				if(size > 1000000){
						printf("Something went wrong during header parsing, "
				                    		"I will ignore it and try to continue anyway.\n");
		
					goto END_OF_HEADER;
				}
				/* skip tag */
				size += (size & 1);
				fileSkip(s->fp, size);							
				break;
		}
	}

END_OF_HEADER :
	if (stream_idx != st->nbStreams -1) {
		fail:
			return -1;
	}

	printf("END_OF_HEADER \r\n");
	
	s->filecontain = (void *)st;

	if (!st->index_loaded)
	{
		avi_load_index(s);	
		st->index_loaded = 1;
	}

}


static int avi_probe(AVProbeData *p)
{
	int i;
	
	/* check file header */
	for(i=0; avi_headers[i][0]; i++)
	    if(!memcmp(p->buf  , avi_headers[i]  , 4) &&
	       !memcmp(p->buf+8, avi_headers[i]+4, 4))
	        return 1;
	
	return 0;
}

AVInputFormat avi_demuxer = {
    "avi",
    "AVI format",
    sizeof(AVIStream),
    avi_probe,
    avi_read_header,
};
