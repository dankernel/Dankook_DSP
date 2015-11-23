#include "wave.h"

void LongToString(long H_chunkID, char *chunkID)
{
  long MASK = 0x000000FF;

  chunkID[0] = (char)(H_chunkID & MASK);
  chunkID[1] = (char)((H_chunkID >> 8) & MASK);
  chunkID[2] = (char)((H_chunkID >> 16) & MASK);
  chunkID[3] = (char)((H_chunkID >> 24) & MASK);
  chunkID[4] = '\0';
}

int ReadWave(char *filename, RiffHeader *R, FormatChunk *F, DataChunk *D) //wav2dat
{
  FILE *fp;
  HEADER H;
  char chunkID[5];

  if ((fp = fopen(filename, "rb")) == NULL)
  {
    printf("\nCannot read file.\n\n");
    return 0;
  }

  //청크는 chunkID 와 chunkSize 로 시작함
  // 청크의 시작이 없을 때 까지 반복
  while (0 != fread(&H, sizeof(HEADER), 1, fp))
  {
    LongToString(H.chunkID, chunkID);
    if (strcmp(chunkID, "RIFF") == 0) // RIFF chunk 인지 확인
    {
      R->chunkID = H.chunkID;
      R->chunkSize = H.chunkSize;
      fread(&(R->wFormat), sizeof(R->wFormat), 1, fp);
      LongToString(R->wFormat, chunkID);

      if (strcmp(chunkID, "WAVE") != 0) // WAVE chunk 인지 확인
      {
        printf("\nNot supported format.\n\n");
        return 0;
      }
    }

    // format chunk 처리
    else if (strcmp(chunkID, "fmt ") == 0)
    {
      F->chunkID = H.chunkID;
      F->chunkSize = H.chunkSize;
      fread(&(F->field), sizeof(F->field), 1, fp);
      // 8bit, 16bit가 아닐때 예외처리
      if (!(F->field.wBitsPerSample == 8) && !(F->field.wBitsPerSample == 16))
      {
        printf("\n%d bits per sample is not supported.\n\n", F->field.wBitsPerSample);
        return -1;
      }

      // chunk size 가 16 이상인 경우
      if (H.chunkSize > sizeof(F->field))
      {
        // 다음 chunk ID 위치로 이동
        fseek(fp, (long)(H.chunkSize - sizeof(F->field)), SEEK_CUR);
      }
    }
    // data chunk 처리
    else if (strcmp(chunkID, "data") == 0)
    {
      D->chunkID = H.chunkID;
      D->chunkSize = H.chunkSize;
      D->waveformData
        = (unsigned char *)malloc(sizeof(char) * H.chunkSize);

      if (D->waveformData == NULL)
        return -1;
      fread(D->waveformData, 1, H.chunkSize, fp);
    }
    // RIFF, fmt, data 이외에는 처리하지 않음
    else
    {
      fseek(fp, (long)H.chunkSize, SEEK_CUR);
    }
  }
  fclose(fp);
  return 1;
}

unsigned char* WaveToPCM(char* name, long* data_size)
{
  RiffHeader R;
  FormatChunk F;
  DataChunk D;

  ReadWave(name, &R, &F, &D);

  *data_size = D.chunkSize;
  if (F.field.wBitsPerSample != 8){
    printf("\n BitsPerSample of input wave file must be 8 bits !!!");
  }

  return(D.waveformData);
}


void read()
{
  RiffHeader R;
  FormatChunk F;
  DataChunk D;

  char name[100] = "Demo.wav";
  double PlayTime;

  ReadWave(name, &R, &F, &D);

  PlayTime = (double)D.chunkSize / (F.field.dwSamplesPerSec * F.field.wChannels * F.field.wBitsPerSample / 8);

  printf("\n[] Data chunk size = %d", D.chunkSize);
  printf("\n[] The number of channel = %d (mono = 1, stereo = 2)", F.field.wChannels);
  printf("\n[] Sampling rate = %d [Hz or samples/sec]", F.field.dwSamplesPerSec);
  printf("\n[] Sample resolution = %d [bits/sample]", F.field.wBitsPerSample);
  printf("\n[] Play time = %lf [sec]\n", PlayTime);
}


int WriteWave2(char *filename, RiffHeader R, FormatChunk F, DataChunk D)
{
  FILE *fp;

  if ((fp = fopen(filename, "wb")) == NULL){
    printf("\t File Open Failure\n");
    return(0);
  }

  fwrite(&R, sizeof(R), 1, fp);

  fwrite(&F, sizeof(F), 1, fp);

  fwrite(&(D.chunkID), sizeof(D.chunkID), 1, fp);
  fwrite(&(D.chunkSize), sizeof(D.chunkSize), 1, fp);
  fwrite(D.waveformData, sizeof(char), D.chunkSize, fp);

  fclose(fp);

  return(1);
}

int WriteWave(char *name,
    short BitsPerSample,   // 8bit과 16bit만 허용 
    long SamplesPerSec, // sampling rate
    short Channel, // channels
    unsigned char *waveform_data, // PCM data
    long waveform_data_size // PCM data 의 길이
    )
{
  RiffHeader R;
  FormatChunk F;
  DataChunk D;

  R.chunkID = 0x46464952; // "RIFF"
  R.chunkSize = 16 + waveform_data_size + 20;
  // PCM data 길이 + RIFF, Format 청크의 길이 - 8 Byte
  R.wFormat = 0x45564157; // "WAVE"

  F.chunkID = 0x20746d66;  // "fmt "
  F.chunkSize = 16; // PCM 이므로 추가 사항 없음
  F.field.wFormatTag = 1; // PCM
  F.field.wChannels = Channel;
  F.field.dwAvgBytesPerSec = SamplesPerSec * Channel * (BitsPerSample / 8);
  F.field.dwSamplesPerSec = SamplesPerSec;
  F.field.wBitsPerSample = BitsPerSample;
  F.field.wBlockAlign = Channel*(BitsPerSample / 8);

  D.chunkID = 0x61746164;   // "data"
  D.chunkSize = waveform_data_size;

  D.waveformData = waveform_data;

  WriteWave2(name, R, F, D);

  return(1);
}

/*
 * Creating do file
 */
void write_do()
{
  char name[100] = "do.wav";

  static double freq[] = {264.0, 297.0, 330.0, 352.0, 396.0, 440.0, 495.0, 528.0}; // 도 의 주파수

  long SamplesPerSec = 22050; // 22kHz sampling
  short BitsPerSample = 8; // 8bits
  short Channels = 1; // mono
  double SamplesPeriod = 1 / SamplesPerSec;
  double PlayTime = 8;
  long waveformDataSize;
  unsigned char *waveformData;
  double t, f;
  long index;

  waveformDataSize = (long)(PlayTime*SamplesPerSec*Channels*(BitsPerSample / 8));
  waveformData = (unsigned char *)malloc(sizeof(char)*waveformDataSize);

  t = 0.0;
  f = freq[0];
  for (index = 0; index < waveformDataSize; index++, t += 1.0 / SamplesPerSec) {
    f = freq[(int)(index / 22050)];  // 도 ~
    waveformData[index] = (int)(128.0 + 100.0 * sin(2.0 * PI * f * t) + 0.5);
  }

  printf("%10ld %10ld\n", index, f);
  WriteWave(name, BitsPerSample, SamplesPerSec, Channels, waveformData, waveformDataSize);

  free(waveformData);
}

void write_mod_samplingrate()
{
  RiffHeader R;
  FormatChunk F;
  DataChunk D;

  long SamplesPerSec;
  char name_input[100] = "headset2.wav";
  char name_half[100] = "half.wav";
  char name_double[100] = "double.wav";
  ReadWave(name_input, &R, &F, &D);

  SamplesPerSec = F.field.dwSamplesPerSec / 2;
  WriteWave(name_half, F.field.wBitsPerSample, SamplesPerSec, F.field.wChannels, D.waveformData, D.chunkSize);

  SamplesPerSec = F.field.dwSamplesPerSec * 2;
  WriteWave(name_double, F.field.wBitsPerSample, SamplesPerSec, F.field.wChannels, D.waveformData, D.chunkSize);
}

void interpolation(unsigned char *input, long size, unsigned char *output)
{
  long i;
  unsigned char *zeropad = (unsigned char *)malloc(sizeof(char) * size * 2);
  unsigned char value;

  // Zero padding 
  for (i = 0; i < size; i++) {
    zeropad[2 * i] = input[i];
    zeropad[2 * i + 1] = 0;
  }

  // Conv
  for (i = 1; i < size * 2 - 1; i++) {
    value = (int)((zeropad[i - 1] + 2 * zeropad[i] + zeropad[i + 1]) / 2.0);
    output[i] = (unsigned char)CLIPPING(value);
  }

}

void subsmapling2to1(unsigned char *input, long size, unsigned char *output)
{
  long i;

  for (i = 0; i < size; i += 2) {
    output[i / 2] = input[i];
  }

}


void NoiseGen(int *noise, long size, int range)
{
  long i;

  for (i = 0; i < size; i++) {
    noise[i] = (rand() % (2 * range + 1) - range);
    /* code */
  }
}

void write_noise()
{
  char name_in[100] = "headset2.wav";
  char name_out[100] = "test_demo+noise.wav";
  char name1[100] = "test_noise.wav";
  long SamplesPerSec = 11025; // 11kHz
  short BitsPerSample = 8; // 8bits
  short Channels = 1; // mono
  double SamplesPeriod = 1 / SamplesPerSec;
  double PlayTime = 5;
  long waveformDataSize;
  unsigned char * waveformData;
  int * noise;
  long i;

  RiffHeader R;
  FormatChunk F;
  DataChunk D;

  waveformDataSize = (long)(5 /*  playtime = 5 */ * 11025 * 1 *(8 / 8));
  waveformData = (unsigned char *)malloc(sizeof(char)*waveformDataSize);
  noise = (int *)malloc(sizeof(int) * waveformDataSize);

  // 잡음 생성
  NoiseGen(noise, waveformDataSize, 30); // range = 30;
  for (i = 0; i < waveformDataSize; i++)
    waveformData[i] = CLIPPING(noise[i] + 128);
  WriteWave(name1, BitsPerSample, SamplesPerSec, Channels, waveformData, waveformDataSize);

  // 잡음 + 다른 사운드 데이터
  ReadWave(name_in, &R, &F, &D);
  free(noise);
  noise = (int *)malloc(sizeof(int) * D.chunkSize);

  NoiseGen(noise, D.chunkSize, 5);
  for (i = 0; i < D.chunkSize; i++)
    D.waveformData[i] = CLIPPING(D.waveformData[i] + noise[i]);

  WriteWave(name_out, F.field.wBitsPerSample, F.field.dwSamplesPerSec, F.field.wChannels, D.waveformData, D.chunkSize);

  free(waveformData);
  free(noise);
}

unsigned char *MemoryAllocationAndDataCopy(unsigned char *waveformData, long chunkSize, int hSize)
{
  unsigned char *tmp = NULL;
  int i;

  tmp = (unsigned char *) malloc(sizeof(char) * chunkSize);

  for (i = 0; i < chunkSize; i++) {
    tmp[i] = waveformData[i];
  }


  return tmp;
}


// 노이즈 제거 (mean filter)
void write_filter_meanfilter()
{
  // input 은 노이즈 섞인 파일
  char name_in[100] = "test_demo+noise.wav";
  char name_out[100] = "test_demo+noise+meanfilter.wav";
  char name_out2[100] = "test_demo+noise+gaussian.wav";

  long i;
  unsigned char *filter;
  int temp, filterSize, hSize, k;
  // 가우시안 필터 (size 5)
  double gfilter[5] = { 0.0625, 0.25, 0.375, 0.25, 0.0625 };
  double tmp;

  RiffHeader R;
  FormatChunk F;
  DataChunk D;

  filterSize = 5; // 필터 크기는 홀수만 가능
  hSize = (int)(filterSize / 2);
  ReadWave(name_in, &R, &F, &D);

  // 메모리 복사
  filter = MemoryAllocationAndDataCopy(D.waveformData, D.chunkSize, hSize);

  for (i = 0; i < D.chunkSize; i++)
  {
    temp = 0;
    for (k = -hSize; k <= hSize; k++)
    {
      temp += (filter[i + k] - 128);
    }
    D.waveformData[i] = CLIPPING(temp / filterSize + 128);
  }

  WriteWave(name_out, F.field.wBitsPerSample, F.field.dwSamplesPerSec, F.field.wChannels, D.waveformData, D.chunkSize);

  // free(filter - hSize);

  // gaussian 필터링
  hSize = (int)(sizeof(sizeof(gfilter) / sizeof(double))) / 2;
  filter = MemoryAllocationAndDataCopy(D.waveformData, D.chunkSize, hSize);

  for (i = 0; i < D.chunkSize; i++) {
    tmp = 0.0;
    for (k = -hSize; k <= hSize; k++) {
      tmp += ((filter[i + k] - 128) * (gfilter[k + hSize]));
    }
    D.waveformData[i] = CLIPPING((int)tmp + 128);
  }

  WriteWave(name_out2, F.field.wBitsPerSample, F.field.dwSamplesPerSec, F.field.wChannels, D.waveformData, D.chunkSize);

  // free(filter - hSize);
}

unsigned char GetMaxWaveform(unsigned char * data, long size)
{
    long i;
      unsigned char max;
        max = abs(data[0] - 128); // max 값 초기화

          // 최대 진폭 찾기
        for (i = 1; i < size; i++)
        {
          if (abs(data[i] - 128) > max)
            max = abs(data[i] - 128);
        }

        return max;
}

// 정규화
void write_normalize()
{
  char name_in[100] = "headset2.wav";
  char name_out[100] = "test_normalize.wav";

  long i;
  unsigned char * data, maxvalue;
  double scale;

  RiffHeader R;
  FormatChunk F;
  DataChunk D;

  ReadWave(name_in, &R, &F, &D);
  data = D.waveformData;
  maxvalue = GetMaxWaveform(data, D.chunkSize);
  scale = (double)128 / maxvalue; // 증폭량 결정
  printf("normalization x%.1lf (maximum waveform level %d)\n", scale, maxvalue);

  // 증폭
  for (i = 0; i < D.chunkSize; i++)
  {
    data[i] = CLIPPING((data[i] - 128) * scale + 128.0);
  }

  WriteWave(name_out, F.field.wBitsPerSample, F.field.dwSamplesPerSec, F.field.wChannels, data, D.chunkSize);
}

// 기음과 배음을 발생시키는 함수 Harmonics()
unsigned char Harmonics(int type /*  배음의 종류*/, double f /*  기음의 주파수 */, double t /*  시간 변수 */)
{
  double value;

  switch (type)
  {
    case 0: // 기음으로 구성
      value = 50.0 * sin(2.0 * PI * f * t);
      break;
    case 1: // 2~5배의 주파수를 가진 배음을 첨가한 소리로 구성
      value = 50.0 * sin(2.0 * PI * f * t) +
        30.0 * sin(2.0 * PI * (2.0 * f) * t) +
        20.0 * sin(2.0 * PI * (3.0 * f) * t) +
        10.0 * sin(2.0 * PI * (4.0 * f) * t) +
        5.0 * sin(2.0 * PI * (5.0 * f) * t);
      break;
    case 2: // 2, 4, 6, 8배의 주파수를 가진 배음을 첨가한 소리로 구성
      value = 50.0 * sin(2.0 * PI * f * t) +
        30.0 * sin(2.0 * PI * (2.0 * f) * t) +
        20.0 * sin(2.0 * PI * (4.0 * f) * t) +
        10.0 * sin(2.0 * PI * (6.0 * f) * t) +
        5.0 * sin(2.0 * PI * (8.0 * f) * t);
      break;
    default:
      value = 50.0 * sin(2.0 * PI * f * t);
      break;
  }
  return ((unsigned char)(value + 128.0)); // 128을 더해 출력
}


// 화음
void write_harmonics()
{
  char name[100] = "test_harmonics.wav";

  //도, 레, 미, 파, 솔, 라, 시, 도의 주파수
  static double freq[] = {
    264.0, 297.0, 330.0, 352.0, 396.0, 440.0, 495.0, 528.0
  };

  long SamplesPerSec = 11025; // 11kHz sampling
  short BitsPerSample = 8; // 8bits
  short Channels = 1; // mono
  double SamplesPeriod = 1 / SamplesPerSec;
  double PlayTime = 1; // 한 음정은 1초씩
  long waveformDataSize;
  unsigned char *waveformData;
  double t, f;
  long index, length;
  int i, type, num = 8; // 8개의 음정

  length = (long)(PlayTime * SamplesPerSec * Channels * (BitsPerSample / 8));
  waveformDataSize = length * num;
  waveformData = (unsigned char *)malloc(sizeof(char)*waveformDataSize);

  type = 1;

  for (i = 0; i < num; i++)
  {
    f = freq[i]; // 음정
    for (index = 0, t = 0.0; index < length; index++, t += 1.0 / SamplesPerSec)
    {
      waveformData[length * i + index] = Harmonics(type, f, t);
    }
  }

  WriteWave(name, BitsPerSample, SamplesPerSec, Channels, waveformData, waveformDataSize);
  free(waveformData);
}
