#include <iostream>
#include "wave.cpp"

int main(int argc, char *argv[]) {
  
  struct audio *ap;
  long ret = 0;

  if (argc < 2) {
    printf("Arg error\n");
    return 0;
  }

  /* Read */
  ap = read_audio(argv[1]);

  /* Print */
  print_audio_info(ap);

  ret = write_mod_samplingrate(ap, 2);
  printf("Old Samples Per sec : %ld \n", ret);

  // print_bin(ap);

  write_audio(ap);
  // WriteWave(name_half, F.field.wBitsPerSample, SamplesPerSec, F.field.wChannels, D.waveformData, D.chunkSize);

  return 0;
}
