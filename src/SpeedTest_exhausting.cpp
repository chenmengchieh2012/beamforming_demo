#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/time.h>

#include "iconv.h"
#include "pthread.h"
#include "speedtest.h"

#include "Common/mrloopsdkheader.h"
#include "mrloopbf_release.h"

#include "transmit_header.h"

#define BUFSIZE  4096
// bool is_open, s_index = false, running = false;
// unsigned char count = 0;
// long bits;

// bool rx_rfstatus = false;

// pthread_t thread;
// pthread_t show_bitrate;
// pthread_t getRFstatus;



ML_RF_INF ML_RF_Record;

#define CHUNK 1


#define MAX_SECTOR 10
#define MIN_SECTOR 1

long long current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

void *Tx_exhaustive(void* ptr){
	//setting sector
	int sector = MIN_SECTOR;
	while(1){
		ML_SetTxSector(sector);
		WiGig_header* whptr = WiGig_create_header();
		WiGig_set_sector(whptr,sector);
		int length = sizeof(WiGig_header);
		ML_Transfer((uint8_t *)whptr, length);

		WiGig_free_header(whptr);

		if(sector<MAX_SECTOR){
			sector++;
		}else{
			sector = MIN_SECTOR;
		}
	}

}

void *Rx_exhaustive(void* ptr){
	//setting sector
	int sector = MIN_SECTOR,status;

	while(1){
		ML_SetRxSector(sector);


		int flag[MAX_SECTOR] = {0};
		int flag_counter = 0;
		long tstart,tend;

		tstart = current_timestamp();

		while(1){
			WiGig_header* whptr = WiGig_create_header();
			WiGig_set_sector(whptr,sector);
			// ML_SendRFStatusReq();
			int length;
			status = ML_Receiver((uint8_t*)whptr, &length);
			// ML_DecodeRFStatusPacket((uint8_t*)whptr, &ML_RF_Record);
			if(status > 0){
				sector = WiGig_get_sector(whptr);
				if(flag[sector] == 0){
					flag[sector] = 1;
					flag_counter++;
				}
				// fprintf(stdout,"RSSI(dBm): %d\n", ML_RF_Record.PHY_RSSI);
				fprintf(stdout,"Sector: %d\n",WiGig_get_sector(whptr));
				
			}
			WiGig_free_header(whptr);

			tend = current_timestamp();
			if(tend - tstart > 1000){
				break;
			}

			if(flag_counter == 10){
				break;
			}

			
		}
		

		if(sector<MAX_SECTOR){
			sector++;
		}else{
			sector = MIN_SECTOR;
		}

		
		
		fprintf(stdout,"chagne time: %ld\n",(tend-tstart));
	}

}

#define CONFIG_FILE "config.txt"

// key of transmit in config.txt
#define KEY_TRANSMIT_MODE "Mode"

#define MAX_KEY_LENGTH 64
#define MAX_VALUE_LENGTH 64

#define TX 1
#define RX 2

pthread_t thread;

int main(int argc, char *argv[]){
	int mode = 0;
	if(ML_Init() != 1){
		return 0;
	}

	FILE* fp;

	char *key;
	char line[MAX_KEY_LENGTH];
	int value;

	if((fp = fopen(CONFIG_FILE, "r")) == NULL){
		printf("file not found");
	    return NULL;
	}
	while(fscanf(fp,"%s\n", &line[0], &value) != EOF){
		// printf("%s",line);

		char *token;

		token = strtok(line, ":");
		key = token;		
		token = strtok(NULL, ":");
		value = atoi(token);


		if(strcmp(key,KEY_TRANSMIT_MODE) == 0){
			if(value == RX){
				mode = RX;
			}else if(value == TX){
				mode = TX;
			}
		}
		memset(key,0,MAX_KEY_LENGTH);
	}
	if(mode == TX){

		// void *ret;
		fprintf(stdout,"TX\n");
		pthread_create(&thread, NULL , Tx_exhaustive , NULL);
		pthread_join( thread, NULL);

	}else if (mode == RX){
		// void *ret;
		fprintf(stdout,"RX\n");
		pthread_create(&thread, NULL , Rx_exhaustive , NULL );
		pthread_join( thread, NULL);
	}
	


}