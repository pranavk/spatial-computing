#include <stdio.h>

void bubble_sort(){
   int arr[] = {5,1,9,12,99,0,9};
   int noEle = sizeof(arr)/sizeof(arr[0]);
   int i,j,temp;
   for (i =0; i < noEle; i++){
      for (j = 0; j < noEle - 1; j++){
         if (arr[j] > arr[j+1]){
            temp = arr[j];
            arr[j] = arr[j+1];
            arr[j+1] = temp;
         }
      }
   }
}
