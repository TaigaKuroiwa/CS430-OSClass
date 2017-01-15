/* Part1 - my thoughts for the answer
5
2
5
7

but the actual answer was
8
0
5
8

:(

The under code is for the part2
*/


#include <stdio.h> 

const struct actions{
	int(*walk)();
	int(*run)();
	int(*eat_dinner)();
};

int me_run(){
	printf("hey I'm walking \n");
	return 0;
}

int me_walk(){
	printf("hey i shall run\n");
	return 0;
}

int me_eat_dinner(){
	printf("no food no fight\n");
	return 0;
}

void call_actions(struct actions* a){
	if(a->walk()){
		printf("We can walk!  Now we try running!\n");
		a->run();
	} else {
		printf("No walking is possible without dinner.\n");
		a->eat_dinner();
	}
}

int main(){
	struct actions me;
	me.walk=me_walk;
	me.run=me_run;
	me.eat_dinner=me_eat_dinner;
	
	call_actions(&me);
	return 0;
}