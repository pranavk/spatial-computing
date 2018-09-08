#include <stdio.h>

void foo(int num)
{
   printf("this is foo calling with nuber %d\n", num);
   if (num > 5)
      printf("foo: number is greater than 5\n");
   else if (num > 3)
      printf("greater than 3\n");
   else
      printf("elsing\n");
}
