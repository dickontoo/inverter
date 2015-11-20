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

int main(int argc, char *argv[])
{
	uint8_t b[103];
	char	invserial[16];
	int	l;
	struct inverter i;

	i.invserial = NULL;

	while (1) {
		l = read(STDIN_FILENO, b, sizeof(b));
		if (l == 0)
			break;
		if (l != 103)
			continue;

		memcpy(invserial, &b[0x11], 0x0e);
		invserial[0x0e] = '\0';

		i.dlserial = b[0x07]<<24 | b[0x06]<<16 | b[0x05]<<8 | b[0x04];
		i.invserial = invserial;

		i.temp = b[0x1f]<<8 | b[0x20];
		i.pv1v = b[0x21]<<8 | b[0x22];
		i.pv2v = b[0x23]<<8 | b[0x24];
		i.pv3v = b[0x25]<<8 | b[0x26];
		i.pv1i = b[0x27]<<8 | b[0x28];
		i.pv2i = b[0x29]<<8 | b[0x2a];
		i.pv3i = b[0x2b]<<8 | b[0x2c];
		i.p1i  = b[0x2d]<<8 | b[0x2e];
		i.p2i  = b[0x2f]<<8 | b[0x30];
		i.p3i  = b[0x31]<<8 | b[0x32];
		i.p1v  = b[0x33]<<8 | b[0x34];
		i.p2v  = b[0x35]<<8 | b[0x36];
		i.p3v  = b[0x37]<<8 | b[0x38];
		i.freq = b[0x39]<<8 | b[0x3a];
		i.p1p  = b[0x3b]<<8 | b[0x3c];
		i.p2p  = b[0x3d]<<8 | b[0x3e];
		i.p3p  = b[0x3f]<<8 | b[0x40];
		i.ttot = b[0x45]<<8 | b[0x46];
		i.tot  = b[0x49]<<8 | b[0x4a];

		printf("Datalogger serial: %u\n"
			"Inverter serial: %s\n"
			"Temperature:\t%5.01f degrees Celsius\n"
			"PV1 voltage:\t%5.01f V\n"
			"PV2 voltage:\t%5.01f V\n"
			"PV3 voltage:\t%5.01f V\n"
			"PV1 current:\t%5.01f A\n"
			"PV2 current:\t%5.01f A\n"
			"PV3 current:\t%5.01f A\n"
			"P1 voltage:\t%5.01f V\n"
			"P2 voltage:\t%5.01f V\n"
			"P3 voltage:\t%5.01f V\n"
			"Frequency:\t%6.02f Hz\n"
			"P1 power:\t%3d W\n"
			"P2 power:\t%3d W\n"
			"P3 power:\t%3d W\n"
			"Today's total:\t%6.02f kWh\n"
			"Total:\t\t%5.01f kWh\n"
			"\n",
			i.dlserial,
			i.invserial,
			((float) i.temp)/10,
			((float) i.pv1v)/10,
			((float) i.pv2v)/10,
			((float) i.pv3v)/10,
			((float) i.pv1i)/10,
			((float) i.pv2i)/10,
			((float) i.pv3i)/10,
			((float) i.p1v)/10,
			((float) i.p2v)/10,
			((float) i.p3v)/10,
			((float) i.freq)/100,	/* Yes... */
			i.p1p,
			i.p2p,
			i.p3p,
			((float) i.ttot)/100,
			((float) i.tot)/10
		      );
		fflush(stdout);
	}

	return 0;
}

