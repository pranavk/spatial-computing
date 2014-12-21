#include <stdio.h>

int fib (int n){
   int a =0, b =1,i;
   int c;
   for (i = 0; i < n; i++){
      c = a + b;
      a = b;
      b = c;
   }
   return c; 
}
