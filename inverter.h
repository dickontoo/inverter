/* inverter.h */
/*
 * (C) 2015 Dickon Hood <dickon@fluff.org>
 *
 * GPLv2.
 */

#include <stdint.h>


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
	uint16_t	p1i;
	uint16_t	p2i;
	uint16_t	p3i;
	uint16_t	p1v;
	uint16_t	p2v;
	uint16_t	p3v;
	uint16_t	freq;		/* centiHz */
	uint16_t	p1p;		/* W */
	uint16_t	p2p;		/* W */
	uint16_t	p3p;		/* W */
	uint16_t	ttot;		/* centikilowatthours */
	uint32_t	tot;
};

