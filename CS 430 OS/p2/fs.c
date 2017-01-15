
/* Project 2 rules:
*		ALL data must be stored on the simulated hard drive!
*			- No global variables
*			- No static variables
*			- No other contrived method to preserve data between function calls
*/

/*
* This is the only file you need to modify.  You might want to add printing or
* changes stuff in tester.c, but the program should work at the end with the
* initial versions of all files except this one.  Function stubs are included
* so that the project will compile as-received.
*/

#include "fs.h"
#include "drive.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stddef.h>

int fdelete(char* fn) {
		char copy[1024] ="";
		char test[1024];
		char fnchecker[1024];
		char checking[1024];
		memcpy(fnchecker, fn, 1024);
		int i, n, truecy=0, truesec=0, j, x;
		int intfnchecker, next_cylinder=0, next_sector=0;
		x = strlen(fn);
		int z=0;
		for (i = 0; i<CYLINDERS; i++) {
			for (n = 0; n<SECTORS_PER_CYLINDER; n++) {
				read_sector(i, n, test);
				memcpy(checking, test, 1024);
					if (0 == strncmp(checking, fnchecker, strlen(fn))) {
						intfnchecker = 1;
						truecy = i;
						truesec = n;
						
					}
			}
		}
		if (intfnchecker != 1){
			return NOT_FOUND;
		}
		if(intfnchecker == 1) {
		write_sector(truecy, truesec, copy);
			return 0;
	}
}

	int load(char* fn, void* data, size_t ds) {
		char *str = (char*)data;
		char test[1024];
		char fnchecker[1024];
		char checking[1024];
		memcpy(fnchecker, fn, 1024);
		int i, n, truecy=0, truesec=0, j, x;
		int intfnchecker, next_cylinder=0, next_sector=0;
		size_t newds;
		newds = ds;
		x = strlen(fn);
		int z=0;
		for (i = 0; i<CYLINDERS; i++) {
			for (n = 0; n<SECTORS_PER_CYLINDER; n++) {
				read_sector(i, n, test);
				memcpy(checking, test, strlen(fn));
					if (0==strncmp(fnchecker, checking, strlen(fn))) {
						printf("checking and fnchecker: %s, %s\n", checking, fnchecker);
						intfnchecker = 1;
						truecy = i;
						truesec = n;
					}
			}
		}

		printf("the value of truecy and truesec is: %d %d\n", truecy, truesec);
		if (intfnchecker != 1){
		return NOT_FOUND;}

		while (newds > CYLINDER_SIZE) {
			++next_cylinder;
			newds = newds - CYLINDER_SIZE;
		}

		while (newds > BYTES_PER_SECTOR) {
			++next_sector;
			newds = newds - BYTES_PER_SECTOR;
		}
		int k=0;
		int y=0;
		if (next_cylinder > 0) {
			y = next_cylinder + 1;
		}
		else {
			y = 0;
		}
		if (next_sector > 0) {
			k = next_sector + 1;
		}
		else {
			k = 1;
		}

		int charposition = 1;
		//printf("the vlue of k and y: %d :: %d\n", k, y);
		//printf("the vlue of cy and sec: %d :: %d\n", next_cylinder, next_sector);
		if (k>1) {
			z = y+k;
			printf("the value of z: %d\n", z);
			char forbigfile[1024];
			printf("the value of cy and sec: %d, %d\n", truecy, truesec);
			read_sector(truecy, truesec, forbigfile);
			memcpy(str, forbigfile+strlen(fn), 1024);
			for (j = 0; j < z; j++) {
				charposition = (1024 * (j + 1))+12;
				truesec++;
				read_sector(truecy, truesec, forbigfile);
				memcpy(str+charposition, forbigfile, 1024);
				
				if (truesec == 100) {
					truesec = truesec-100;
					n++;
				}if(truecy == 50){
					truecy = truecy - 50;
				}//this is for the save, must adjust for the load　←adjusted
			}return 0;
		}
		else {
			read_sector(truecy, truesec, test);
			memcpy(str, test + x, 1024);
			//printf("cy sec inside: %d, %d \n", truecy, truesec);
			return 0;
		}
	}
	
	void format() {
		char freespace[1024] = "";
		int i, n;
		//("right before the for loop\n");
		for (i = 0; i<CYLINDERS; i++) {
			for (n = 0; n<SECTORS_PER_CYLINDER; n++) {
				write_sector(i, n, freespace);
			}
		}
	}

	int save(char* fn, void* data, size_t ds) {
		//ネームチェック方法
		char *str = (char *)data;
		char newchar[1024];
		char newfn[1024];
		char checker[1024];
		char fnchecker[1024];
		char fnchecker2[strlen(fn)];
		char forbigt[1024];
		char bigtchecker[2] = "";
		//memcpy(bigtchecker, fn, strlen(fn)-3);
		memcpy(fnchecker2, fn, strlen(fn));
		memcpy(newfn, fn, strlen(fn));
		memcpy(newchar, data, 1024);
		strcat(newfn, newchar);
		int i, n, x, y, z, j, fnsize, intfnchecker, intbigtchecker = 0;
		size_t newds;
		newds = ds;
		int next_cylinder=0, next_sector=0;
		fnsize= strlen(fn);
		for (i = 0; i < CYLINDERS; i++) {
			for (n = 0; n < SECTORS_PER_CYLINDER; n++) {
				read_sector(i, n, checker);
				memcpy(fnchecker, checker, fnsize);
					if (0==strncmp(fnchecker, fnchecker2, strlen(fn))) {
						//printf("fnchecker, fnchecker2: %s, %s\n", fnchecker, fnchecker2);
						intfnchecker =1;
					}
			}
		}
		if (intfnchecker == 1){
		return NAME_CONFLICT;
		}
		
		for (i = 0; i < CYLINDERS; i++) {
			for (n = 0; n < SECTORS_PER_CYLINDER; n++){
				read_sector(i, n, checker);
				memcpy(fnchecker, checker, 1024);
				if(0 > strncmp(bigtchecker, fnchecker, 2)){
					intbigtchecker++;
				}
			}
		}
		//printf("intbigtchecker: %d\n", intbigtchecker);
		if(intbigtchecker >= 1500){
			return NO_SPACE;
		}
		
		
		while (newds > CYLINDER_SIZE) {
			++next_cylinder;
			newds = newds - CYLINDER_SIZE;
		}
		while (newds > BYTES_PER_SECTOR) {
			++next_sector;
			newds = newds - BYTES_PER_SECTOR;
		}
		x = next_sector;
		y = next_cylinder;
		int charposition;
		//printf("the vlue of x and y: %d :: %d\n", x, y);

		if (x>1) {
			z = x + y;
			//printf("the value of z: %d\n", z);
			char forbigfile[1024];
			i = rand()%50;
			n = rand()%100;
			//i = 0;
			//n = 0;
			read_sector(i, n, checker);
			while(0 < strncmp(bigtchecker, checker, 2)){
				n++;
				read_sector(i, n, checker);
				if(n=99){
					n = n -99;
					i++;
				}if(i = 49){
					i = i-49;
				}
			}
			write_sector(n, i, newfn);
			//printf("checking i and n: %d %d\n", i, n);
			for (j = 0; j < z; j++) {
				charposition = (1024 * (j + 1))+12;
				memcpy(forbigfile, str + charposition, 1024);
				i++;
				read_sector(i, n, checker);
				while(0 < strncmp(bigtchecker, checker, 2)){
				n++;
				read_sector(i, n, checker);
				if(n=99){
					n = n -99;
					i++;
				}if(i = 49){
					i = i-49;
				}
			}
				write_sector(n, i, forbigfile);
				if (i == 99) {
					i = i -99;
					n++;
					if (n == 49) {
						n= n -49;
					}
				}
			}//printf("bigt file: %s\n", fn);
			return 0;
		}
		else {
			j = rand()%50;
			n = rand()%100;
			/*printf("checking j and n: %d %d\n", j, n);*/
			write_sector(j, n, newfn);
			//printf("newfn inside: %s\n", newfn);
			return 0;
		}
	}

