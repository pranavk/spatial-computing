int fib (int n){
   int a = 0;
   int b = 1;
   int i =0, c = 0;
   for (i = 0; i < n; i++){
      c = a + b;
      a = b;
      b = c;
   }
   return c; 
}
