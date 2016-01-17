/* String "incrementing"
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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "strinc.h"

uint8_t lenchars = 0;

char strchars[128] = { '\0' };
unsigned char revchars[128] = { 0 };

void setcharset(char *set)
{
	static const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	static const char lower[] = "abcdefghijklmnopqrstuvwxyz";
	static const char num[] = "0123456789";	/* 10 */

	if (strchr(set, 'u')) {
		memcpy(strchars + lenchars, upper, 26);
		lenchars += 26;
	}

	if (strchr(set, 'l')) {
		memcpy(strchars + lenchars, lower, 26);
		lenchars += 26;
	}

	if (strchr(set, 'n')) {
		memcpy(strchars + lenchars, num, 10);
		lenchars += 10;
	}

	if (strchr(set, '_')) {
		*(strchars + lenchars) = '_';
		lenchars += 1;
	}

	for (uint8_t i = 0; i < lenchars; i++)
		revchars[(unsigned char)strchars[i]] = i;
}

size_t strinc(char *str)
{
	size_t len = strlen(str);
	bool carry;

	do {
		carry = false;

		if (revchars[(unsigned char)*str] + 1 >= lenchars) {
			carry = true;
			*str = strchars[0];
		} else {
			*str = strchars[revchars[(unsigned char)*str] + 1];
		}

		str++;
	} while (carry && *str);

	if (carry) {
		*(str++) = strchars[0];
		*str = '\0';
		len++;
	}

	return len;
}
