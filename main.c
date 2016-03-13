/* inverter.c */
/*
 * (C) 2015 Dickon Hood <dickon@fluff.org>
 *
 * Parse the inverter datalogger blobs, and produce something useful
 * from it.
 */

#include <stdio.h>
#include <unistd.h>
#include "inverter.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <sys/time.h>

void usage(char *name)
{
	printf("Usage:\n"
		"\t%s [serial [-u|-l] host port frequency]\n"
		"\n"
		"or pass a packet in on stdin\n\n",
		name);
}



int dosocket(int argc, char *argv[], uint8_t *dlmagic,
	int *dlmagiclen, int *sleeptime, int *lfd)
{
	int fd;
	int i = 1;
	struct addrinfo *a;
	int flag = 1;
	int listening = 0;
	struct addrinfo hints;
	struct timeval timeout;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;     /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM; /* Stream socket */
	hints.ai_flags = AI_PASSIVE;     /* For wildcard IP address */
	hints.ai_protocol = 0;           /* Any protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	if (argc < 4 || strcmp(argv[1], "-h") == 0 ||
			strcmp(argv[1], "--help") == 0) {
		usage(argv[0]);
		exit(0);
	}

	*dlmagiclen = genstring(atoi(argv[i++]), dlmagic, *dlmagiclen);
	if (argv[i][0] == '-') {
		if (argv[i][1] == 'u') {
			hints.ai_socktype = SOCK_DGRAM;
		} else if (argv[i][1] == 'l') {
			listening = 1;
		} else {
			fprintf(stderr, "Choose one of -l or -u\n");
			exit(1);
		}
		i++;
	}

	printf("Sleeptime: %s\n", argv[i+2]);
	*sleeptime = atoi(argv[i+2]);
	printf("%s:%s\n", argv[i], argv[i+1]);
	if ((fd = getaddrinfo(listening ? NULL : argv[i],
			argv[i+1], &hints, &a)) != 0) {
		fprintf(stderr, "getaddrinfoL %s", gai_strerror(fd));
		exit(1);
	}
	fd = socket(a->ai_family, a->ai_socktype, a->ai_protocol);
	if (fd == -1) {
		perror("socket");
		exit(1);
	}
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY,
			(char *) &flag, sizeof(int));
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
			(char *) &flag, sizeof(int));
	setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE,
			(char *) &flag, sizeof(int));
	memset(&timeout, 0, sizeof(timeout));
	timeout.tv_sec = 10;
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));
	setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout, sizeof(timeout));

	if (listening) {
		if (bind(fd, a->ai_addr, a->ai_addrlen) == -1) {
			perror("bind");
			exit(1);
		}
		*lfd = fd;
		listen(fd, 16);
		fd = accept(fd, a->ai_addr, &a->ai_addrlen);
	} else {
		if (connect(fd, a->ai_addr, a->ai_addrlen) == -1) {
			perror("connect");
			exit(1);
		}
	}

	return fd;
}



int main(int argc, char *argv[])
{
	uint8_t		b[103];
	char		invserial[16];
	int		l;
	struct inverter i;
	struct		tm *tm;
	char		atime[32];
	int		fd;
	int		sleeptime = 0;
	uint8_t		dlmagic[16];
	int		dlmagiclen = 0;
	int		lfd = -1;

	fd = STDIN_FILENO;

	if (argc > 1) {
		dlmagiclen = sizeof(dlmagic);
		fd = dosocket(argc, argv, dlmagic, &dlmagiclen, &sleeptime, &lfd);
	}

	if (fd == -1)
		exit(1);

	i.invserial = invserial;
	argc--;

	do {
		if (argc) {
			sleep(sleeptime);
			write(fd, dlmagic, dlmagiclen);
		}

		l = read(fd, b, sizeof(b));
		if (l == 0)
			break;
		if (l != 103)
			continue;
		if (l == -1 && lfd != -1) {
			close(fd);
			while (fd == -1)
				fd = accept(lfd, NULL, NULL);
			continue;
		}

		if (parsepacket(&i, b, l) != 0)
			continue;

		tm = gmtime(&i.parsetime);
		strftime(atime, sizeof(atime), "%Y%m%dT%H%M%SZ", tm);

		printf("Time:\t\t%s\n"
			"Datalogger serial: %u\n"
			"Inverter serial: %s\n"
			"Temperature:\t%5.01f degrees Celsius\n"
			"PV1 voltage:\t%5.01f V\n"
			"PV2 voltage:\t%5.01f V\n"
			"PV3 voltage:\t%5.01f V\n"
			"PV1 current:\t%5.01f A\n"
			"PV2 current:\t%5.01f A\n"
			"PV3 current:\t%5.01f A\n"
			"L1 current:\t%5.01f A\n"
			"L2 current:\t%5.01f A\n"
			"L3 current:\t%5.01f A\n"
			"L1 voltage:\t%5.01f V\n"
			"L2 voltage:\t%5.01f V\n"
			"L3 voltage:\t%5.01f V\n"
			"Frequency:\t%6.02f Hz\n"
			"L1 power:\t%3d W\n"
			"L2 power:\t%3d W\n"
			"L3 power:\t%3d W\n"
			"Today's total:\t%6.02f kWh\n"
			"Total:\t\t%5.01f kWh\n"
			"\n",
			atime,
			i.dlserial,
			i.invserial,
			((float) i.temp)/10,
			((float) i.pv1v)/10,
			((float) i.pv2v)/10,
			((float) i.pv3v)/10,
			((float) i.pv1i)/10,
			((float) i.pv2i)/10,
			((float) i.pv3i)/10,
			((float) i.l1i)/10,
			((float) i.l2i)/10,
			((float) i.l3i)/10,
			((float) i.l1v)/10,
			((float) i.l2v)/10,
			((float) i.l3v)/10,
			((float) i.freq)/100,	/* Yes... */
			i.l1p,
			i.l2p,
			i.l3p,
			((float) i.ttot)/100,
			((float) i.tot)/10
		      );
		fflush(stdout);
/*
 * It dumps out a 31-byte binary blob saying it's done it.  Read and discard.
 */
		if (argc)
			read(fd, b, 31);
	} while (argc);

	return 0;
}

