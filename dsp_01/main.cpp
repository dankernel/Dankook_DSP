#include <iostream>
#include "wave.cpp"

int main(int argc, char** argv) {

  RiffHeader R;
  FormatChunk F;
  DataChunk D;

  long SamplesPerSec;
  char name_input[100] = "test.wav";
  unsigned char *data_out;
  int ret = 0;

  ret = ReadWave(name_input, &R, &F, &D);
  if (ret < 1)
    printf("Err ret : %d\n", ret);

  if (F.field.wBitsPerSample != 8 || F.field.wChannels != 1)
    return -1;

  data_out = (unsigned char *)malloc(sizeof(char) * D.chunkSize * 2);
  interpolation(D.waveformData, D.chunkSize, data_out);
  WriteWave("out.wav", F.field.wBitsPerSample, 2 * F.field.dwSamplesPerSec, 
      F.field.wChannels, data_out, D.chunkSize * 2);

  // write_do();
  // write_mod_samplingrate();
  return 0;
}
