#include <stdio.h>

void foo(int);
void fun();

int main() {
   printf("this is main.\n");
   int a;
   printf("Enter your number: ");
   scanf("%d", &a);
   if (a>  5)
       fun();
   else
       foo(a * a);
   printf("this is after fun\n");
   return 0;
}
