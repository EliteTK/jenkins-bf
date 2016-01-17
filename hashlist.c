/* List of hashes and searching
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

#include <errno.h>
#include <error.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "hashlist.h"

struct ht_node {
	struct ht_node *left, *right;
	uint32_t hash;
};

uint32_t *hlist;
int hcount;

static int hashcmp(int32_t a, int32_t b)
{
	return ((a < b) * -1) + ((a > b) * 1);
}

static int shashcmp(const void *pa, const void *pb)
{
	const int32_t *a = pa, *b = pb;

	return hashcmp(*a, *b);
}

static struct ht_node *ht_insert(struct ht_node *t, uint32_t h)
{
	int cond;

	if (!t) {
		if (t = malloc(sizeof *t), !t)
			error(EXIT_FAILURE, errno, "Could not allocate memory");

		t->hash = h;
		t->left = t->right = NULL;
		hcount++;
	} else if (cond = hashcmp(h, t->hash), cond < 0) {
		t->left = ht_insert(t->left, h);
	} else if (cond > 0) {
		t->right = ht_insert(t->right, h);
	}

	return t;
}

static int ht_flatten(struct ht_node *tree, uint32_t *arr, int size, int i)
{
	if (!tree || i >= size)
		return i;

	i = ht_flatten(tree->left, arr, size, i);
	arr[i] = tree->hash;
	i = ht_flatten(tree->right, arr, size, i + 1);

	return i;
}

static void ht_free(struct ht_node *tree)
{
	if (!tree)
		return;

	ht_free(tree->left);
	ht_free(tree->right);

	free(tree);
}

void loadlist(char *fname)
{
	FILE *f;
	struct ht_node *htree = NULL;
	int lno = 0;
	char line[2048];

	hcount = 0;

	if (!strcmp(fname, "-"))
		f = stdin;
	else if (f = fopen(fname, "r"), !f)
		error(EXIT_FAILURE, errno, "Could not open `%s'", fname);

	while (!feof(f) && !ferror(f)) {
		lno++;
		unsigned long val;

		if (!fgets(line, sizeof line, f))
			continue;

		if (strlen(line) == sizeof line - 1 && !feof(f) && !ferror(f)
		    && line[strlen(line) - 1] != '\n') {
			int c;

			fprintf(stderr, "Line %d could be too long\n", lno);
			while (c = fgetc(f), c != '\n' && c != EOF);
		}
		errno = 0;
		val = strtoul(line, NULL, 0);
		if (errno) {
			fprintf(stderr, "Line %d could not be converted\n", lno);
			continue;
		}
		htree = ht_insert(htree, val);
	}

	if (strcmp(fname, "-"))
		fclose(f);

	hlist = malloc(sizeof *hlist * hcount);
	if (!hlist)
		error(EXIT_FAILURE, errno, "Could not allocate memory");
	if (ht_flatten(htree, hlist, hcount, 0) != hcount)
		error(EXIT_FAILURE, 0, "Could not flatten hash tree");

	ht_free(htree);
}

void freelist(void)
{
	free(hlist);
}

bool inlist(uint32_t hash)
{
	uint32_t *found;

	found = bsearch(&hash, hlist, hcount, sizeof *hlist,
				     shashcmp);

	if (!found)
		return false;

	return *found == hash;
}
