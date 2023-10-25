#ifndef _INSERT_H
#define _INSERT_H
#include <stdint.h>

// Insert pStr2 into pStr1 's location.
// For example
// char *pStr = NULL;
// strinsert( &pStr, "Happy", 1, "ABC" );
// The output string should be HABCappy
int32_t strinsert(char **ppResult, const char *pStr1, int32_t location,
                  const char *pStr2);
#endif