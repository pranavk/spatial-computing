#include <stdio.h>

int sum_triple_loop (int a, int b, int c){
   int su = 0, i, j, k;
   for (i=0;i<a;i++)
      for (j=0; j<b;j++)
         for (k=0; k< c; k++)
            su = su + i + j;
   return su;
}
