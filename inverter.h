/* inverter.h */
/*
 * (C) 2015 Dickon Hood <dickon@fluff.org>
 *
 * GPLv2.
 */

#include <stdint.h>
#include <time.h>



/* All values are deci-$unit unless otherwise specified. */
struct inverter {
	uint32_t	dlserial;
	char		*invserial;
	uint16_t	temp;
	uint16_t	pv1v;
	uint16_t	pv2v;
	uint16_t	pv3v;
	uint16_t	pv1i;
	uint16_t	pv2i;
	uint16_t	pv3i;
	uint16_t	l1i;
	uint16_t	l2i;
	uint16_t	l3i;
	uint16_t	l1v;
	uint16_t	l2v;
	uint16_t	l3v;
	uint16_t	freq;		/* centiHz */
	uint16_t	l1p;		/* W */
	uint16_t	l2p;		/* W */
	uint16_t	l3p;		/* W */
	uint16_t	ttot;		/* centikilowatthours */
	uint32_t	tot;
	time_t		parsetime;
};



int genstring(uint32_t dlserial, uint8_t *string, int slen);
int parsepacket(struct inverter *i, uint8_t *buf, int len);
