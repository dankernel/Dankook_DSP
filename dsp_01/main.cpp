#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

#include "wave.cpp"
#include "lame/main.c"

#define KB 1024
#define MB KB * 1024
#define GB MB * 1024

char *tmp_dkdk(int argc, char *argv[]) 
{
  // lame_global_flags gf;

  /* initialize libmp3lame */
  // laem_init(&gf);

  if(argc==1) {
    printf("[Fail] Arg error! \n");
  }

  return NULL;


}

char *mp3_encode(char *path) 
{
  int ret = 0;
  int status = 0;

  if (path == NULL)
    return NULL;

  if (strstr(path, ".mp3") == NULL)
    return NULL;

  if ((ret = fork()) == 0) {
    printf("ch\n");
    execlp("ps", "-al", NULL);
  } else {
    waitpid(ret, &status, WNOHANG);
  }
  printf("pe\n");

}


int main(int argc, char *argv[]) {
  
  struct audio *ap;
  long ret = 0;

  if (argc < 2) {
    printf("Arg error\n");
    return 0;
  }

  /* Test */
  tmp_dkdk(argc, argv);

  // mp3_encode(argv[1]);
  

  /* Read */
  // ap = read_audio(argv[1]);
  // if (ap == NULL)
    // goto fail;

  /* Print info */
  // print_audio_info(ap);

  /* Decimation */
  // struct audio *new_ap;
  // if ((new_ap = Decimation(ap)) == NULL)
  //   goto fail;
  //
  // if (write_audio(new_ap) < 0)
  //   goto fail;

  /* Print info */
  // print_audio_info(new_ap);

  /* End */
  printf("[OK] End\n");
  return 0;

fail:
  printf("[Fail]\n");
  return 0;

}

