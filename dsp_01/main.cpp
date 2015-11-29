#include <iostream>
#include "wave.cpp"

int main(int argc, char *argv[]) {
  

  struct audio *ap;

  if (argc < 2) {
    printf("Arg error\n");
    return 0;
  }

  /* Read */
  ap = read_audio(argv[1]);

  /* Print */
  print_audio_info(ap);

  // WriteWave(name_half, F.field.wBitsPerSample, SamplesPerSec, F.field.wChannels, D.waveformData, D.chunkSize);

  return 0;
}
