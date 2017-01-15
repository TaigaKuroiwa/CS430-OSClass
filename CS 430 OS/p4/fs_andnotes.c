/*
CS440 Project4
Taiga Kuroiwa
down notes are all my websites that I used to study, so don't worry. 
http://www.math.kobe-u.ac.jp/~kodama/tips-free-memory.html Linux のメモリー管理(メモリ－が足りない？,メモリーリークの検出/防止)

http://blog.livedoor.jp/sonots/archives/39965025.html　valgrind使い方　その１
http://hiroom2.jimdo.com/2015/06/15/valgrind%E3%81%AE%E4%BD%BF%E3%81%84%E6%96%B9/　　valgrindの使い方 その２ こっちのほうが詳しい

http://www.geocities.co.jp/SiliconValley-Bay/7437/c/malloc.html 簡単なmallocとよくある問題に対する解決方法
http://detail.chiebukuro.yahoo.co.jp/qa/question_detail/q1418979754　freeの問題への解決策かも？トライの価値あり
http://www.ced.is.utsunomiya-u.ac.jp/lecture/2004/prog/p2/kadai2/1_alloc.php　初歩の初歩、メモリの説明

http://oswald.hatenablog.com/entry/20081129/1228697674 構造体のfree化の際の注意

*/


/* Project 2 rules:
 *              ALL data must be stored on the simulated hard drive!
 *                      - No global variables
 *                      - No static variables
 *                      - No other contrived method to preserve data between function calls
 */

/*
 * This is the only file you need to modify.  You might want to add printing or
 * changes stuff in tester.c, but the program should work at the end with the
 * initial versions of all files except this one.
 */

#include "fs.h"
#include "drive.h"

#include<string.h>
#include<stdio.h>
#include<stdlib.h>

/*
 * Specific to my solution:
 *      - The free space table is stored at the beginning of the drive.
 *      - The FAT comes next, and can grow
 *      - Files are stored at the END of the drive first, so the FAT
 *      can grow toward them
 *      - Chaining:  snum of next location is 1st 4 bytes of sector, 0 if none
 */

#define FREESPACE_SECTORS (TOTAL_SECTORS / BYTES_PER_SECTOR + 1)
#define USED 1
#define FREE 2
#define SN_C(s) (s / SECTORS_PER_CYLINDER)
#define SN_S(s) (s % SECTORS_PER_CYLINDER)
#define SPARAM(s) SN_C(s), SN_S(s)
#define USSIZE (BYTES_PER_SECTOR - 4)
#define TEP ((BYTES_PER_SECTOR-4) / sizeof(struct table_entry))

struct table_entry {
        char fname[12];
        int cylinder;
        int sector;
        size_t size;
};

int fdelete(char* fn){
        return load(fn, 0, 0);
}

size_t savesector(void* data) {
        int fs_sector = FREESPACE_SECTORS - 1;
        char* sector = (char*)malloc(BYTES_PER_SECTOR);
        int check;
        while(fs_sector > -1){
                read_sector(0, fs_sector, sector);
                check = BYTES_PER_SECTOR;
                while(check > 0) {
                        if(sector[check] == FREE)
                                goto foundit;
                        check--;
                }
                fs_sector--;
        }
		//free(sector->fname);
		//free(sector->cylinder);
		//free(sector->sector);
		//free(sector->size);
		free(sector);
        return NO_SPACE;
foundit:
        sector[check] = USED;
        write_sector(0, fs_sector, sector);
        int snum = fs_sector * BYTES_PER_SECTOR + check;
        write_sector(SPARAM(snum), data);
		//free(sector->fname);
		//free(sector->cylinder);
		//free(sector->sector);
		//free(sector->size);
		free(sector);
        return snum;
}

int is_free(int c, int s, int mark){
        int snum = c * SECTORS_PER_CYLINDER + s;
        int sector = snum / BYTES_PER_SECTOR;
        int insector = snum % BYTES_PER_SECTOR;
        // Free space table must fit in the first cylinder
        char* data = (char*)malloc(BYTES_PER_SECTOR);//how should i free this?
        read_sector(0, sector, data);
        if(mark){
                data[insector] = mark;
                write_sector(0, sector, data);
        }
		char temp[BYTES_PER_SECTOR];
		strncpy(temp, data, BYTES_PER_SECTOR);
		free(data);
        return data[insector];
}

int load(char* fn, void* data, size_t ds){
        int* sector = (int*)malloc(BYTES_PER_SECTOR);
        read_sector(0, FREESPACE_SECTORS, sector);
        int FAT_end = sector[0];
        int FAT_sector = 0;
        struct table_entry* tstart = (struct table_entry*)(sector + 1);
        int c = 0;
        int s = 0;
        struct table_entry te;
        while(1) {
                for(struct table_entry* i = tstart; i < tstart + TEP; i++)
                        if(!strcmp(i->fname, fn)){
                                memcpy(&te, i, sizeof(struct table_entry));
                                if(!ds) {
                                        i->fname[0] = 0;
                                        write_sector(0, FREESPACE_SECTORS + FAT_sector, sector);
                                }
                                goto foundit;
                        }
                FAT_sector++;
                int* extraleak = (int*)calloc(BYTES_PER_SECTOR / sizeof(int), 1);
                if(FAT_sector * TEP == FAT_end){
						free(extraleak);
						free(sector);
						return NOT_FOUND;
				}
                read_sector(0, FREESPACE_SECTORS + FAT_sector, sector);
                tstart = (struct table_entry*)(sector + 1);
				free(extraleak);
        }
		free(sector);
        return NOT_FOUND;
foundit:
        c = te.cylinder;
        s = te.sector;
        int sectornum = 0;
        int next = 0;
        size_t space_left = ds;
        size_t left = te.size;
        while(c != 0) {
                read_sector(c, s, sector);
                next = sector[0];
                if(ds){
                        if(left > USSIZE && space_left > USSIZE)
                                memcpy(data + (sectornum * USSIZE), sector+1, USSIZE);
                        else if(space_left < left) {
                                memcpy(data + (sectornum * USSIZE), sector+1, space_left);
								free(sector);
                                return NO_SPACE;
                        }
                        else if(space_left >= left) {
                                memcpy(data + (sectornum * USSIZE), sector+1, left);
								free(sector);
                                return 0;
                        }
                        space_left -= USSIZE;
                        sectornum++;
                        left -= USSIZE;
                } else
                        is_free(c, s, FREE);
                c = SN_C(next);
                s = SN_S(next);
        }
		free(sector);
        return 0;
}
void format() {
        // Write freespace table, all used beyond end
        char* sector = (char*)malloc(BYTES_PER_SECTOR);
        for(int i = 0; i < FREESPACE_SECTORS; ++i){
                for(char* j = sector; j < sector + BYTES_PER_SECTOR; j++)
                        *j = FREE;
                // Special cases
                switch(i) {
                        case 0:
                                for(int j = 0; j < FREESPACE_SECTORS+1; ++j) // +1 for FAT
                                        sector[j] = USED;
                                break;
                        case FREESPACE_SECTORS - 1:
                                for(int j = TOTAL_SECTORS % BYTES_PER_SECTOR; j < BYTES_PER_SECTOR; j++)
                                        sector[j] = USED;
                }
                write_sector(0, i, sector);
        }
        // Write initial FAT
        for(char* j = sector; j < sector + BYTES_PER_SECTOR; j++)
                *j = 0;
        int* isector = (int*)sector;//do i need to free this too?
        isector[0] = TEP;
        write_sector(0, FREESPACE_SECTORS, sector);
		free(sector);
}

int save(char* fn, void* data, size_t ds){
        // First, check to see if the name is used
        int spot;
        if(load(fn, &spot, 4) != NOT_FOUND)
                return NAME_CONFLICT;
        int f_sectors = ds/USSIZE;
        if(ds%USSIZE)
                f_sectors++;
        int last = 0;
        int* sector = (int*)malloc(BYTES_PER_SECTOR);
        while(f_sectors){
                f_sectors--;
                int cpysize = (f_sectors * USSIZE < ds)? USSIZE:f_sectors*USSIZE-ds;
                sector[0] = last;
                memcpy(sector+1, data + (USSIZE * f_sectors), cpysize);
                //size_t* sbl = (size_t*)calloc(100, sizeof(size_t));//where did this sbl go?!
                last = savesector(sector);
                if(last == NO_SPACE){
						free(sector);
						//free(sbl);
                        return NO_SPACE; // TODO:  Cleanup partial file
        }
		}
        struct table_entry te;
        te.cylinder = SN_C(last);
        te.sector = SN_S(last);
        te.size = ds;

        strcpy(te.fname, fn);
        read_sector(0, FREESPACE_SECTORS, sector);
        int FAT_end = sector[0];
        int FAT_sector = 1;
        struct table_entry* tstart = (struct table_entry*)(sector + 1);
        while(1){
                for(struct table_entry* i = tstart; i < tstart + TEP; i++)
                        if(!(i->fname[0])){
                                memcpy(i, &te, sizeof(struct table_entry));
                                write_sector(0, FREESPACE_SECTORS + FAT_sector - 1, sector);
								free(sector);
								//free(sbl);
                                return 0;
                        }
                if(FAT_sector * TEP == FAT_end){
                        if(is_free(0, FREESPACE_SECTORS + FAT_sector, 0) == FREE) {
                        // We need to add a sector to the FAT
                                for(int* i = sector; i < sector + (sizeof(sector)/sizeof(int)); ++i)
                                        *i = 0;
                                write_sector(0, FREESPACE_SECTORS + FAT_sector, sector);
                                is_free(0, FREESPACE_SECTORS + FAT_sector, USED);
                                FAT_end += TEP;
                                read_sector(0, FREESPACE_SECTORS, sector);
                                sector[0] += TEP;
                                write_sector(0, FREESPACE_SECTORS, sector);
                        } else {
                                // TODO:  Delete half-written file!
								free(sector);
								//free(sbl);
                                return NO_SPACE;
                        }
                }
                FAT_sector++;
                read_sector(0, FREESPACE_SECTORS + FAT_sector - 1, sector);
                tstart = (struct table_entry*)(sector + 1);
        }
		free(sector);
		//free(sbl);
        return 0;
}
