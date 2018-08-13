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
#include <stdarg.h>

#include "iconv.h"
#include "pthread.h"
#include "speedtest.h"

#include "Common/mrloopsdkheader.h"
#include "mrloopbf_release.h"

#include "transmit_header.h"

#define BUFSIZE  4096
#define TOPIC_SIZE 64
#define DEBUG(...) do { fprintf(stdout,__VA_ARGS__); fflush(stdout); } while (0)

ML_RF_INF ML_RF_Record;
char myturn[TOPIC_SIZE];

typedef struct {
  char *Sub_Topic;
  char *ServerIP;
}MQTT_INFO;

#define CHUNK 1
#define ROUNDS 10

#define MAX_SECTOR 10
#define MIN_SECTOR 1

char max_sector, min_sector;

// void out_fmt(){
//   DEBUG("The output format : \n");
//   DEBUG("status\n");
//   DEBUG("RSSI(dBm)\n");
//   DEBUG("Tx Sector\n");
//   DEBUG("Rx Sector\n");
//   DEBUG("time cost\n");
//   DEBUG("---------\n");
// }

int _length(unsigned char* s,int size) {
    int i = 0,iter=0;
    for(iter=0;iter<size;iter++){
      if(s[iter] != 0){
        ++i;
      }
    }
    return i;
}

/* current_timestamp */
long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

/* MQTT */
#define TERMINAL_CMD "gnome-terminal -x"
#define SUB_SUBSCRIBE_EXEC_LANG "python"
#define SUB_SUBSCRIBE_EXEC_FILE "src/test_sub.py"
#define SUB_PUBLISH_EXEC_FILE "src/test_pub.py"
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

void mqtt_pub(char *pubTopic, char *serverIP){
  char *command;
  int command_length = strlen(SUB_SUBSCRIBE_EXEC_LANG)+ \
                       strlen(SUB_PUBLISH_EXEC_FILE)+ \
                       strlen(serverIP)+ \
                       strlen(pubTopic)+1;
  char *command_item[] = {(char*)SUB_SUBSCRIBE_EXEC_LANG, (char*)SUB_PUBLISH_EXEC_FILE \
                          , serverIP, pubTopic};
  command = generate_command(command_item, command_length, 4);
  system(command);
  free(command);
}

void* mqtt_sub(void *ptr){
  MQTT_INFO *MQTT_Info = (MQTT_INFO*)ptr;
  char *command;
  int command_length = strlen(TERMINAL_CMD)+ \
                       strlen(SUB_SUBSCRIBE_EXEC_LANG)+ \
                       strlen(SUB_PUBLISH_EXEC_FILE)+ \
                       strlen(MQTT_Info->ServerIP)+ \
                       strlen(MQTT_Info->Sub_Topic)+1;
  char *command_item[] = {(char*)TERMINAL_CMD, (char*)SUB_SUBSCRIBE_EXEC_LANG ,(char*)SUB_SUBSCRIBE_EXEC_FILE \
          , MQTT_Info->ServerIP, MQTT_Info->Sub_Topic};
  command = generate_command(command_item, command_length, 5);
  printf("command : %s\n", command);
  system(command);
  free(command);
  free(MQTT_Info);
  return ((void *)0);
}

void init_isSwitched_file(){
  FILE* fp;
  fp = fopen("isSwitched.txt", "w");
  if(fp == NULL){
    printf("file not found");
  }
  fclose(fp);
}

/* checkfile */
void *checkfile(void* ptr){
  char *topic = (char*)ptr;
  // printf("[ checkfile ]\n");
  char line[TOPIC_SIZE];
  while(true){
    FILE* fp;
    if((fp = fopen("isSwitched.txt", "r")) == NULL){
      printf("file not found");
    }else if(fscanf(fp,"%s\n", &line[0]) != EOF){
      strcpy(myturn, line);
    }
    fclose(fp);
    if(!strcmp(myturn, topic)){
      memset(myturn, 0, strlen(myturn));
      init_isSwitched_file();
    }
    memset(line,0,TOPIC_SIZE);
  }
  return ((void *)0);
}

void* set_zoom(void* ptr){
  char temp;
  while(true){
    FILE* f;
    if((f = fopen("optimization.txt", "r")) == NULL){
    }else if(fscanf(f, "%s\n", &temp) != EOF){
        int central_sector = (int)temp;
        max_sector = char(central_sector + 1);
        min_sector = char(central_sector - 1);
    }
  }
}

/* Tx_exhaustive */
void Tx_exhaustive(char *Pub_Topic, char *ServerIP){
  int ischanged = 0;
  max_sector = MAX_SECTOR;
  min_sector = MIN_SECTOR;
	char sector = min_sector, shift = 10;
	while(1){
		if(ML_Init() != 1){
      exit(1);
		}
		ML_SetTxSector(sector);
		ML_SetSpeed(2);
		ML_HiddenDebugMsg();
		WiGig_header* whptr = WiGig_create_header();
		WiGig_set_sector(whptr,sector);
		int length = sizeof(WiGig_header);
		DEBUG("sector: %d\n",sector);

		unsigned char* buf = (unsigned char*) malloc(BUFSIZE * CHUNK  * sizeof(char));
		if(buf == NULL && whptr == NULL){
			ML_Close();
			continue;
		}
		memset(buf, 0, BUFSIZE * CHUNK);
		memcpy(buf,whptr,length);

		int status;
		status = ML_Transfer(buf, BUFSIZE * CHUNK);
		DEBUG("tx status: %d\n",status);
		ML_Close();

		free(whptr);
		free(buf);

		if(sector < max_sector){
			sector++;
		}else if(sector == max_sector){
			sector = MIN_SECTOR;
      ischanged++;
		}

    if(ischanged == max_sector * ROUNDS) {
			/* change to dongle 2 */
      printf("[ switch ]\n");
      // usleep(100);
      mqtt_pub(Pub_Topic, ServerIP);
      break;
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

			int length = sizeof(WiGig_header);

			uint8_t* buf = (uint8_t*) malloc(BUFSIZE * CHUNK);
      memset(buf, 0, BUFSIZE*CHUNK);
      int Rx_length = BUFSIZE * CHUNK;

			if(buf == NULL && whptr == NULL){
				ML_Close();
				continue;
			}
			status = ML_Receiver(buf, &Rx_length);
      DEBUG("status:%d\n",status);
			memcpy(whptr,buf,length);

			if(status > 0){
				int tx_sector;
				tx_sector = WiGig_get_sector(whptr);
				if(flag[tx_sector] == 0){
					flag[tx_sector] = 1;
					flag_counter++;
				}
				ML_GetRFStatus(&ML_RF_Record);
        DEBUG("RSSI(dBm):%d\n", ML_RF_Record.PHY_RSSI);
        DEBUG("Tx Sector:%d\n",tx_sector);
        DEBUG("Rx Sector:%d\n",rx_sector);
        fprintf(stdout,"message size : %d\n",_length(buf,BUFSIZE * CHUNK));
			}else{
        // DEBUG("RSSI(dBm):\n");
        // DEBUG("Tx Sector:\n");
        // DEBUG("Rx Sector:\n");
      }
			free(whptr);
			free(buf);

			tend = current_timestamp();
			if(tend - tstart > 1000){    // check if timeout
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

    // DEBUG("The time cost of each Rx Sector:%ld\n",(tend-tstart));
	}
  return ((void *)0);
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
  char *Sub_Topic = NULL, *Pub_Topic = NULL, *ServerIP = NULL;
	ML_Close();
  memset(myturn, 0, TOPIC_SIZE);

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
// #ifdef TEST
//   #define SUB_TOPIC_VALUE "a"
//   #define PUB_TOPIC_VALUE "b"
//   #define SERVERIP_VALUE "140.113.207.102"
//   assert(!strcmp(Sub_Topic,SUB_TOPIC_VALUE));
//   assert(!strcmp(Pub_Topic,PUB_TOPIC_VALUE));
//   assert(!strcmp(ServerIP,SERVERIP_VALUE));
//   exit(1);
// #endif
//-----------------------------------------

	if(mode == TX){
    init_isSwitched_file();
    pthread_t thread_mqtt, thread_zoom;
    MQTT_INFO *MQTT_Info = (MQTT_INFO*)malloc(sizeof(MQTT_INFO));
    MQTT_Info->Sub_Topic = Sub_Topic;
    MQTT_Info->ServerIP = ServerIP;
    pthread_create(&thread_zoom, NULL, set_zoom, NULL);
    pthread_create(&thread_mqtt, NULL, mqtt_sub, (void*)MQTT_Info);
		DEBUG("TX\n");
    int iter_rounds = 1;
		pthread_create(&thread, NULL , checkfile , (void*)Sub_Topic);
    strcpy(myturn, Sub_Topic);
    while(true){
      if(!strcmp(myturn, Sub_Topic)){
        memset(myturn, 0, strlen(myturn));
        printf("==== [ Round %2d ] ====\n", iter_rounds);
        Tx_exhaustive(Pub_Topic, ServerIP);
        iter_rounds++;
      }
    }
    pthread_join(thread_mqtt, NULL);
    pthread_join(thread, NULL);
    pthread_join(thread_zoom, NULL);
    free(MQTT_Info);
	}else if (mode == RX){
		pthread_create(&thread, NULL , Rx_exhaustive , NULL );
		pthread_join( thread, NULL);
	}
  free(Sub_Topic);
  free(Pub_Topic);
  free(ServerIP);
}
