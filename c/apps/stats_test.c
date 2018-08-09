#include <stdio.h>

#include "stats.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

static void print_array(double a[], size_t size)
{
  size_t i;

  for (i = 0; i < size; i++) {
    printf("%f ", a[i]);
  }

  printf("\n\n");
}

int main(void)
{
  double a[] = {1.6, 2.7, 3.8, 4.9};
  double sum = 0.0;
  double avg = 0.0;
  double variance = 0.0;
  double stdev = 0.0;
  double min = 0.0;
  double max = 0.0;

  printf("Array:\n");
  print_array(a, ARRAY_SIZE(a));

  sum = stats_sum(a, ARRAY_SIZE(a));
  avg = stats_average(a, ARRAY_SIZE(a));
  variance = stats_var(a, ARRAY_SIZE(a));
  stdev = stats_stdev(a, ARRAY_SIZE(a));
  min = stats_min(a, ARRAY_SIZE(a));
  max = stats_max(a, ARRAY_SIZE(a));

  printf("The sum is %f\n", sum);
  printf("The average is %f\n", avg);
  printf("The variance is %f\n", variance);
  printf("The standard deviation is %f\n", stdev);
  printf("The min value in array is %f\n", min);
  printf("The max value in array is %f\n", max);

  return 0;
}