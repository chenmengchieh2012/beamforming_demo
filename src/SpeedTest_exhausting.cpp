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
		if(ML_Init() != 1){
			return 0;
		}
		ML_SetTxSector(sector);
		ML_SetSpeed(7);
		ML_HiddenDebugMsg();
		WiGig_header* whptr = WiGig_create_header();
		WiGig_set_sector(whptr,sector);
		int length = sizeof(WiGig_header);
		fprintf(stdout,"sector: %d\n",sector);

		unsigned char* buf = (unsigned char*) malloc(BUFSIZE * CHUNK  * sizeof(char));
		if(buf == NULL && whptr == NULL){
			ML_Close();
			continue;
		}
		memset(buf, 0, BUFSIZE * CHUNK);
		memcpy(buf,whptr,length);

		int status;
		status = ML_Transfer(buf, BUFSIZE *CHUNK);
		fprintf(stdout,"tx status: %d\n",status);
		ML_Close();

		free(whptr);
		free(buf);

		if(sector<MAX_SECTOR){
			sector++;
		}else{
			sector = MIN_SECTOR;
		}
		
	}

}

void *Rx_exhaustive(void* ptr){
	//setting sector
	int rx_sector = MIN_SECTOR,status;

	while(1){
		if(ML_Init() != 1){
			return 0;
		}
		ML_HiddenDebugMsg();
		ML_SetSpeed(7);
		ML_SetRxSector(rx_sector);


		int flag[MAX_SECTOR] = {0};
		int flag_counter = 0;
		long tstart,tend;

		tstart = current_timestamp();

		while(1){
			WiGig_header* whptr = WiGig_create_header();
			// WiGig_set_sector(whptr,sector);
			// ML_SendRFStatusReq();
			int length = sizeof(WiGig_header);

			uint8_t* buf = (uint8_t*) malloc(BUFSIZE * CHUNK);
			int Rx_length = BUFSIZE * CHUNK;

			if(buf == NULL && whptr == NULL){
				ML_Close();
				continue;
			}

			status = ML_Receiver(buf, &Rx_length);
			fprintf(stdout,"status: %d\n",status);
			memcpy(whptr,buf,length);
			
			if(status > 0){
				int tx_sector;
				tx_sector = WiGig_get_sector(whptr);
				if(flag[tx_sector] == 0){
					flag[tx_sector] = 1;
					flag_counter++;
				}
				ML_SendRFStatusReq();
				ML_DecodeRFStatusPacket(buf, &ML_RF_Record);
				fprintf(stdout,"RSSI(dBm): %d\n", ML_RF_Record.PHY_RSSI);
				fprintf(stdout,"Tx Sector: %d\n",tx_sector);
				fprintf(stdout,"Rx Sector: %d\n",rx_sector);
				
			}
			free(whptr);
			free(buf);

			tend = current_timestamp();
			if(tend - tstart > 1000){
				break;
			}

			if(flag_counter == 10){
				break;
			}

			
		}
		usleep(10000);
		ML_Close();

		if(rx_sector<MAX_SECTOR){
			rx_sector++;
		}else{
			rx_sector = MIN_SECTOR;
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


	FILE* fp;

	char *key;
	char line[MAX_KEY_LENGTH];
	int value;

	if((fp = fopen(CONFIG_FILE, "r")) == NULL){
		printf("file not found");
	    return 0;
	}
	while(fscanf(fp,"%s\n", &line[0]) != EOF){
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