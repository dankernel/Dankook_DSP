#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI	3.1415926535

struct twinddle_factor
{
	double real;
	double imag;
};

void tf_print(struct twinddle_factor *tf, int size);
int tf_copy(struct twinddle_factor *a, struct twinddle_factor *b);

struct twinddle_factor *FFT_Calc(
    struct twinddle_factor *result, 
    struct twinddle_factor *input, 
    int size, 
    int inverse, 
    int *index)
{
  int i = 0;
	struct twinddle_factor *t = NULL;
  
  /* Return */
  if (size == 1) {
    tf_copy(&result[(*index)], &input[*(index)]);
    *(index) = *(index) + 1;

    printf("End! : size : %d, H_%d | %lf, %lf \n", size, *index, input[*(index)].real, input[*(index)].imag);
    printf("END input : %lf %lf\n", input[(*index)].real, input[(*index)].imag);
    printf("END result : %lf %lf\n", result[(*index)].real, result[(*index)].imag);
    /* tf_print(input, size);  */
    return NULL;
  }

  if (size == 2) 
  {
  }

	double degree = 2*PI/ size * inverse;
  t = (struct twinddle_factor*)malloc(sizeof(struct twinddle_factor) * size);

  printf(">>>> 111 Loop Size : %d, H_%d \n", size, *index);
  tf_print(input, size); 

  for (i = 0; i < size / 2; i++) {

    t[i].real = input[i].real + input[i + size / 2].real;
		t[i].imag = input[i].imag + input[i + size / 2].imag;
  }

	for (i = 0; i < size / 2; i++)
  {

    t[i + size / 2].real = (input[i].real - input[i + size / 2].real)*cos(degree*i) + (input[i].imag - input[i + size / 2].imag)*sin(degree*i);
		t[i + size / 2].imag = (input[i + size / 2].real - input[i].real)*sin(degree*i) + (input[i].imag - input[i + size / 2].imag)*cos(degree*i);
	}

  printf(">>>> 222 Loop Size : %d, H_%d \n", size, *index);
  tf_print(t, size); 

  FFT_Calc(result, &t[0], size/2, inverse, index);
  FFT_Calc(result, &t[size/2], size/2, inverse, index);
  
  /* printf("%lf \n",cos(degree*4*(1))); */
	return t;
}

int tf_copy(struct twinddle_factor *a, struct twinddle_factor *b)
{
  a->real = b->real;
  a->imag = b->imag;

  return 0;

}

struct twinddle_factor *tf_init(int *array, int size)
{
  int i = 0;

  struct twinddle_factor *tf = NULL;

  if (size <= 0)
    return NULL;

  tf = (struct twinddle_factor*)malloc(sizeof(struct twinddle_factor) * size);

  if (array == NULL)
    goto insert_0;
  else
    goto insert_array;

insert_array:
  for (i = 0; i < size; i++) {
    tf[i].real = array[i];
    tf[i].imag = 0;
  }
  return tf;

insert_0:
  for (i = 0; i < size; i++) {
    tf[i].real = 0;
    tf[i].imag = 0;
  }
  return tf;
}

struct twinddle_factor *Main_FFT(struct twinddle_factor *input, const int size, int inverse)
{
  struct twinddle_factor *result = NULL;
  int *index = NULL;

  /* Init inedx */
  index = (int*)malloc(sizeof(int));
  *index = 0;


  /* Init resutn array */
  result = tf_init(NULL, size);

  /* Run FFT main */
  FFT_Calc(result, input, size, inverse, index);

  printf("Index : %d\n", *index);
  return result;
}



void tf_print(struct twinddle_factor *tf, int size)
{
  int i = 0;

  printf("==== PRINT ====\n");
  for (i = 0; i < size; i++) {
    printf("%p real : %lf / imag : %lf \n", &tf[i], tf[i].real, tf[i].imag);
  }
  printf("==== PRINT : end ====\n");
 
}

int main(int argc, const char *argv[])
{
  int size = 8;
  /* int array[] = {1, 2, 3, 4}; */
  int array[] = {1, 2, 3, 4, 5, 6, 7, 8};
  struct twinddle_factor *tf = NULL;
  struct twinddle_factor *result = NULL;

  tf = tf_init(array, size);

  tf_print(tf, size);

  printf("===== \n\n");
  tf = Main_FFT(tf, size, 1);
  tf_print(tf, size);
  
  return 0;
}
