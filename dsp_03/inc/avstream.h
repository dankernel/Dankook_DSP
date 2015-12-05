#ifndef AVSTREAM_H
#define AVSTREAM_H

#include "common.h"

enum FILEFORMAT {
	AVI_FORMAT = 1,
};

struct AVFormatContext;

typedef struct _AVProbeData {
    unsigned char *buf; /**< Buffer must have AVPROBE_PADDING_SIZE of extra allocated bytes filled with zero. */
    int buf_size;       /**< Size of buf except extra allocated bytes */
} AVProbeData;

typedef struct _AVInputFormat {
	const char *name;
	const char *long_name;
	int priv_data_size;
	int (*read_probe)(AVProbeData *);
	int (*read_header)(struct AVFormatContext *);	
	struct AVInputFormat *next;
} AVInputFormat;

typedef struct AVFormatContext {

	struct AVInputFormat *iformat;
	//	struct AVOutputFormat *oformat;
	int formatType;
	void *filecontain;
	int64_t duration;
	int64_t start_time;
	FILE *fp;
} AVFormatContext;




#endif
