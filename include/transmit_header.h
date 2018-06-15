#ifndef __TRANSMIT_HEADER
#define __TRANSMIT_HEADER

typedef struct header{
	int WiGig_id;
	int sector;
	// int ack;
}WiGig_header;
WiGig_header* WiGig_create_header();
void WiGig_free_header(WiGig_header* ptr);
int WiGig_get_ID(WiGig_header* ptr);
void WiGig_set_sector(WiGig_header* ptr, int sector);
int WiGig_get_sector(WiGig_header* ptr);
void WiGig_free_header(WiGig_header* ptr);
// #ifndef ACK
// #define ACK 1
// #endif

// #ifndef NAK
// #define NAK 1
// #endif

#endif