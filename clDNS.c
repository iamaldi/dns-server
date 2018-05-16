/* clDNS.c */

#include <stdlib.h>
#include <stdio.h>
#include <cnaiapi.h>
#include <unistd.h>
#include <string.h>

#define BUFFSIZE 256

int readln(char *, int);
void errorHandling(char *argv[]);

/*-----------------------------------------------------------------------
 *
 * Program: DNS Client
 * Purpose: connect with DNS server, make DNS query, receive DNS response
 * Usage:   ./clDNS dns-server port hostname
 *
 *-----------------------------------------------------------------------
 */
int main(int argc, char *argv[]){

	computer dnsServer;
	appnum port;
  char *hostname;
	connection conn;
	char buff[BUFFSIZE];
	int len;

	if (argc < 3 || argc > 4) {
		errorHandling(argv);
	}

	// Convert the DNS server hostname / IP to binary format `computer`
	dnsServer = cname_to_comp(argv[1]);

  // DNS Server hostname / IP could not be resolved
	if (dnsServer == -1){
    (void) fprintf(stderr, "Error: could not resolve DNS server at `%s` \n", argv[1]);
		exit(1);
  }

  // Convert port number to appnum and get hostname
	if (argc == 4) {
		port = (appnum) atoi(argv[2]);
    hostname = argv[3];
  } else {
    // Incorrect number of command line arguments
    errorHandling(argv);
  }

  // Establish connection with DNS Server
	conn = make_contact(dnsServer, port);

  // Connection failed
	if (conn < 0) {
    (void) fprintf(stderr, "Error: could not establish connection with DNS server at `%s` on port `%s`\n", argv[1], argv[2]);
      exit(1);
  }

  // Calculate the length of `hostname`
	len = strlen(hostname);

	// Make a DNS query providing `hostname`
	(void) send(conn, hostname, len, 0);

	// Clear buffer
	memset(buff, 0, BUFFSIZE);

	// Receive DNS response
	if(recv(conn, buff, BUFFSIZE, 0)){

  // Write DNS response on standard output
	(void) fprintf(stdout,"%s", buff);
	fflush(stdout);
}

  // DNS query complete, terminate connection with DNS server
  (void) send_eof(conn);
  (void) printf("\n");

	return 0;
}

// Handle usage errors
void errorHandling(char *argv[]){
  (void) fprintf(stderr, "Usage: %s dns-server port hostname\n", argv[0]);
  (void) fprintf(stderr, "\nExample: %s 127.0.0.1 20000 google.com\n", argv[0]);
  exit(1);
}
