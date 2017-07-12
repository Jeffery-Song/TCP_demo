// test TCP functions in sock.h, which comes from "hello_world_rc_write"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <endian.h>
#include <byteswap.h>
#include <getopt.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include "sock.h"
char *msgs = "hello client, I'm server\n";
char *msgc = "hello server, I'm client\n";

int main(int argc, char *argv[]){

	int sock;
	uint32_t port = 18515;
	char *server_name = NULL;
	char tmp[30] = "";

	while (1) {
		int c;
		struct option opt[] = {
			{ .name = "port", .has_arg = 1, .val = 'p' },
			{ .name = NULL, .has_arg = 0, .val = '\0'}
		};

		c = getopt_long(argc, argv, "p", opt, NULL);

		if(c == -1)
			break;

		switch (c) {
		case 'p':
			port = strtoul(optarg, NULL, 0);
			if(port < 0 || port > 65535) {
				return 1;
			}
			break;

		default:
			return 1;
		}
	}
	if(optind == argc -1)
		server_name = strdup(argv[optind]);
	else if(optind < argc) {
		return 1;
	}

	if(server_name)
		sock = sock_client_connect(server_name, port);
	else 
		sock = sock_daemon_connect(port);
	if(sock < 0){
		fprintf(stderr, "Couldn't built TCP connection to server %s, port %d\n", server_name, port);
		return 1;
	}

	int sync_rst;
	if(server_name)
		sync_rst = sock_sync_data(sock, !server_name, strlen(msgc)*sizeof(char), msgc, tmp);
	else
		sync_rst = sock_sync_data(sock, !server_name, strlen(msgs)*sizeof(char), msgs, tmp); 
	if (sync_rst < 0) {
		fprintf(stderr, "Couldn't exchange message\n");
		return 1;
	}

	printf("Message is %s\n", tmp);

	close(sock);
	free(server_name);
	return 0;
}