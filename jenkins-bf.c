/* Jenkins hash brute force.
 *
 * Copyright (C) 2015  Tomasz Kramkowski <tk@the-tk.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <error.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "hash.h"
#include "strinc.h"
#include "hashlist.h"

volatile bool running;

static void sigint(int sig __attribute__((unused)))
{
	running = false;
}

static void usage(FILE * f)
{
	extern char *program_invocation_name;

	fprintf(f,
		"Usage:\n"
		"    %s <hash file> [charset [maxsize [minsize]]]\n"
		"    %s - [charset [maxsize [minsize]]]\n"
		"\n"
		"Options:\n"
		"    charset: [uln_] sets of characters to use ((u)pper, "
		"(l)ower, (n)umbers, (_) underscores)\n"
		"    maxsize: [255] plaintext size to stop at\n"
		"    minsize: [1] plaintext size to start at\n",
		program_invocation_name,
		program_invocation_name);
}

int main(int argc, char **argv)
{
	size_t maxkey, minkey, keylen;

	if (argc < 2 || argc > 5) {
		fprintf(stderr, "Incorrect argument count.\n");
		usage(stderr);
		return 1;
	}

	puts("Reading hash list...");
	loadlist(argv[1]);
	puts("Complete.");

	if (argc > 2)
		setcharset(argv[2]);
	else
		setcharset("uln_");

	if (argc > 3)
		maxkey = strtoul(argv[3], NULL, 10);
	else
		maxkey = 255;

	if (argc > 4)
		minkey = strtoul(argv[4], NULL, 10);
	else
		minkey = 1;

	if (maxkey < minkey)
		error(EXIT_FAILURE, 0, "Maximum key length is less than minimum key length.");
	if (minkey < 1)
		error(EXIT_FAILURE, 0, "Minimum key length cannot be less than 1.");

	char key[maxkey + 1];
	memset(key, '\0', maxkey + 1);
	strinc(key);
	memset(key, key[0], minkey);
	keylen = minkey;

	running = true;
	signal(SIGINT, sigint);

	do {
		uint32_t h = hash(key, keylen, 0xABCDEF00);

		if (inlist(h))
			printf("%" PRIu32 " -> %s\n", h, key);
	} while (keylen = strinc(key), keylen < maxkey && running);

	freelist();

	return 0;
}
