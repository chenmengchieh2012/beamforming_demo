
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "transmit_header.h"
#define CONFIG_FILE "config.txt"
#define MAX_KEY_LENGTH 64
#define KEY_WIGIG_ID "WiGig_id"

// #define UNITTEST 1


WiGig_header* WiGig_create_header(){
	FILE* fp;

	if((fp = fopen(CONFIG_FILE, "r")) == NULL){
	    return NULL;
	}

	char key[MAX_KEY_LENGTH];
	int value;
	WiGig_header* wiGig_header = (WiGig_header*) malloc(sizeof(WiGig_header));
	
	while(fscanf(fp,"%s:%d\n", &key[0], &value) != EOF){
		if(strcmp(key,KEY_WIGIG_ID) == 0){
			wiGig_header->WiGig_id = value;
		}
		memset(key,0,MAX_KEY_LENGTH);

	}
	return wiGig_header;
}

void WiGig_set_sector(WiGig_header* ptr, int sector){
	if(ptr == NULL){
		return;
	}
	ptr->sector = sector;
	return;
}

int WiGig_get_sector(WiGig_header* ptr){
	if(ptr == NULL){
		return 0;
	}
	return ptr->sector;
}

int WiGig_get_ID(WiGig_header* ptr){
	if(ptr == NULL){
		return 0;
	}
	return ptr->WiGig_id;
}

void WiGig_free_header(WiGig_header* ptr){
	free(ptr);
	return;
}

#ifdef UNITTEST

int main(){
	WiGig_header* whptr = WiGig_create_header();
	WiGig_set_sector(whptr,1);
	printf("wigig id : %d\n",WiGig_get_ID(whptr));
	printf("sector : %d\n",WiGig_get_sector(whptr));
	WiGig_free_header(whptr);
	return 0;
}

#endif