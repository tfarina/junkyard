/*
 * Code from: www.geeksforgeeks.org/binary-representation-of-a-given-number
 */

#include <stdio.h>

/* To print binary representation of unsigned integer, start from 31th bit,
 * check wheter 31th bit is ON or OFF, if it is ON print "1" else print "0".
 * Do this for all bits from 31 to 0.
 */
static void bin(unsigned n) {
  unsigned i;
  for (i = 1 << 31; i > 0; i = i / 2) {
    (n & i) ? printf("1") : printf("0");
  }
}

int main(int argc, char **argv) {
  bin(120);
  printf("\n");

  return 0;
}
