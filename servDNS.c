/* servDNS.c */

#include <stdlib.h>
#include <stdio.h>
#include <cnaiapi.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define BUFFSIZE 255 // Max hostname length
#define PORT_LISTEN 20000 // Listen on default port 20000
#define LOCAL " LOCAL"
#define NON_LOCAL " non-LOCAL"
#define UNKNOWN " UNKNOWN"
#define NOT_FOUND "-1"

/*-----------------------------------------------------------------------
 *
 * Program: DNS Server
 * Purpose: Accept DNS queries from clients, resolve hostnames to IP addresses
 * Usage:   ./servDNS
 *
 *-----------------------------------------------------------------------
 */

// DNS record structure
typedef struct dns_rec_struct {
	char cname[255];
	computer ip_address;
} dns_rec_struct;

char* comp_to_dotted_ip(computer comp);
void load_dns_cache(dns_rec_struct dns_cache[]);
computer resolve_query(dns_rec_struct dns_cache[], char *hostname);
int update_dns_cache(int index, dns_rec_struct dns_cache[], computer non_local_ip, char *hostname);

int main(int argc, char *argv[]){

	connection conn;
	char buff[BUFFSIZE];
	dns_rec_struct dns_cache[5];
	computer res_ip_addr;
	char *dotted_ip;
	int curr_index = sizeof(dns_cache);

	// Load local DNS cache with data from text file
	load_dns_cache(dns_cache);

	// Incorrect number of command line arguments
	if (argc > 1) {
		(void) fprintf(stderr, "Usage: %s\n", argv[0]);
		exit(1);
	}

	fprintf(stdout, "[DEBUG] DNS server running on port %d\n", PORT_LISTEN);
	fflush(stdout);

	// Wait for DNS queries from clients
	while(1){
		conn = await_contact((appnum) PORT_LISTEN);

		// Could not establish socket connection
		if (conn < 0){
			fprintf(stdout, "[ERROR] Failed to establish socket connection\n");
			exit(1);
		}

		fprintf(stdout, "\n[DEBUG] Incoming connection established on socket with fd: %d\n", conn);
		fflush(stdout);

		// Clear `buff` array before each use
		memset(buff, 0, BUFFSIZE);

		// Wait for DNS query
		while(recv(conn, buff, BUFFSIZE, 0) > 0){
			fflush(stdout);
			fprintf(stdout, "[DEBUG] Received DNS Query for: %s\n", buff);
			fflush(stdout);

			// IP address found in the local DNS cache
			if((res_ip_addr = resolve_query(dns_cache, buff)) != -1) {
				// Convert from long to dotted IP format
				dotted_ip = comp_to_dotted_ip(res_ip_addr);
				(void) send(conn, dotted_ip, strlen(dotted_ip), 0);
				(void) send(conn, LOCAL, sizeof(LOCAL), 0);

				// IP address not found in local DNS cache
			} else if((res_ip_addr = cname_to_comp(buff)) != -1) {

				// Print debug message
				fprintf(stdout, "[DEBUG] Record not found in local DNS cache\n[DEBUG] Resolving DNS Query using author's API\n\n");
				fflush(stdout);

				// Resolving using author's API function
				// Convert from long to dotted IP format
				dotted_ip = comp_to_dotted_ip(res_ip_addr);
				(void) send(conn, dotted_ip, strlen(dotted_ip), 0);
				(void) send(conn, NON_LOCAL, strlen(NON_LOCAL), 0);

				// Update local DNS cache with new record
				curr_index = update_dns_cache(curr_index, dns_cache, res_ip_addr, buff);

			} else {
				// Print debug message
				fprintf(stdout, "[DEBUG] Could not resolve dns query\n");
				fflush(stdout);
				// Hostname could not be resolved at all
				(void) send(conn, NOT_FOUND, strlen(NOT_FOUND), 0);
				(void) send(conn, UNKNOWN, strlen(UNKNOWN), 0);

			}

			// Clear `buff` array after each use
			memset(buff, 0, BUFFSIZE);
		}

		// Terminate main socket thread
		send_eof(conn);
	}

	return 0;
}


void load_dns_cache(dns_rec_struct dns_cache[]){

	FILE *fp;
	char *filename = "dns_cache.txt";
	int i = 0; // Iteration counter

	// Open text file
	fp = fopen(filename, "r");

	// File not found or corrupted
	if (fp == NULL){
        (void) fprintf(stderr, "Could not open file `%s`\n", filename);
				exit(1);
	}

	// Iterate till the end of file
	while(!feof(fp) && i < 5){
		// Read IP address and CNAME
		fscanf(fp, "%d %s\n", &dns_cache[i].ip_address, dns_cache[i].cname);
		i++;
	}

	// Close file
	fclose(fp);

}

computer resolve_query(dns_rec_struct dns_cache[], char *hostname){
	// Print debug message
	fprintf(stdout, "[DEBUG] Resolving DNS Query locally\n\n");
	fflush(stdout);

	// Iterate through local DNS cache
	int i;
	for(i=0; i<5; i++){
		if(strcmp(dns_cache[i].cname, hostname) == 0){
			return dns_cache[i].ip_address;
		}
	}
	// Not found in cache
	return -1;
}

int update_dns_cache(int index, dns_rec_struct dns_cache[], computer non_local_ip, char *hostname){
		int temp_index = (index-1) % (int) sizeof(&dns_cache);
		dns_cache[temp_index].ip_address = non_local_ip;
		strcpy(dns_cache[temp_index].cname, hostname);
		return temp_index;
}

// Convert from `computer` to dotted IP string format
char* comp_to_dotted_ip(computer comp){
    struct in_addr ip = { comp };
    return inet_ntoa(ip);
}
