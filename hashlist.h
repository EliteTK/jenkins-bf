#ifndef HASHLIST_H
#define HASHLIST_H

#include <stdbool.h>
#include <stdint.h>

void loadlist(char *fname);
void freelist(void);
bool inlist(uint32_t hash);

#endif /* HASHLIST_H */
