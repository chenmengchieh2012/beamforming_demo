#ifndef __TRANSMIT_HEADER
#define __TRANSMIT_HEADER

typedef struct header{
	int WiGig_id;
	int sector;
	int RSSI;
	int ack;
}WiGig_header;


#ifndef ACK
#define ACK 1
#endif

#ifndef NAK
#define NAK 1
#endif

#endif