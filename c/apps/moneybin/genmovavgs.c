#include <stdlib.h>
#include <stdio.h>

#include "crossover.h"
#include "sma.h"
#include "ta.h"

static void print_movavg(ta_bars_t *b, double *ma1, double *ma2) {
  int i;
  int year, month, day;
  timestamp_t *timestamp;

  printf("# date\t open\t high\t low\t close\t adjClose\t volume\t 5-day moving average close\t 20-day moving average close\n");

  for (i = 0; i < b->numrows; i++) {
    timestamp = &b->timestamp[i];

    ta_getdate(timestamp, &year, &month, &day);

    printf("%04d-%02d-%02d ", year, month, day);
    printf("%9.3f ", b->open[i]);
    printf("%9.3f ", b->high[i]);
    printf("%9.3f ", b->low[i]);
    printf("%9.3f ", b->close[i]);
    printf("%9.3f ", b->adjclose[i]);
    printf("%d ", b->volume[i]);
    printf("%9.3f ", *(ma1 + i));
    printf("%9.3f ", *(ma2 + i));
    putc('\n', stdout);
  }
}

int main(void)
{
  int err;
  char filename[13] = "PETR4.SA.csv";
  ta_bars_t *bars;
  double *ma1, *ma2;
  int window1 = 5 /* fast period */, window2 = 20 /* slow period */;
  int year, month, day;
  timestamp_t *timestamp;
  size_t pos;
  int crossover;

  /* Read bar data from CSV file. */
  err = read_csv(filename, &bars);
  if (err < 0) {
    return err;
  }

  /* Allocate memory for the moving average array. */
  ma1 = malloc(sizeof(double) * bars->numrows);
  if (ma1 == NULL) {
    return -1;
  }

  /* Allocate memory for the moving average array. */
  ma2 = malloc(sizeof(double) * bars->numrows);
  if (ma2 == NULL) {
    return -1;
  }

  /* Calculate the moving averages. */
  smovavg(bars->close, bars->numrows, window1, ma1);
  smovavg(bars->close, bars->numrows, window2, ma2);

  print_movavg(bars, ma1, ma2);

  return 0;
}