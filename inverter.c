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

int parsepacket(struct inverter *i, uint8_t *b, int l)
{
	if (l == 0)
		return -1;
	if (l != 103)
		return -1;

	i->parsetime = time(NULL);

	if (i->invserial) {
		memcpy(i->invserial, &b[0x11], 0x0e);
		i->invserial[0x0e] = '\0';
	}

	i->dlserial = b[0x07]<<24 | b[0x06]<<16 | b[0x05]<<8 | b[0x04];

	i->temp = b[0x1f]<<8 | b[0x20];
	i->pv1v = b[0x21]<<8 | b[0x22];
	i->pv2v = b[0x23]<<8 | b[0x24];
	i->pv3v = b[0x25]<<8 | b[0x26];
	i->pv1i = b[0x27]<<8 | b[0x28];
	i->pv2i = b[0x29]<<8 | b[0x2a];
	i->pv3i = b[0x2b]<<8 | b[0x2c];
	i->l1i  = b[0x2d]<<8 | b[0x2e];
	i->l2i  = b[0x2f]<<8 | b[0x30];
	i->l3i  = b[0x31]<<8 | b[0x32];
	i->l1v  = b[0x33]<<8 | b[0x34];
	i->l2v  = b[0x35]<<8 | b[0x36];
	i->l3v  = b[0x37]<<8 | b[0x38];
	i->freq = b[0x39]<<8 | b[0x3a];
	i->l1p  = b[0x3b]<<8 | b[0x3c];
	i->l2p  = b[0x3d]<<8 | b[0x3e];
	i->l3p  = b[0x3f]<<8 | b[0x40];
	i->ttot = b[0x45]<<8 | b[0x46];
	i->tot  = b[0x49]<<8 | b[0x4a];

	return 0;
}



int genstring(uint32_t dlserial, uint8_t *s, int slen)
{
	if (slen < 16)
		return -1;

	/* Fixed four bytes: */
	s[ 0] = 0x68;
	s[ 1] = 0x02;
	s[ 2] = 0x40;
	s[ 3] = 0x30;

	/* BE serial number, twice: */
	s[ 4] = (dlserial & 0x000000ff) >>  0;
	s[ 5] = (dlserial & 0x0000ff00) >>  8;
	s[ 6] = (dlserial & 0x00ff0000) >> 16;
	s[ 7] = (dlserial & 0xff000000) >> 24;

	s[ 8] = (dlserial & 0x000000ff) >>  0;
	s[ 9] = (dlserial & 0x0000ff00) >>  8;
	s[10] = (dlserial & 0x00ff0000) >> 16;
	s[11] = (dlserial & 0xff000000) >> 24;

	s[12] = 0x01;
	s[13] = 0x00;
	/* No, I don't know either: */
	s[14] = (115 + (s[4] + s[5] + s[6] + s[7]) * 2) & 0xff;
	s[15] = 0x16;

	return 16;
}
