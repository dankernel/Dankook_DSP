#ifndef AVI_H
#define AVI_H

#include <stdint.h>

#define AVIF_HASINDEX           0x00000010        // Index at end of file?
#define AVIF_MUSTUSEINDEX       0x00000020
#define AVIF_ISINTERLEAVED      0x00000100
#define AVIF_TRUSTCKTYPE        0x00000800        // Use CKType to find key frames?
#define AVIF_WASCAPTUREFILE     0x00010000
#define AVIF_COPYRIGHTED        0x00020000

#define AVI_MAX_RIFF_SIZE       0x40000000LL
#define AVI_MASTER_INDEX_SIZE   256

#define AVINDEX_KEYFRAME 0x0001


/* index flags */
#define AVIIF_INDEX             0x10

typedef	unsigned short				WORD;
typedef	unsigned int				DWORD;
typedef int 						LONG;

#define AVI_STREAM_MAX		20

#define AVI_AUDIO 				1
#define AVI_VIDEO 				2

typedef struct _AVIINDEXENTRY{
	DWORD ckid;
	DWORD dwFlags;
	DWORD dwChunkOffset;
	DWORD dwChunkLength;
	DWORD timestamp;

} AVIINDEXENTRY;


typedef struct _AviMainHeader {
	DWORD dwMicroSecPerFrame; // frame display rate (or 0)
	DWORD dwMaxBytesPerSec; // max. transfer rate
	DWORD dwPaddingGranularity; // pad to multiples of this
	DWORD  dwFlags; // the ever-present flags
	DWORD  dwTotalFrames; // # frames in file
	DWORD  dwInitialFrames;
	DWORD  dwStreams;
	DWORD  dwSuggestedBufferSize;
	DWORD  dwWidth;
	DWORD  dwHeight;
	DWORD  dwReserved[4];
} AviMainHeader;


typedef struct _AVIStreamHeader {
	DWORD  fccType;
	DWORD  fccHandler; //stream_codec_tag
	DWORD  dwFlags;
	DWORD wPriority;
	DWORD wLanguage;
	DWORD dwInitialFrames;
	DWORD dwScale;
	DWORD dwRate; /* dwRate / dwScale == samples/second */
	DWORD dwStart;
	DWORD dwLength; /* In units above... */
	DWORD dwSuggestedBufferSize;
	DWORD dwQuality;
	DWORD dwSampleSize;
	WORD left;
	WORD top;
	WORD right;
	WORD bottom;//// streamheader

	int64_t cum_len;
	int codectype; // video stream format
	int codecwidth;
	int codecheight;
	int bits_per_coded_sample;
	AVIINDEXENTRY *aviIndexEntry;
	unsigned int nb_index_entries;
} AVIStreamHeader;

typedef struct tagBITMAPINFOHEADER { 
	DWORD biSize; //x
	LONG biWidth; //0
	LONG biHeight; //0
	WORD biPlanes; //x
	WORD biBitCount; //0 bits_per_coded_sample
	DWORD biCompression;// 0 //codec
	DWORD biSizeImage; //x
	LONG biXPelsPerMeter; //x
	LONG biYPelsPerMeter; //x
	DWORD biClrUsed; //x
	DWORD biClrImportant; //x
} BITMAPINFOHEADER; 


typedef struct _AVIStream {
	int avPlay;
	int nbStreams;
	int64_t aviSize;
	int64_t moviList;
	int64_t moviEnd;
	int index_loaded;
	AviMainHeader mainHeader;
	AVIStreamHeader streamHeader[AVI_STREAM_MAX];
} AVIStream;




#endif
