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
  if (ap == NULL)
    goto fail;

  /* Print info */
  print_audio_info(ap);

  /* Decimation */
  struct audio *new_ap;
  if ((new_ap = Decimation(ap)) == NULL)
    goto fail;

  if (write_audio(new_ap) < 0)
    goto fail;

  /* Print info */
  print_audio_info(new_ap);

  /* End */
  printf("[OK] End\n");
  return 0;

fail:
  printf("[Fail]\n");
  return 0;
}
