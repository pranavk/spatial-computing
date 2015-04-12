#include <stdio.h>

int sum_single_loop (int a){
   int su = 3, i;
   for (i=0;i<a;i++)
      su = su + i;
   return su;
}
