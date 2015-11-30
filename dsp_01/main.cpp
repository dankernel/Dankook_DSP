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
  printf("[OK] Read \n");

  /* Print info */
  print_audio_info(ap);

  /* Sampling rate  */
  struct audio *new_ap;
  new_ap = Decimation(ap);
  write_audio(new_ap);
  print_audio_info(new_ap);

  printf("[OK] End\n");

  return 0;
}
