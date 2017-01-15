#include "tester.h"
#include <stdlib.h>
#include <stdio.h>
//plz check the getbest part too. I just needed to get rid off
//from the code inorder to compile. sorry. 

int getbest(void* calling, int para, size_t numf){
	int i, top=0;
	/*
	int (*func)(int) = ((**void)calling[i]);
	int temp = size_t calling(para);
	for(i=0; i < numf; ++i)
		if(top < temp){
			top = temp;
		}
		
		*/
	return top;
	//function and pass it, pointers to pointers
	
}

int sorting(int* array, size_t length){
	int *p_array;
	p_array = (int*) malloc(length * sizeof(int));
	
	int i, min, j, m;
	
	for (i=0; i<length; ++i){
		p_array[i]=array[i];
	}
	for (i=0; i<length ; ++i) 
	{
		min = i;
		for ( j= i+1; j<length; ++j)
		{
			if(p_array[j]<p_array[min])
				min = j;
		}
		if(min != i)
		{
			int temp;
			temp = p_array[i];
			p_array[i] = p_array[min];
			p_array[min]=temp;
		}
	}
	return p_array;
}

int com(int n1, int n2){
	int total = 0;
	int sub = 0;
	total = n1 | n2;
	sub = total >> 3;
	sub = sub ^ 0x2e;
	return sub;
}



int main() {
	struct bundle_struct mystruct = {
	.find_best=getbest,
	.sorted_copy=sorting,
	.combine=com,
	};
	test(&mystruct);
	return 0;
}
