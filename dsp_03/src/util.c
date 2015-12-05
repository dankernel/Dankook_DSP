
#include "common.h"
#include "avstream.h"

#define REGISTER_DEMUXER(X,x) { \
    extern AVInputFormat x##_demuxer; \
    av_register_input_format(&x##_demuxer); }


#define DETECT_SIZE (32*1024)

AVInputFormat *first_iformat = NULL;

void av_register_input_format(AVInputFormat *format)
{
	AVInputFormat *p;

	printf("av_register_input_format \r\n");

	p = first_iformat;

	if(p == NULL)
	{
		first_iformat = format;
		format->next = NULL;
	}
	else
	{
		while (p->next !=  NULL) p = p->next;
		 p->next = format;
		format->next = NULL;
	}
}

static AVInputFormat *av_probe_input_format(AVProbeData *pd)
{
	AVInputFormat *fmt1,*fmt;
	int score;
	int find = 0;

	fmt = NULL;
	printf("av_probe_input_format \r\n");


	if(first_iformat == NULL)
		printf("NULL is RRRRRRRRRRRRRR \r\n");

	for(fmt1 = first_iformat; fmt1 != NULL; fmt1 = fmt1->next) {
		if (fmt1->read_probe) {
			if(fmt1->read_probe(pd) > 0)
			{
				printf("find \r\n");
				return fmt1;
				break;
			}
		} 
		
	}

	

	return fmt1;

}


int av_read_header_file(AVFormatContext *ic_ptr)
{
	int ret = -1;
	AVInputFormat *fmt = ic_ptr->iformat;

	if(fmt->read_header)
		ret = fmt->read_header(ic_ptr);

	return ret;
}

int av_open_input_file(AVFormatContext *ic_ptr)
{
	AVProbeData pd;
	AVInputFormat *fmt;
	char buf[DETECT_SIZE];

	int ret;

	ret = fileRead(ic_ptr->fp, buf, DETECT_SIZE);
	if(ret <= 0) return -1;

	if(fileSeek(ic_ptr->fp,0, SEEK_SET) < 0) return -1;
	
	pd.buf = buf;
	pd.buf_size = ret;
	
	fmt = av_probe_input_format(&pd);
	if(!fmt) return -1;

	ic_ptr->iformat = fmt;
	printf("%08X \r\n", ic_ptr->iformat);
	
	return 0;
}

void av_register_all(void)
{
	REGISTER_DEMUXER(AVI, avi);
}