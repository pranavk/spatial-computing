#include <stdio.h>

int sum_double_loop (int a, int b){
   int su = 0, i, j;
   for (i=0;i<a;i++)
      for (j=0; j<b;j++)
         su = su + i + j;
   return su;
}
