#include <stdio.h>

int fibc (int n){
   int a =0, b =1,i;
   int c;
   for (i = 0; i < n; i++){
      c = a + b;
      if (c%5==0)
         c = a*b;
      printf ("%d\n",c);
      a = b;
      b = c;
   }
   return c; 
}
