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
#include <assert.h>

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
char *myturn;

typedef struct {
  char *Sub_Topic;
  char *ServerIP;
}MQTT_INFO;

#define CHUNK 1

#define MAX_SECTOR 10
#define MIN_SECTOR 1

/* current_timestamp */
long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

/* MQTT */
void mqtt_pub(char *pubTopic){
  char *command;
  char cmd[] = "python test_pub.py";
  if((command = (char*)malloc(strlen(cmd)+strlen(pubTopic)+1)) != NULL){
      command[0] = '\0';
      strcat(command,cmd);
      strcat(command,pubTopic);
  } else {
      fprintf(stderr, "malloc failed!\n");
  }
  execl(command, (char*)0);
}

/* MQTT */
#define TERMINAL_CMD "gnome-terminal -x"
#define SUB_SUBSCRIBE_EXEC_LANG "python"
#define SUB_SUBSCRIBE_EXEC_FILE "src/test_sub.py"
#define STRING_SPACE " "

void add_command(char* command,char* item){
  strcat(command,item);
  strcat(command,STRING_SPACE);
  return;
}

char* generate_command(char* cmd_item[],int command_length, int size){
  char *command;
  int iter;
  if((command = (char*)malloc(command_length*sizeof(char)+size)) != NULL){
    command[0] = '\0';
    for( iter=0 ; iter<size ; iter++){
      add_command(command,cmd_item[iter]);
    }
  }else {
      fprintf(stderr, "malloc failed!\n");
  }
  return command;
}

void* mqtt_sub(void *ptr){
  MQTT_INFO *MQTT_Info = (MQTT_INFO*)ptr;
  char *command;
  int command_length = strlen(TERMINAL_CMD)+ \
                       strlen(SUB_SUBSCRIBE_EXEC_LANG)+ \
                       strlen(SUB_SUBSCRIBE_EXEC_FILE)+ \
                       strlen(MQTT_Info->ServerIP)+ \
                       strlen(MQTT_Info->Sub_Topic)+1;
  char *command_item[] = {TERMINAL_CMD,SUB_SUBSCRIBE_EXEC_LANG,SUB_SUBSCRIBE_EXEC_FILE \
          ,MQTT_Info->ServerIP,MQTT_Info->Sub_Topic};
  command = generate_command(command_item, command_length, 5);
  printf("command : %s\n", command);
  system(command);
  free(command);
  free(MQTT_Info);
}

/* checkfile */
void *checkfile(void* ptr){
  while(true){
    FILE* fp;
    char line[64];
    if((fp = fopen("isSwitched", "r")) == NULL){
      printf("file not found");
        return 0;
    }
    if(fscanf(fp,"%s\n", &line[0]) != EOF){
      memcpy(myturn, line, sizeof(line));
    }
    fclose(fp);

    FILE* fp2;
    if((fp2 = fopen("isSwitched", "w")) == NULL){
      printf("file not found");
        return 0;
    }
    fclose(fp2);
  }
}

/* Tx_exhaustive */
void *Tx_exhaustive(char *Pub_Topic){
	//setting sector
  int ischanged = 1;
	int sector = MIN_SECTOR;
	while(1){
		if(ML_Init() != 1){
			return 0;
		}
		ML_SetTxSector(sector);
		ML_SetSpeed(2);
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

    if(ischanged == MAX_SECTOR) {
			/* change to dongle 2 */
      mqtt_pub(Pub_Topic);
      break;
		}

		if(sector<MAX_SECTOR){
			sector++;
		}else{
			sector = MIN_SECTOR;
      ischanged++;
		}
	}
}

/* Rx_exhaustive */
void *Rx_exhaustive(void* ptr){
	//setting sector
	int rx_sector = MIN_SECTOR,status;

	while(1){
		if(ML_Init() != 1){
			return 0;
		}
		ML_HiddenDebugMsg();
		ML_SetSpeed(2);
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
				ML_GetRFStatus(&ML_RF_Record);
				fprintf(stdout,"RSSI(dBm): %d\n", ML_RF_Record.PHY_RSSI);
				fprintf(stdout,"Tx Sector: %d\n",tx_sector);
				fprintf(stdout,"Rx Sector: %d\n",rx_sector);

			}
			free(whptr);
			free(buf);

			tend = current_timestamp();
			if(tend - tstart > 1000){ // check if timeout
				break;
			}

			if(flag_counter == 10){
				break;
			}
		}

		ML_Close();

		if(rx_sector<MAX_SECTOR) {
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
#define SUB_TOPIC "Sub_Topic"
#define PUB_TOPIC "Pub_Topic"
#define SERVERIP "ServerIP"

#define MAX_KEY_LENGTH 64
#define MAX_VALUE_LENGTH 64

#define TX 1
#define RX 2

// #define TEST

pthread_t thread;

int main(int argc, char *argv[]){
	int mode = 0;
  char *Sub_Topic, *Pub_Topic, *ServerIP;
	ML_Close();

	FILE* fp;

	char *key;
	char line[MAX_KEY_LENGTH];
	int value;

	if((fp = fopen(CONFIG_FILE, "r")) == NULL){
		printf("file not found");
	    return 0;
	}
	while(fscanf(fp,"%s\n", &line[0]) != EOF){
		char *token;
		token = strtok(line, ":");
		key = token;
		token = strtok(NULL, ":");
		value = atoi(token);

    // printf("key : %s\n", key);
    // printf("value : %s\n", token);

		if(strcmp(key,KEY_TRANSMIT_MODE) == 0){
			if(value == RX){
				mode = RX;
			}else if(value == TX){
				mode = TX;
			}
		}else if(strcmp(key, SUB_TOPIC) == 0){
      Sub_Topic = strdup(token);
    }else if(strcmp(key, PUB_TOPIC) == 0){
      Pub_Topic = strdup(token);
    }else if(strcmp(key, SERVERIP) == 0){
      ServerIP = strdup(token);
    }
		memset(key,0,MAX_KEY_LENGTH);
	}
  fclose(fp);

//-----------------------------------------
//TEST
#ifdef TEST
  #define SUB_TOPIC_VALUE "a"
  #define PUB_TOPIC_VALUE "b"
  #define SERVERIP_VALUE "192.168.100.5"
  assert(!strcmp(Sub_Topic,SUB_TOPIC_VALUE));
  assert(!strcmp(Pub_Topic,PUB_TOPIC_VALUE));
  assert(!strcmp(ServerIP,SERVERIP_VALUE));
  exit(1);
#endif
//----------------------------------------- s

  pthread_t thread_mqtt;
  MQTT_INFO *MQTT_Info = (MQTT_INFO*)malloc(sizeof(MQTT_INFO));
  MQTT_Info->Sub_Topic = Sub_Topic;
  MQTT_Info->ServerIP = ServerIP;
  pthread_create(&thread_mqtt, NULL, mqtt_sub, (void*)MQTT_Info);
  // mqtt_sub(Sub_Topic, ServerIP);
	if(mode == TX){

		// void *ret;
		fprintf(stdout,"TX\n");
		pthread_create(&thread, NULL , checkfile , NULL);
		pthread_join(thread, NULL);
    myturn = strdup(Sub_Topic);
    while(true){
      if(myturn == Sub_Topic){
        memset(myturn,'\0',MAX_KEY_LENGTH);
        Tx_exhaustive(Pub_Topic);
      }
    }
    free(myturn);
	}else if (mode == RX){
		// void *ret;
		// fprintf(stdout,"RX\n");
		pthread_create(&thread, NULL , Rx_exhaustive , NULL );
		pthread_join( thread, NULL);
	}
  pthread_join(thread_mqtt, NULL);
  free(MQTT_Info);

}
