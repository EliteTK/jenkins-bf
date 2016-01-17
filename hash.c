/*
 * A reworked version of the public domain code found at:
 * http://www.burtleburtle.net/bob/hash/doobs.html
 */

#include <stdint.h>

#include "hash.h"

static inline void mix(uint32_t * a, uint32_t * b, uint32_t * c)
{
	*a = *a - *b;
	*a = *a - *c;
	*a = *a ^ (*c >> 13);
	*b = *b - *c;
	*b = *b - *a;
	*b = *b ^ (*a << 8);
	*c = *c - *a;
	*c = *c - *b;
	*c = *c ^ (*b >> 13);
	*a = *a - *b;
	*a = *a - *c;
	*a = *a ^ (*c >> 12);
	*b = *b - *c;
	*b = *b - *a;
	*b = *b ^ (*a << 16);
	*c = *c - *a;
	*c = *c - *b;
	*c = *c ^ (*b >> 5);
	*a = *a - *b;
	*a = *a - *c;
	*a = *a ^ (*c >> 3);
	*b = *b - *c;
	*b = *b - *a;
	*b = *b ^ (*a << 10);
	*c = *c - *a;
	*c = *c - *b;
	*c = *c ^ (*b >> 15);
}

uint32_t hash(char *k, uint32_t length, uint32_t initval)
{
	uint32_t a, b, c, len;

	len = length;
	a = b = 0x9e3779b9;
	c = initval;

	while (len >= 12) {
		a = a + (k[0] + ((uint32_t) k[1] << 8) +
			 ((uint32_t) k[2] << 16) + ((uint32_t) k[3] << 24));
		b = b + (k[4] + ((uint32_t) k[5] << 8) +
			 ((uint32_t) k[6] << 16) + ((uint32_t) k[7] << 24));
		c = c + (k[8] + ((uint32_t) k[9] << 8) +
			 ((uint32_t) k[10] << 16) + ((uint32_t) k[11] << 24));
		mix(&a, &b, &c);
		k = k + 12;
		len = len - 12;
	}

	c = c + length;
	switch (len) {
	case 11:
		c = c + ((uint32_t) k[10] << 24);
	case 10:
		c = c + ((uint32_t) k[9] << 16);
	case 9:
		c = c + ((uint32_t) k[8] << 8);
	case 8:
		b = b + ((uint32_t) k[7] << 24);
	case 7:
		b = b + ((uint32_t) k[6] << 16);
	case 6:
		b = b + ((uint32_t) k[5] << 8);
	case 5:
		b = b + k[4];
	case 4:
		a = a + ((uint32_t) k[3] << 24);
	case 3:
		a = a + ((uint32_t) k[2] << 16);
	case 2:
		a = a + ((uint32_t) k[1] << 8);
	case 1:
		a = a + k[0];
	}

	mix(&a, &b, &c);

	return c;
}
