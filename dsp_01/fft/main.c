#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI	3.141

struct twinddle_factor
{
	double real;
	double imag;
};

void tf_print(struct twinddle_factor *tf, int size);

struct twinddle_factor *FFT_Calc(struct twinddle_factor *input, const int size)
{
  int i = 0;
	double degree = 360 / size;

	struct twinddle_factor *t = (struct twinddle_factor*)malloc(sizeof(struct twinddle_factor) * size);

  for (i = 0; i < size / 2; i++) {
    t[i].real = input[i].real + input[i + size / 2].real;
		t[i].imag = input[i].imag + input[i + size / 2].imag;

    /* printf("%d i %lf %d  isize %lf\n",i,input[i].real,i+size/2,input[i+size/2].real); */
  }

	for (i = 0; i < size / 2; i++) {
		t[i + size / 2].real = (input[i].real + input[i + size / 2].real)*cos(degree*i) + (input[i].imag - input[i+size/2].imag)*sin(degree*i);
		t[i + size / 2].imag = (input[i + size / 2].real - input[i].real)*sin(degree*i) + (input[i].imag + input[i + size / 2].imag)*cos(degree*i);
	}

	return t;
}
void Main_FFT(int size)
{

}

struct twinddle_factor *tf_init(int *array, int size)
{
  int i = 0;

  struct twinddle_factor *tf = NULL;

  if (size < 1)
    return NULL;

  tf = (struct twinddle_factor*)malloc(sizeof(struct twinddle_factor) * size);

  for (i = 0; i < size; i++) {
    tf[i].real = array[i];
    tf[i].imag = 0;
  }
  
  return tf;
}

void tf_print(struct twinddle_factor *tf, int size)
{
  int i = 0;

  for (i = 0; i < size; i++) {
    printf("%p real : %lf / imag : %lf \n", &tf[i], tf[i].real, tf[i].imag);
  }
 
}

int main(int argc, const char *argv[])
{
  int array[] = {1, 2, 3, 4};
  struct twinddle_factor *tf = NULL;

  tf = tf_init(array, 4);

  tf_print(tf, 4);

  printf("===== \n\n");
  tf = FFT_Calc(tf, 4);
  tf_print(tf, 4);
  
  return 0;
}
