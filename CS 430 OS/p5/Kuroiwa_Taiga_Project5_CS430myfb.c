//Project 5, Taiga Kuroiwa
#include<stdio.h>
#include<signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


void dontstop(int param){
	printf("We got a %d, but we're not stopping!\n", param);
}	

int main(){
	struct sigaction handler;
	handler.sa_handler = dontstop;
	sigaction(SIGINT, &handler, 0);
	sigaction(SIGQUIT, &handler, 0);
	sigaction(SIGSTOP, &handler, 0);
	sigaction(SIGKILL, &handler, 0);
	sigaction(SIGTERM, &handler, 0);
	
	FILE *fp;
	char s[256];
	
	if(fork()){
		return 0;
	}
	for(;;){
		if ((fp = fopen("savemefrombomb.txt", "r")) == NULL){
			fork();
		}else{
			break;
		}
	}
	return 0;
}